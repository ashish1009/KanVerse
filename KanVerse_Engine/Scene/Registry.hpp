/*!
 * @file Registry.hpp
 * @brief Generational entity manager and multi-component query orchestrator.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#pragma once

#include "Scene/EntityID.hpp"
#include "Scene/SparseSet.hpp"
#include "Core/Diagnostics/Assert.hpp"
#include <algorithm>
#include <limits>
#include <memory>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace KanVerse {
  
  /*!
   * @class Registry
   * @brief Manages generational entity lifecycles, component pools, and cache-friendly query views.
   */
  class Registry {
  public:
    Registry() = default;
    
    ~Registry() noexcept {
      Clear();
    }
    
    Registry(const Registry&) = delete;
    Registry& operator=(const Registry&) = delete;
    Registry(Registry&&) noexcept = default;
    Registry& operator=(Registry&&) noexcept = default;
    
    /*!
     * @brief Allocates a new entity identifier, recycling freed slots or expanding the generation pool.
     * @return Valid 64-bit `EntityID` ($O(1)$ complexity).
     */
    [[nodiscard]] EntityID Create() {
      std::uint32_t index = 0;
      if (!m_FreeList.empty()) {
        index = m_FreeList.back();
        m_FreeList.pop_back();
      } else {
        index = static_cast<std::uint32_t>(m_Generations.size());
        m_Generations.push_back(0);
      }
      return ECS::CreateID(index, m_Generations[index]);
    }
    
    /*!
     * @brief Destroys an entity, removes all its attached components, and increments its slot generation.
     * @param entity Target 64-bit entity handle.
     */
    void Destroy(EntityID entity) {
      if (!IsValid(entity)) {
        return;
      }
      
      for (auto& [type, pool] : m_Pools) {
        if (pool->Contains(entity)) {
          pool->Remove(entity);
        }
      }
      
      const std::uint32_t index = ECS::GetIndex(entity);
      ++m_Generations[index];
      m_FreeList.push_back(index);
    }
    
    /*!
     * @brief Verifies whether an entity handle points to a live, generation-matched slot.
     * @param entity Target 64-bit entity handle.
     * @return True if alive and valid ($O(1)$ complexity).
     */
    [[nodiscard]] bool IsValid(EntityID entity) const noexcept {
      const std::uint32_t index = ECS::GetIndex(entity);
      if (index >= m_Generations.size()) {
        return false;
      }
      return m_Generations[index] == ECS::GetGeneration(entity);
    }
    
    /*!
     * @brief Constructs and attaches a component in-place on an entity.
     * @tparam T Component type.
     * @tparam Args Constructor argument types for `T`.
     * @param entity Target 64-bit entity handle.
     * @param args Forwarded constructor arguments.
     * @return Mutable reference to the emplaced component.
     */
    template <typename T, typename... Args>
    T& Emplace(EntityID entity, Args&&... args) {
      KV_CORE_ASSERT(IsValid(entity), "[KanVerse ECS] Attempted to emplace component on invalid EntityID!");
      return GetPool<T>()->Emplace(entity, std::forward<Args>(args)...);
    }
    
    /*!
     * @brief Retrieves a mutable reference to an entity's component.
     * @tparam T Component type.
     * @param entity Target 64-bit entity handle.
     * @return Mutable reference to `T`.
     */
    template <typename T>
    [[nodiscard]] T& Get(EntityID entity) noexcept {
      KV_CORE_ASSERT(IsValid(entity), "[KanVerse ECS] Attempted to get component on invalid EntityID!");
      return GetPool<T>()->Get(entity);
    }
    
    /*!
     * @brief Retrieves a const reference to an entity's component.
     * @tparam T Component type.
     * @param entity Target 64-bit entity handle.
     * @return Const reference to `T`.
     */
    template <typename T>
    [[nodiscard]] const T& Get(EntityID entity) const noexcept {
      KV_CORE_ASSERT(IsValid(entity), "[KanVerse ECS] Attempted to get component on invalid EntityID!");
      return GetPool<T>()->Get(entity);
    }
    
    /*!
     * @brief Checks whether an entity currently possesses a specific component.
     * @tparam T Component type.
     * @param entity Target 64-bit entity handle.
     * @return True if the component is attached ($O(1)$ complexity).
     */
    template <typename T>
    [[nodiscard]] bool Has(EntityID entity) const noexcept {
      if (!IsValid(entity)) {
        return false;
      }
      const auto it = m_Pools.find(std::type_index(typeid(T)));
      if (it == m_Pools.end()) {
        return false;
      }
      return it->second->Contains(entity);
    }
    
    /*!
     * @brief Removes a component of type `T` from an entity.
     * @tparam T Component type.
     * @param entity Target 64-bit entity handle.
     */
    template <typename T>
    void Remove(EntityID entity) {
      KV_CORE_ASSERT(IsValid(entity), "[KanVerse ECS] Attempted to remove component from invalid EntityID!");
      const auto it = m_Pools.find(std::type_index(typeid(T)));
      if (it != m_Pools.end()) {
        it->second->Remove(entity);
      }
    }
    
    /*!
     * @brief Wipes all entities, generations, free lists, and component pools.
     */
    void Clear() noexcept {
      for (auto& [type, pool] : m_Pools) {
        pool->Clear();
      }
      m_Pools.clear();
      m_Generations.clear();
      m_FreeList.clear();
    }
    
    /*!
     * @class View
     * @brief High-throughput query view iterating across entities possessing all specified components.
     * @tparam Components Variadic pack of required component types.
     */
    template <typename... Components>
    class View {
    public:
      explicit View(Registry& registry) noexcept
      : m_Registry(registry), m_LeadingPool(FindSmallestPool()) {}
      
      /*!
       * @brief Iterates across all matching entities invoking `func(EntityID, Components&...)`.
       * @tparam Func Callable matching `void(EntityID, Components&...)`.
       */
      template <typename Func>
      void Each(Func&& func) {
        if (m_LeadingPool == nullptr) {
          return;
        }
        const auto& entities = m_LeadingPool->GetEntities();
        for (const EntityID entity : entities) {
          if (m_Registry.IsValid(entity) && (m_Registry.Has<Components>(entity) && ...)) {
            func(entity, m_Registry.Get<Components>(entity)...);
          }
        }
      }
      
      /*!
       * @brief Iterates across matching components invoking `func(Components&...)` without the `EntityID`.
       * @tparam Func Callable matching `void(Components&...)`.
       */
      template <typename Func>
      void Each(Func&& func) requires(!std::is_invocable_v<Func, EntityID, Components&...>) {
        if (m_LeadingPool == nullptr) {
          return;
        }
        const auto& entities = m_LeadingPool->GetEntities();
        for (const EntityID entity : entities) {
          if (m_Registry.IsValid(entity) && (m_Registry.Has<Components>(entity) && ...)) {
            func(m_Registry.Get<Components>(entity)...);
          }
        }
      }
      
    private:
      [[nodiscard]] ISparseSet* FindSmallestPool() noexcept {
        ISparseSet* smallest = nullptr;
        std::size_t minSize  = std::numeric_limits<std::size_t>::max();
        
        auto checkPool = [this, &smallest, &minSize]<typename T>() {
          const auto it = m_Registry.m_Pools.find(std::type_index(typeid(T)));
          if (it != m_Registry.m_Pools.end()) {
            if (it->second->Size() < minSize) {
              minSize  = it->second->Size();
              smallest = it->second.get();
            }
          } else {
            minSize  = 0;
            smallest = nullptr;
          }
        };
        
        (checkPool.template operator()<Components>(), ...);
        return smallest;
      }
      
      Registry&   m_Registry;
      ISparseSet* m_LeadingPool{ nullptr };
    };
    
    /*!
     * @brief Constructs a multi-component query view over the registry.
     * @tparam Components Required component types.
     * @return Configured `View<Components...>` instance.
     */
    template <typename... Components>
    [[nodiscard]] View<Components...> GetView() noexcept {
      return View<Components...>(*this);
    }
    
  private:
    template <typename T>
    [[nodiscard]] SparseSet<T>* GetPool() {
      const auto typeIdx = std::type_index(typeid(T));
      const auto it      = m_Pools.find(typeIdx);
      if (it == m_Pools.end()) {
        auto newPool         = std::make_unique<SparseSet<T>>();
        SparseSet<T>* rawPtr = newPool.get();
        m_Pools.emplace(typeIdx, std::move(newPool));
        return rawPtr;
      }
      return static_cast<SparseSet<T>*>(it->second.get());
    }
    
    template <typename T>
    [[nodiscard]] const SparseSet<T>* GetPool() const noexcept {
      const auto typeIdx = std::type_index(typeid(T));
      const auto it      = m_Pools.find(typeIdx);
      KV_CORE_ASSERT(it != m_Pools.end(), "[KanVerse ECS] Component pool does not exist!");
      return static_cast<const SparseSet<T>*>(it->second.get());
    }
    
    std::vector<std::uint32_t>                                      m_Generations;
    std::vector<std::uint32_t>                                      m_FreeList;
    std::unordered_map<std::type_index, std::unique_ptr<ISparseSet>> m_Pools;
  };
  
} // namespace KanVerse
