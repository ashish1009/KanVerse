/*!
 * @file Components.hpp
 * @brief Data-oriented ECS components for spatial transforms, rendering, lighting, physics, and AI.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#pragma once

#include "Core/Math/Math.hpp"
#include "Renderer/Geometry/Mesh.hpp"
#include "Renderer/Materials/Material.hpp"
#include "Renderer/Camera/PerspectiveCamera.hpp"
#include "Renderer/Lighting/DirectionalLight.hpp"
#include "Renderer/Lighting/PointLight.hpp"
#include "Renderer/Lighting/EnvironmentLighting.hpp"
#include "Physics/BoundingBox.hpp"
#include "Physics/ColliderPrimitives.hpp"
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>

namespace KanVerse {
  
  /*!
   * @struct TagComponent
   * @brief Stores a human-readable string identifier for an entity.
   */
  struct TagComponent {
    std::string Tag{ "Entity" };
    
    TagComponent() = default;
    TagComponent(const TagComponent&) = default;
    explicit TagComponent(std::string_view tag) : Tag(tag) {}
  };
  
  /*!
   * @struct TransformComponent
   * @brief Represents 3D world translation, Euler rotation (radians), and local scale.
   */
  struct TransformComponent {
    Math::Vec3 Translation{ 0.0f, 0.0f, 0.0f };
    Math::Vec3 Rotation{ 0.0f, 0.0f, 0.0f };
    Math::Vec3 Scale{ 1.0f, 1.0f, 1.0f };
    
    TransformComponent() = default;
    TransformComponent(const TransformComponent&) = default;
    explicit TransformComponent(const Math::Vec3& translation) noexcept
    : Translation(translation) {}
    
    /*!
     * @brief Computes the $4 \times 4$ TRS model matrix ($T \cdot R_z \cdot R_y \cdot R_x \cdot S$).
     * @return Column-major $4 \times 4$ transformation matrix.
     */
    [[nodiscard]] Math::Mat4 GetTransform() const noexcept {
      Math::Mat4 translationMatrix = Math::Identity();
      translationMatrix.columns[3] = Math::MakeVec4(Translation.x, Translation.y, Translation.z, 1.0f);
      
      const Math::Mat4 rotX = Math::Rotate(Rotation.x, Math::MakeVec3(1.0f, 0.0f, 0.0f));
      const Math::Mat4 rotY = Math::Rotate(Rotation.y, Math::MakeVec3(0.0f, 1.0f, 0.0f));
      const Math::Mat4 rotZ = Math::Rotate(Rotation.z, Math::MakeVec3(0.0f, 0.0f, 1.0f));
      const Math::Mat4 rotationMatrix = Math::Multiply(rotZ, Math::Multiply(rotY, rotX));
      
      Math::Mat4 scaleMatrix = Math::Identity();
      scaleMatrix.columns[0].x = Scale.x;
      scaleMatrix.columns[1].y = Scale.y;
      scaleMatrix.columns[2].z = Scale.z;
      
      return Math::Multiply(translationMatrix, Math::Multiply(rotationMatrix, scaleMatrix));
    }
    
    [[nodiscard]] operator Math::Mat4() const noexcept {
      return GetTransform();
    }
  };
  
  /*!
   * @struct MeshComponent
   * @brief Holds a shared reference to a GPU-backed 3D `Mesh`.
   */
  struct MeshComponent {
    std::shared_ptr<Mesh> MeshGeometry{ nullptr };
    
    MeshComponent() = default;
    MeshComponent(const MeshComponent&) = default;
    explicit MeshComponent(std::shared_ptr<Mesh> mesh) noexcept
    : MeshGeometry(std::move(mesh)) {}
  };
  
  /*!
   * @struct MaterialComponent
   * @brief Holds a shared reference to a PBR surface `Material`.
   */
  struct MaterialComponent {
    std::shared_ptr<Material> SurfaceMaterial{ nullptr };
    
    MaterialComponent() = default;
    MaterialComponent(const MaterialComponent&) = default;
    explicit MaterialComponent(std::shared_ptr<Material> material) noexcept
    : SurfaceMaterial(std::move(material)) {}
  };
  
  /*!
   * @struct CameraComponent
   * @brief Attaches an optical `PerspectiveCamera` lens to an entity.
   */
  struct CameraComponent {
    PerspectiveCamera CameraInstance{ 60.0f * (3.14159265f / 180.0f), 1280.0f / 720.0f, 0.1f, 1000.0f };
    bool              Primary{ true };
    
    CameraComponent() = default;
    CameraComponent(const CameraComponent&) = default;
    explicit CameraComponent(const PerspectiveCamera& camera, bool isPrimary = true) noexcept
    : CameraInstance(camera), Primary(isPrimary) {}
  };
  
  /*!
   * @struct DirectionalLightComponent
   * @brief Represents the primary directional sun/moon light and its shadow frustum.
   */
  struct DirectionalLightComponent {
    DirectionalLight LightInstance{ Math::MakeVec3(-0.6f, -1.0f, -0.4f), Math::MakeVec3(1.0f, 0.95f, 0.88f), 1.2f };
    bool             Primary{ true };
    
    DirectionalLightComponent() = default;
    DirectionalLightComponent(const DirectionalLightComponent&) = default;
    explicit DirectionalLightComponent(const DirectionalLight& light, bool isPrimary = true) noexcept
    : LightInstance(light), Primary(isPrimary) {}
  };
  
  /*!
   * @struct PointLightComponent
   * @brief Wraps a local omni-directional `PointLight` emitter.
   */
  struct PointLightComponent {
    PointLight LightInstance{};
    
    PointLightComponent() = default;
    PointLightComponent(const PointLightComponent&) = default;
    explicit PointLightComponent(const PointLight& light) noexcept
    : LightInstance(light) {}
    PointLightComponent(const Math::Vec3& color, float intensity, float radius = 15.0f) noexcept
    : LightInstance(color, intensity, radius) {}
  };
  
  /*!
   * @struct EnvironmentLightComponent
   * @brief Stores global ambient environment illumination parameters.
   */
  struct EnvironmentLightComponent {
    EnvironmentLighting Environment{
      .AmbientColor     = Math::MakeVec3(0.18f, 0.20f, 0.24f),
      .AmbientIntensity = 1.0f
    };
    
    EnvironmentLightComponent() = default;
    EnvironmentLightComponent(const EnvironmentLightComponent&) = default;
    explicit EnvironmentLightComponent(const EnvironmentLighting& env) noexcept
    : Environment(env) {}
  };
  
  /*!
   * @struct ColliderComponent
   * @brief Defines physical collision bounds (`Box`, `Capsule`, or `Cylinder`) for an entity.
   */
  struct ColliderComponent {
    enum class ShapeType : std::uint8_t { Box = 0, Capsule, Cylinder };
    
    ShapeType                 Type{ ShapeType::Box };
    Physics::OBB              BoxCollider{};
    Physics::CapsuleCollider  Capsule{};
    Physics::CylinderCollider Cylinder{};
    bool                      IsTrigger{ false };
    
    ColliderComponent() = default;
    
    [[nodiscard]] static ColliderComponent MakeBox(simd::float3 halfExtents) noexcept {
      ColliderComponent comp;
      comp.Type                    = ShapeType::Box;
      comp.BoxCollider.HalfExtents = halfExtents;
      return comp;
    }
    
    [[nodiscard]] static ColliderComponent MakeCapsule(float radius, float height) noexcept {
      ColliderComponent comp;
      comp.Type              = ShapeType::Capsule;
      comp.Capsule.Radius    = radius;
      comp.Capsule.BasePoint = simd_make_float3(0.0f, radius, 0.0f);
      comp.Capsule.TipPoint  = simd_make_float3(0.0f, radius + height, 0.0f);
      return comp;
    }
  };
  
  enum class PlayerState : std::uint8_t {
    Grounded = 0,
    Airborne,
    Sliding,
    GroundSlamming
  };
  
  /*!
   * @struct CharacterControllerComponent
   * @brief Kinematic character locomotion, jump/slam state, and procedural squash-and-stretch spring parameters.
   */
  struct CharacterControllerComponent {
    PlayerState   State{ PlayerState::Airborne };
    simd::float3  Velocity{ 0.0f, 0.0f, 0.0f };
    float         MoveSpeed{ 9.5f };
    float         JumpImpulse{ 14.0f };
    float         Gravity{ -28.0f };
    float         SlamVelocity{ -32.0f };
    std::uint32_t JumpsRemaining{ 2 };
    bool          IsGrounded{ false };
    bool          IsSliding{ false };
    float         SlideTimer{ 0.0f };
    bool          IsSlamming{ false };
    float         FacingYaw{ 0.0f };
    bool          TriggerSlamImpact{ false };
    simd::float3  ImpactPosition{ 0.0f, 0.0f, 0.0f };
    float         RollAngle{ 0.0f };
    float         PitchAngle{ 0.0f };
    simd::float3  VisualScale{ 1.0f, 1.0f, 1.0f };
    float         SquashVelocity{ 0.0f };
  };
  
  /*!
   * @struct DebrisComponent
   * @brief Rigid shard kinematics and restitution parameters for shattered blocks.
   */
  struct DebrisComponent {
    simd::float3 LinearVelocity{ 0.0f, 0.0f, 0.0f };
    simd::float3 AngularVelocity{ 0.0f, 0.0f, 0.0f };
    float        Lifetime{ 4.0f };
    float        Bounciness{ 0.35f };
    float        Friction{ 0.82f };
  };
  
  /*!
   * @struct FracturableComponent
   * @brief Marks an entity as breakable when struck by a ground-slam shockwave.
   */
  struct FracturableComponent {
    float BlastResistance{ 1.0f };
    bool  PersistDebris{ false };
  };
  
  enum class DoorType : std::uint8_t {
    Angular = 0,
    Sliding
  };
  
  /*!
   * @struct DoorComponent
   * @brief State and hinge/slide parameters for proximity-activated doors.
   */
  struct DoorComponent {
    DoorType    Type{ DoorType::Angular };
    bool        IsXAligned{ true };
    float       CurrentAngle{ 0.0f };
    float       TargetAngle{ 2.18166156f };
    float       CurrentSlide{ 0.0f };
    float       TargetSlide{ 0.0f };
    float       OpenSpeed{ 8.0f };
    bool        IsOpen{ false };
    float       InteractionRadius{ 2.2f };
    Math::Vec3  HingePosition{ 0.0f, 0.0f, 0.0f };
    float       Width{ 1.5f };
    bool        IsLocked{ false };
    std::string RequiredKeyID{ "" };
  };
  
  enum class EnemyAIState : std::uint8_t {
    Sleep = 0,
    Idle,
    Patrol,
    Walk,
    Sprint,
    Jump,
    Fly,
    Attack
  };
  
  /*!
   * @struct AIControllerComponent
   * @brief Autonomous enemy state machine, agro sensing, and locomotion velocity.
   */
  struct AIControllerComponent {
    EnemyAIState State{ EnemyAIState::Sleep };
    simd::float3 Velocity{ 0.0f, 0.0f, 0.0f };
    simd::float3 HomePosition{ 0.0f, 0.0f, 0.0f };
    float        AgroRange{ 14.0f };
    float        AttackRange{ 4.5f };
    float        MoveSpeed{ 5.0f };
    float        StateTimer{ 0.0f };
    bool         IsGrounded{ false };
  };
  
} // namespace KanVerse
