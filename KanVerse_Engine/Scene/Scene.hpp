/*!
 * @file Scene.hpp
 * @brief Master scene graph and ECS coordinator for simulation, shadows, rendering, and destruction mechanics.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#pragma once

#include "Scene/Registry.hpp"
#include "Scene/Entity.hpp"
#include "Scene/Components.hpp"
#include "Core/Time/TimeStep.hpp"
#include "Renderer/Framebuffer/Framebuffer.hpp"
#include "Renderer/Lighting/DirectionalLight.hpp"
#include "Renderer/Lighting/EnvironmentLighting.hpp"
#include "Renderer/Pipeline/Pipeline.hpp"

#include <string>
#include <string_view>
#include <memory>

namespace KanVerse {
  
  /*!
   * @class Scene
   * @brief Top-level container managing entity lifecycles, simulation updates, destruction, and render passes.
   */
  class Scene {
  public:
    explicit Scene(std::string_view debugName = "Scene");
    virtual ~Scene();
    
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;
    Scene(Scene&&) noexcept = default;
    Scene& operator=(Scene&&) noexcept = default;
    
    [[nodiscard]] static std::shared_ptr<Scene> Create(std::string_view debugName = "Scene");
    
    Entity CreateEntity(std::string_view name = "Entity");
    void DestroyEntity(Entity entity);
    
    /*!
     * @brief Updates scene simulation, debris physics, and destruction triggers.
     * @param ts Delta time step.
     */
    void OnUpdate(TimeStep ts);
    
    /*!
     * @brief Fractures a breakable block or floor tile into tumbling debris shards upon ground slam.
     * @param blockEntity Target entity to destroy.
     * @param impactOrigin Shockwave origin point in world coordinates.
     */
    void ShatterBlock(Entity blockEntity, const simd::float3& impactOrigin);
    
    /*!
     * @brief Sets the shared shard mesh used when spawning debris particles.
     * @param shardMesh Mesh reference.
     */
    void SetShardMesh(std::shared_ptr<Mesh> shardMesh) { m_ShardMesh = std::move(shardMesh); }
    
    /*!
     * @brief Renders all instanced geometry into an offscreen depth-only shadow framebuffer.
     * @param shadowPipeline Depth-only pipeline state utilizing ShadowDepth.metal.
     * @param shadowFramebuffer Target depth-only framebuffer.
     */
    virtual void OnRenderShadowPass(const std::shared_ptr<Pipeline>& shadowPipeline,
                                    const std::shared_ptr<Framebuffer>& shadowFramebuffer);
    
    /*!
     * @brief Finds the primary CameraComponent and renders all entities possessing MeshComponent and MaterialComponent.
     * @param defaultPipeline Main lighting pipeline state.
     * @param target Optional custom offscreen framebuffer target (defaults to swapchain if null).
     */
    virtual void OnRenderRuntime(const std::shared_ptr<Pipeline>& defaultPipeline,
                                 const std::shared_ptr<Framebuffer>& target = nullptr);
    
    void OnViewportResize(uint32_t width, uint32_t height);
    
    void SetDirectionalLight(const DirectionalLight& light) noexcept { m_SunLight = light; }
    void SetEnvironmentLighting(const EnvironmentLighting& env) noexcept { m_Environment = env; }
    
    [[nodiscard]] Registry& GetRegistry() noexcept { return m_Registry; }
    [[nodiscard]] const Registry& GetRegistry() const noexcept { return m_Registry; }
    [[nodiscard]] const std::string& GetName() const noexcept { return m_DebugName; }
    
  private:
    void UpdateDebris(TimeStep ts);
    
    std::string         m_DebugName;
    Registry            m_Registry;
    DirectionalLight    m_SunLight;
    EnvironmentLighting m_Environment;
    std::shared_ptr<Mesh> m_ShardMesh{nullptr}; ///< Shared mesh for debris shards.
    uint32_t            m_ViewportWidth{1280};
    uint32_t            m_ViewportHeight{720};
    
    friend class Entity;
  };
  
  template <typename T, typename... Args>
  inline T& Entity::AddComponent(Args&&... args) {
    KV_CORE_ASSERT(IsValid(), "[KanVerse Entity] Cannot add component to invalid entity!");
    return m_Scene->m_Registry.Emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
  }
  
  template <typename T>
  inline T& Entity::GetComponent() {
    KV_CORE_ASSERT(IsValid(), "[KanVerse Entity] Cannot get component from invalid entity!");
    return m_Scene->m_Registry.Get<T>(m_EntityHandle);
  }
  
  template <typename T>
  inline const T& Entity::GetComponent() const {
    KV_CORE_ASSERT(IsValid(), "[KanVerse Entity] Cannot get component from invalid entity!");
    return m_Scene->m_Registry.Get<T>(m_EntityHandle);
  }
  
  template <typename T>
  inline bool Entity::HasComponent() const {
    if (!IsValid()) return false;
    return m_Scene->m_Registry.Has<T>(m_EntityHandle);
  }
  
  template <typename T>
  inline void Entity::RemoveComponent() {
    KV_CORE_ASSERT(IsValid(), "[KanVerse Entity] Cannot remove component from invalid entity!");
    m_Scene->m_Registry.Remove<T>(m_EntityHandle);
  }
  
  inline bool Entity::IsValid() const {
    return m_Scene != nullptr && m_Scene->m_Registry.IsValid(m_EntityHandle);
  }
  
} // namespace KanVerse
