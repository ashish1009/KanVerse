/*!
 * @file Scene.mm
 * @brief Implementation of Scene lifecycle, destruction mechanics, dynamic floor-aware debris physics, and PBR batch rendering.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#include "Scene/Scene.hpp"
#include "Core/Diagnostics/Log.hpp"
#include "Core/Diagnostics/Assert.hpp"
#include "Renderer/Renderer/Renderer.hpp"
#include "Audio/AudioSystem.hpp"
#include <map>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cstdlib>

namespace KanVerse {
  
  Scene::Scene(std::string_view debugName)
  : m_DebugName(debugName),
  m_SunLight(Math::MakeVec3(-0.6f, -1.0f, -0.4f), Math::MakeVec3(1.0f, 0.95f, 0.88f), 1.2f),
  m_Environment{
    .AmbientColor = Math::MakeVec3(0.18f, 0.20f, 0.24f),
    .AmbientIntensity = 1.0f
  } {
    KV_CORE_INFO("Scene instantiated: '{}'", m_DebugName);
  }
  
  Scene::~Scene() {
    m_Registry.Clear();
    KV_CORE_INFO("Scene destroyed and registry cleared: '{}'", m_DebugName);
  }
  
  std::shared_ptr<Scene> Scene::Create(std::string_view debugName) {
    return std::make_shared<Scene>(debugName);
  }
  
  Entity Scene::CreateEntity(std::string_view name) {
    EntityID id = m_Registry.Create();
    Entity entity(id, this);
    entity.AddComponent<TagComponent>(name);
    entity.AddComponent<TransformComponent>();
    return entity;
  }
  
  void Scene::DestroyEntity(Entity entity) {
    KV_CORE_ASSERT(entity.IsValid(), "[KanVerse Scene] Attempted to destroy an invalid entity!");
    m_Registry.Destroy(entity.GetID());
  }
  
  void Scene::ShatterBlock(Entity blockEntity, const simd::float3& impactOrigin) {
    if (!blockEntity || !blockEntity.IsValid()) return;
    
    AudioSystem::Play(SoundEffect::Shatter);
    
    const auto& parentTransform = blockEntity.GetComponent<TransformComponent>();
    auto material = blockEntity.GetComponent<MaterialComponent>().SurfaceMaterial;
    
    bool persist = false;
    if (blockEntity.HasComponent<FracturableComponent>()) {
      persist = blockEntity.GetComponent<FracturableComponent>().PersistDebris;
    }
    
    const simd::float3 blockCenter = parentTransform.Translation;
    
    constexpr float subOffset = 0.2f;
    for (float x : {-subOffset, subOffset}) {
      for (float y : {-subOffset, subOffset}) {
        for (float z : {-subOffset, subOffset}) {
          Entity shard = CreateEntity("DebrisShard");
          auto& transform = shard.GetComponent<TransformComponent>();
          transform.Translation = blockCenter + simd_make_float3(x, y, z);
          
          if (m_ShardMesh) {
            shard.AddComponent<MeshComponent>(m_ShardMesh);
          }
          if (material) {
            shard.AddComponent<MaterialComponent>(material);
          }
          
          simd::float3 blastDir = transform.Translation - impactOrigin;
          float dist = simd_length(blastDir);
          if (dist > 1e-4f) {
            blastDir /= dist;
          } else {
            blastDir = simd_make_float3(0.0f, 1.0f, 0.0f);
          }
          
          blastDir.y += 0.55f;
          blastDir = simd_normalize(blastDir);
          
          const float impulseSpeed = 10.0f + (static_cast<float>(rand() % 100) / 100.0f) * 8.0f;
          
          DebrisComponent debris;
          debris.LinearVelocity  = blastDir * impulseSpeed;
          debris.AngularVelocity = simd_make_float3(
                                                    (static_cast<float>(rand() % 200 - 100) / 100.0f) * 16.0f,
                                                    (static_cast<float>(rand() % 200 - 100) / 100.0f) * 16.0f,
                                                    (static_cast<float>(rand() % 200 - 100) / 100.0f) * 16.0f
                                                    );
          debris.Lifetime   = persist ? 999999.0f : 5.0f;
          debris.Bounciness = 0.35f;
          debris.Friction   = 0.85f;
          
          shard.AddComponent<DebrisComponent>(debris);
        }
      }
    }
    
    DestroyEntity(blockEntity);
  }
  
  void Scene::UpdateDebris(TimeStep ts) {
    const float dt = ts.GetSeconds();
    constexpr float shardRadius = 0.09f; // Half-extent of shard mesh
    
    std::vector<EntityID> deadEntities;
    auto debrisView = m_Registry.GetView<TransformComponent, DebrisComponent>();
    auto colliderView = m_Registry.GetView<TransformComponent, ColliderComponent>();
    
    debrisView.Each([&](EntityID entity, TransformComponent& transform, DebrisComponent& debris) {
      debris.Lifetime -= dt;
      if (debris.Lifetime <= 0.0f) {
        deadEntities.push_back(entity);
        return;
      }
      
      debris.LinearVelocity.y += -28.0f * dt;
      simd::float3 nextPos = transform.Translation + debris.LinearVelocity * dt;
      
      // Dynamic collision check against all scene colliders (floors, platforms, ground)
      bool grounded = false;
      float highestSurfaceY = -7.64f + 0.18f; // Fallback ground plane
      
      colliderView.Each([&](EntityID colEntity, TransformComponent& tc, ColliderComponent& col) {
        if (colEntity == entity) return;
        
        const simd::float3 halfExtents = col.BoxCollider.HalfExtents * tc.Scale;
        const simd::float3 boxMin = tc.Translation - halfExtents;
        const simd::float3 boxMax = tc.Translation + halfExtents;
        
        if (nextPos.x + shardRadius > boxMin.x && nextPos.x - shardRadius < boxMax.x &&
            nextPos.z + shardRadius > boxMin.z && nextPos.z - shardRadius < boxMax.z) {
          const float boxTopY = boxMax.y + shardRadius;
          const float shardFeetY = transform.Translation.y - shardRadius;
          
          if (debris.LinearVelocity.y <= 0.1f && boxMax.y > shardFeetY - 0.5f && boxMax.y <= shardFeetY + 0.6f) {
            if (boxTopY > highestSurfaceY) {
              highestSurfaceY = boxTopY;
              grounded = true;
            }
          }
        }
      });
      
      if (grounded || nextPos.y <= highestSurfaceY) {
        nextPos.y = grounded ? highestSurfaceY : highestSurfaceY;
        transform.Translation = nextPos;
        
        debris.LinearVelocity.y = -debris.LinearVelocity.y * debris.Bounciness;
        debris.LinearVelocity.x *= debris.Friction;
        debris.LinearVelocity.z *= debris.Friction;
        debris.AngularVelocity  *= 0.7f;
        
        if (std::abs(debris.LinearVelocity.y) < 0.25f) {
          debris.LinearVelocity.y = 0.0f;
          debris.LinearVelocity.x = 0.0f;
          debris.LinearVelocity.z = 0.0f;
          debris.AngularVelocity  = simd_make_float3(0.0f, 0.0f, 0.0f);
        }
      } else {
        transform.Translation = nextPos;
      }
      
      transform.Rotation += debris.AngularVelocity * dt;
    });
    
    for (EntityID id : deadEntities) {
      m_Registry.Destroy(id);
    }
  }
  
  void Scene::OnUpdate(TimeStep ts) {
    UpdateDebris(ts);
    
    auto controllerView = m_Registry.GetView<TransformComponent, CharacterControllerComponent>();
    controllerView.Each([&](EntityID, TransformComponent&, CharacterControllerComponent& controller) {
      if (controller.TriggerSlamImpact) {
        AudioSystem::Play(SoundEffect::GroundSlam);
        
        constexpr float blastRadius = 3.6f;
        std::vector<Entity> blocksToShatter;
        
        auto fracturableView = m_Registry.GetView<TransformComponent, FracturableComponent>();
        fracturableView.Each([&](EntityID ent, TransformComponent& transform, FracturableComponent&) {
          const float dist = simd_distance(transform.Translation, controller.ImpactPosition);
          if (dist <= blastRadius) {
            blocksToShatter.push_back(Entity(ent, this));
          }
        });
        
        for (auto& block : blocksToShatter) {
          ShatterBlock(block, controller.ImpactPosition);
        }
      }
    });
  }
  
  void Scene::OnRenderShadowPass(const std::shared_ptr<Pipeline>& shadowPipeline,
                                 const std::shared_ptr<Framebuffer>& shadowFramebuffer) {
    KV_CORE_ASSERT(shadowPipeline != nullptr, "[KanVerse Scene] Shadow pipeline cannot be null!");
    KV_CORE_ASSERT(shadowFramebuffer != nullptr, "[KanVerse Scene] Shadow framebuffer cannot be null!");
    
    DirectionalLight sunLight = m_SunLight;
    auto dirLightView = m_Registry.GetView<TransformComponent, DirectionalLightComponent>();
    dirLightView.Each([&](EntityID entity, TransformComponent& transform, DirectionalLightComponent& light) {
      (void)entity;
      (void)transform;
      if (light.Primary) {
        sunLight = light.LightInstance;
      }
    });
    
    Renderer::BeginShadowPass(sunLight, shadowFramebuffer);
    
    std::map<Mesh*, std::vector<InstanceData>> shadowBatches;
    auto renderView = m_Registry.GetView<TransformComponent, MeshComponent>();
    renderView.Each([&](EntityID entity, TransformComponent& transform, MeshComponent& mesh) {
      if (!mesh.MeshGeometry) return;
      
      if (m_Registry.Has<PointLightComponent>(entity)) {
        return;
      }
      if (m_Registry.Has<MaterialComponent>(entity)) {
        const auto& mat = m_Registry.Get<MaterialComponent>(entity).SurfaceMaterial;
        if (mat) {
          const Math::Vec4& emissive = mat->GetEmissiveColor();
          const float emissiveMax = std::max({emissive.x, emissive.y, emissive.z});
          if (mat->IsTransparent() || emissiveMax > 0.5f) {
            return;
          }
        }
      }
      
      const Math::Mat4 modelMatrix = transform.GetTransform();
      shadowBatches[mesh.MeshGeometry.get()].push_back(InstanceData{
        .ModelMatrix  = modelMatrix,
        .NormalMatrix = Math::Transpose(Math::Inverse(modelMatrix))
      });
    });
    
    for (auto& [meshPtr, instances] : shadowBatches) {
      if (!instances.empty()) {
        Renderer::SubmitInstanced(
                                  shadowPipeline,
                                  nullptr,
                                  meshPtr->GetSubmeshes().empty() ? nullptr : std::shared_ptr<Mesh>(meshPtr, [](Mesh*){}),
                                  instances
                                  );
      }
    }
    
    Renderer::EndScene();
  }
  
  void Scene::OnRenderRuntime(const std::shared_ptr<Pipeline>& defaultPipeline,
                              const std::shared_ptr<Framebuffer>& target) {
    KV_CORE_ASSERT(defaultPipeline != nullptr, "[KanVerse Scene] Default pipeline cannot be null during render!");
    
    const PerspectiveCamera* mainCamera = nullptr;
    auto cameraView = m_Registry.GetView<TransformComponent, CameraComponent>();
    cameraView.Each([&](EntityID entity, TransformComponent& transform, CameraComponent& camera) {
      (void)entity;
      (void)transform;
      if (camera.Primary) {
        mainCamera = &camera.CameraInstance;
      }
    });
    
    if (!mainCamera) {
      KV_CORE_WARN("Scene::OnRenderRuntime: No primary camera active in registry. Skipping draw pass.");
      return;
    }
    
    DirectionalLight sunLight = m_SunLight;
    auto dirLightView = m_Registry.GetView<TransformComponent, DirectionalLightComponent>();
    dirLightView.Each([&](EntityID entity, TransformComponent& transform, DirectionalLightComponent& light) {
      (void)entity;
      (void)transform;
      if (light.Primary) {
        sunLight = light.LightInstance;
      }
    });
    
    EnvironmentLighting envLighting = m_Environment;
    auto envLightView = m_Registry.GetView<EnvironmentLightComponent>();
    envLightView.Each([&](EntityID entity, EnvironmentLightComponent& env) {
      (void)entity;
      envLighting = env.Environment;
    });
    
    std::vector<GPUPointLight> gpuPointLights;
    gpuPointLights.reserve(Renderer::kMaxPointLights);
    
    auto pointLightView = m_Registry.GetView<TransformComponent, PointLightComponent>();
    pointLightView.Each([&](EntityID entity, TransformComponent& transform, PointLightComponent& plComp) {
      (void)entity;
      if (gpuPointLights.size() >= Renderer::kMaxPointLights) return;
      const auto& pl = plComp.LightInstance;
      const Math::Vec3& color = pl.GetColor();
      
      const float constant  = (pl.GetConstant()  > 0.01f)  ? pl.GetConstant()  : 1.0f;
      const float linear    = (pl.GetLinear()    > 0.001f) ? pl.GetLinear()    : 0.35f;
      const float quadratic = (pl.GetQuadratic() > 0.001f) ? pl.GetQuadratic() : 0.08f;
      const float radius    = std::clamp(pl.GetRadius(), 6.0f, 16.0f);
      
      gpuPointLights.push_back(GPUPointLight{
        .PositionAndRadius = Math::MakeVec4(transform.Translation.x, transform.Translation.y, transform.Translation.z, radius),
        .ColorAndIntensity = Math::MakeVec4(color.x, color.y, color.z, pl.GetIntensity()),
        .Attenuation       = Math::MakeVec4(constant, linear, quadratic, 0.0f)
      });
    });
    
    using BatchKey = std::pair<Mesh*, Material*>;
    struct BatchBucket {
      std::shared_ptr<Mesh>     MeshRef;
      std::shared_ptr<Material> MaterialRef;
      std::vector<InstanceData> Instances;
    };
    std::map<BatchKey, BatchBucket> opaqueBatches;
    std::map<BatchKey, BatchBucket> transparentBatches;
    
    auto renderView = m_Registry.GetView<TransformComponent, MeshComponent, MaterialComponent>();
    renderView.Each([&](EntityID entity,
                        TransformComponent& transform,
                        MeshComponent& mesh,
                        MaterialComponent& material) {
      (void)entity;
      if (mesh.MeshGeometry && material.SurfaceMaterial) {
        BatchKey key = { mesh.MeshGeometry.get(), material.SurfaceMaterial.get() };
        auto& targetMap = material.SurfaceMaterial->IsTransparent() ? transparentBatches : opaqueBatches;
        auto& bucket = targetMap[key];
        bucket.MeshRef     = mesh.MeshGeometry;
        bucket.MaterialRef = material.SurfaceMaterial;
        const Math::Mat4 modelMatrix = transform.GetTransform();
        bucket.Instances.push_back(InstanceData{
          .ModelMatrix  = modelMatrix,
          .NormalMatrix = Math::Transpose(Math::Inverse(modelMatrix))
        });
      }
    });
    
    Renderer::BeginScene(*mainCamera, sunLight, envLighting, target, gpuPointLights);
    
    for (auto& [key, bucket] : opaqueBatches) {
      if (!bucket.Instances.empty()) {
        Renderer::SubmitInstanced(
                                  defaultPipeline,
                                  bucket.MaterialRef,
                                  bucket.MeshRef,
                                  bucket.Instances
                                  );
      }
    }
    
    auto transparentPipeline = Renderer::GetTransparentPipeline();
    if (!transparentPipeline) {
      transparentPipeline = defaultPipeline;
    }
    for (auto& [key, bucket] : transparentBatches) {
      if (!bucket.Instances.empty()) {
        Renderer::SubmitInstanced(
                                  transparentPipeline,
                                  bucket.MaterialRef,
                                  bucket.MeshRef,
                                  bucket.Instances
                                  );
      }
    }
    
    Renderer::EndScene();
  }
  
  void Scene::OnViewportResize(uint32_t width, uint32_t height) {
    if (width == 0 || height == 0) return;
    m_ViewportWidth  = width;
    m_ViewportHeight = height;
    auto cameraView = m_Registry.GetView<CameraComponent>();
    cameraView.Each([&](EntityID entity, CameraComponent& camera) {
      (void)entity;
      camera.CameraInstance.SetViewportSize(width, height);
    });
  }
  
} // namespace KanVerse
