/*!
 * @file SparseSet.hpp
 * @brief Paged sparse-set container managing contiguous component pools with automated memory tracking.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#pragma once

#include "Scene/EntityID.hpp"
#include "Core/Diagnostics/Assert.hpp"
#include "Core/Memory/MemoryTracking.hpp"
#include <cstdint>
#include <utility>
#include <vector>

namespace KanVerse {
  
  /*!
   * @class ISparseSet
   * @brief Type-erased base interface enabling polymorphic component pool cleanup and iteration.
   *
   * @note Thread-safety: Not thread-safe for concurrent mutations on the same pool.
   */
  class ISparseSet {
  public:
    virtual ~ISparseSet() noexcept = default;
    
    /*!
     * @brief Queries whether an entity index exists within this component pool.
     * @param entity Target 64-bit entity handle.
     * @return True if the component is assigned ($O(1)$ time complexity).
     */
    [[nodiscard]] virtual bool Contains(EntityID entity) const noexcept = 0;
    
    /*!
     * @brief Erases an entity's component using swap-and-pop $O(1)$ removal.
     * @param entity Target 64-bit entity handle.
     */
    virtual void Remove(EntityID entity) = 0;
    
    /*!
     * @brief Releases all allocated components and sparse index pages.
     */
    virtual void Clear() noexcept = 0;
    
    /*!
     * @brief Queries the active component count in the dense pool.
     * @return Number of tightly packed components ($O(1)$ time complexity).
     */
    [[nodiscard]] virtual std::size_t Size() const noexcept = 0;
    
    /*!
     * @brief Accesses the contiguous dense entity array for cache-friendly linear iteration.
     * @return Const reference to the dense `EntityID` vector.
     */
    [[nodiscard]] virtual const std::vector<EntityID>& GetEntities() const noexcept = 0;
  };
  
  /*!
   * @class SparseSet
   * @brief Paged sparse array paired with dense contiguous component buffers.
   *
   * @tparam Component Concrete component type stored contiguously in `m_DenseComponents`.
   * @note Uses 4096-element sparse pages to avoid monolithic sparse array allocations while
   *       maintaining strict $O(1)$ lookup, insertion, and swap-and-pop deletion.
   */
  template <typename Component>
  class SparseSet final : public ISparseSet {
  public:
    static constexpr std::size_t   PageSize     = 4096;
    static constexpr std::uint32_t InvalidIndex = 0xFFFFFFFFU;
    
    SparseSet() = default;
    
    ~SparseSet() noexcept override {
      Clear();
    }
    
    SparseSet(const SparseSet&) = delete;
    SparseSet& operator=(const SparseSet&) = delete;
    
    SparseSet(SparseSet&& other) noexcept
    : m_SparsePages(std::move(other.m_SparsePages)),
    m_DenseEntities(std::move(other.m_DenseEntities)),
    m_DenseComponents(std::move(other.m_DenseComponents)),
    m_MemoryGuard(std::move(other.m_MemoryGuard)) {}
    
    SparseSet& operator=(SparseSet&& other) noexcept {
      if (this != &other) {
        Clear();
        m_SparsePages     = std::move(other.m_SparsePages);
        m_DenseEntities   = std::move(other.m_DenseEntities);
        m_DenseComponents = std::move(other.m_DenseComponents);
        m_MemoryGuard     = std::move(other.m_MemoryGuard);
      }
      return *this;
    }
    
    /*!
     * @brief Constructs and assigns a component in-place for the target entity.
     * @tparam Args Constructor argument types forwarded to `Component`.
     * @param entity Target 64-bit entity handle.
     * @param args Arguments forwarded to the component constructor.
     * @return Mutable reference to the newly emplaced component ($O(1)$ amortized).
     */
    template <typename... Args>
    Component& Emplace(EntityID entity, Args&&... args) {
      KV_CORE_ASSERT(!Contains(entity), "[KanVerse ECS] Entity already possesses this component!");
      
      const std::uint32_t entityIndex = ECS::GetIndex(entity);
      AssurePage(entityIndex);
      
      const std::size_t pageIndex     = entityIndex / PageSize;
      const std::size_t offset        = entityIndex % PageSize;
      const std::uint32_t denseIndex  = static_cast<std::uint32_t>(m_DenseEntities.size());
      
      m_SparsePages[pageIndex][offset] = denseIndex;
      m_DenseEntities.push_back(entity);
      m_DenseComponents.emplace_back(std::forward<Args>(args)...);
      
      UpdateMemoryTracking();
      return m_DenseComponents.back();
    }
    
    /*!
     * @brief Retrieves a mutable reference to an entity's component.
     * @param entity Target 64-bit entity handle.
     * @return Mutable reference to `Component` ($O(1)$ complexity).
     */
    [[nodiscard]] Component& Get(EntityID entity) noexcept {
      KV_CORE_ASSERT(Contains(entity), "[KanVerse ECS] Attempted to query non-existent component!");
      const std::uint32_t entityIndex = ECS::GetIndex(entity);
      const std::size_t pageIndex     = entityIndex / PageSize;
      const std::size_t offset        = entityIndex % PageSize;
      return m_DenseComponents[m_SparsePages[pageIndex][offset]];
    }
    
    /*!
     * @brief Retrieves a const reference to an entity's component.
     * @param entity Target 64-bit entity handle.
     * @return Const reference to `Component` ($O(1)$ complexity).
     */
    [[nodiscard]] const Component& Get(EntityID entity) const noexcept {
      KV_CORE_ASSERT(Contains(entity), "[KanVerse ECS] Attempted to query non-existent component!");
      const std::uint32_t entityIndex = ECS::GetIndex(entity);
      const std::size_t pageIndex     = entityIndex / PageSize;
      const std::size_t offset        = entityIndex % PageSize;
      return m_DenseComponents[m_SparsePages[pageIndex][offset]];
    }
    
    [[nodiscard]] bool Contains(EntityID entity) const noexcept override {
      const std::uint32_t entityIndex = ECS::GetIndex(entity);
      const std::size_t pageIndex     = entityIndex / PageSize;
      const std::size_t offset        = entityIndex % PageSize;
      
      if (pageIndex >= m_SparsePages.size() || m_SparsePages[pageIndex] == nullptr) {
        return false;
      }
      return m_SparsePages[pageIndex][offset] != InvalidIndex;
    }
    
    void Remove(EntityID entity) override {
      if (!Contains(entity)) {
        return;
      }
      
      const std::uint32_t entityIndex  = ECS::GetIndex(entity);
      const std::size_t pageIndex      = entityIndex / PageSize;
      const std::size_t offset         = entityIndex % PageSize;
      const std::uint32_t removedIndex = m_SparsePages[pageIndex][offset];
      
      const std::uint32_t lastIndex    = static_cast<std::uint32_t>(m_DenseEntities.size() - 1);
      const EntityID lastEntity        = m_DenseEntities.back();
      const std::uint32_t lastEntityIdx = ECS::GetIndex(lastEntity);
      
      // Swap-and-Pop: move the tail element into the removed slot to preserve 100% dense contiguity
      if (removedIndex != lastIndex) {
        m_DenseEntities[removedIndex]   = std::move(m_DenseEntities.back());
        m_DenseComponents[removedIndex] = std::move(m_DenseComponents.back());
        
        const std::size_t lastPage   = lastEntityIdx / PageSize;
        const std::size_t lastOffset = lastEntityIdx % PageSize;
        m_SparsePages[lastPage][lastOffset] = removedIndex;
      }
      
      m_DenseEntities.pop_back();
      m_DenseComponents.pop_back();
      m_SparsePages[pageIndex][offset] = InvalidIndex;
      
      UpdateMemoryTracking();
    }
    
    void Clear() noexcept override {
      for (std::uint32_t* page : m_SparsePages) {
        delete[] page;
      }
      m_SparsePages.clear();
      m_DenseEntities.clear();
      m_DenseComponents.clear();
      m_MemoryGuard.Reset(0);
    }
    
    [[nodiscard]] std::size_t Size() const noexcept override {
      return m_DenseEntities.size();
    }
    
    [[nodiscard]] const std::vector<EntityID>& GetEntities() const noexcept override {
      return m_DenseEntities;
    }
    
    [[nodiscard]] const std::vector<Component>& GetComponents() const noexcept {
      return m_DenseComponents;
    }
    
    [[nodiscard]] std::vector<Component>& GetComponents() noexcept {
      return m_DenseComponents;
    }
    
  private:
    void AssurePage(std::uint32_t entityIndex) {
      const std::size_t pageIndex = entityIndex / PageSize;
      if (pageIndex >= m_SparsePages.size()) {
        m_SparsePages.resize(pageIndex + 1, nullptr);
      }
      if (m_SparsePages[pageIndex] == nullptr) {
        m_SparsePages[pageIndex] = new std::uint32_t[PageSize];
        for (std::size_t i = 0; i < PageSize; ++i) {
          m_SparsePages[pageIndex][i] = InvalidIndex;
        }
      }
    }
    
    void UpdateMemoryTracking() noexcept {
      std::size_t totalBytes = 0;
      for (const std::uint32_t* page : m_SparsePages) {
        if (page != nullptr) {
          totalBytes += PageSize * sizeof(std::uint32_t);
        }
      }
      totalBytes += m_DenseEntities.capacity() * sizeof(EntityID);
      totalBytes += m_DenseComponents.capacity() * sizeof(Component);
      m_MemoryGuard.Reset(totalBytes);
    }
    
    std::vector<std::uint32_t*> m_SparsePages;
    std::vector<EntityID>       m_DenseEntities;
    std::vector<Component>      m_DenseComponents;
    ScopedMemoryAllocation      m_MemoryGuard{};
  };
  
} // namespace KanVerse
