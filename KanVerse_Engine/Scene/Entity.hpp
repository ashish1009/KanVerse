/*!
 * @file Entity.hpp
 * @brief Ergonomic high-level wrapper around a 64-bit EntityID and parent Scene.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#pragma once

#include "Scene/EntityID.hpp"
#include "Scene/Registry.hpp"
#include "Core/Diagnostics/Assert.hpp"

namespace KanVerse {
  
  class Scene;
  
  /*!
   * @class Entity
   * @brief Lightweight handle binding a 64-bit EntityID to its parent Scene registry.
   */
  class Entity {
  public:
    Entity() = default;
    
    /*!
     * @brief Constructs an entity handle bound to a scene.
     * @param handle 64-bit entity identifier.
     * @param scene Pointer to the owning scene instance.
     */
    Entity(EntityID handle, Scene* scene)
    : m_EntityHandle(handle), m_Scene(scene) {}
    
    Entity(const Entity&) = default;
    Entity& operator=(const Entity&) = default;
    Entity(Entity&&) noexcept = default;
    Entity& operator=(Entity&&) noexcept = default;
    
    /*!
     * @brief Constructs and attaches a component in-place on this entity.
     * @tparam T Component type.
     * @tparam Args Forwarded constructor argument types.
     * @param args Arguments forwarded to component constructor.
     * @return Reference to the newly constructed component.
     */
    template <typename T, typename... Args>
    T& AddComponent(Args&&... args);
    
    /*!
     * @brief Retrieves mutable reference to the entity's component.
     * @tparam T Component type.
     * @return Reference to component.
     */
    template <typename T>
    [[nodiscard]] T& GetComponent();
    
    /*!
     * @brief Retrieves const reference to the entity's component.
     * @tparam T Component type.
     * @return Const reference to component.
     */
    template <typename T>
    [[nodiscard]] const T& GetComponent() const;
    
    /*!
     * @brief Queries whether the entity currently holds the specified component.
     * @tparam T Component type.
     * @return True if component exists on entity.
     */
    template <typename T>
    [[nodiscard]] bool HasComponent() const;
    
    /*!
     * @brief Removes a component from this entity.
     * @tparam T Component type.
     */
    template <typename T>
    void RemoveComponent();
    
    /*!
     * @brief Queries whether this handle refers to a live entity in the parent scene.
     * @return True if entity exists and is valid.
     */
    [[nodiscard]] bool IsValid() const;
    
    /*!
     * @brief Accesses the raw 64-bit entity handle.
     */
    [[nodiscard]] EntityID GetID() const noexcept { return m_EntityHandle; }
    
    /*!
     * @brief Implicit conversion operator to EntityID.
     */
    [[nodiscard]] operator EntityID() const noexcept { return m_EntityHandle; }
    
    /*!
     * @brief Explicit boolean conversion checking entity validity.
     */
    [[nodiscard]] explicit operator bool() const noexcept { return IsValid(); }
    
    [[nodiscard]] bool operator==(const Entity& other) const noexcept {
      return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene;
    }
    
    [[nodiscard]] bool operator!=(const Entity& other) const noexcept {
      return !(*this == other);
    }
    
  private:
    EntityID m_EntityHandle{ECS::NullEntity};
    Scene*   m_Scene{nullptr};
  };
  
} // namespace KanVerse
