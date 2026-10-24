// FILE: KanVerse_Engine/Physics/ColliderPrimitives.hpp
/*!
 * @file ColliderPrimitives.hpp
 * @brief Mathematical collision volumes (Capsule, Cylinder, ContactManifold) for physical queries.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#pragma once

#include "Physics/BoundingBox.hpp"
#include <simd/simd.h>

namespace KanVerse::Physics {
  
  struct ContactManifold {
    simd::float3 Normal{ 0.0f, 1.0f, 0.0f };
    simd::float3 Point{ 0.0f, 0.0f, 0.0f };
    float        PenetrationDepth{ 0.0f };
    bool         HasContact{ false };
  };
  
  struct CapsuleCollider {
    simd::float3 BasePoint{ 0.0f, 0.5f, 0.0f };
    simd::float3 TipPoint{ 0.0f, 1.5f, 0.0f };
    float        Radius{ 0.4f };
    
    [[nodiscard]] constexpr simd::float3 GetCenter() const noexcept { return (BasePoint + TipPoint) * 0.5f; }
    [[nodiscard]] constexpr float GetCylinderHeight() const noexcept { return simd::distance(TipPoint, BasePoint); }
    [[nodiscard]] simd::float3 ClosestPointOnSegment(simd::float3 point) const noexcept;
    [[nodiscard]] ContactManifold IntersectsOBB(const OBB& obb) const noexcept;
    [[nodiscard]] ContactManifold IntersectsAABB(const AABB& aabb) const noexcept;
  };
  
  struct CylinderCollider {
    simd::float3 Center{ 0.0f, 0.0f, 0.0f };
    simd::float3 Axis{ 0.0f, 1.0f, 0.0f };
    float        HalfHeight{ 0.5f };
    float        Radius{ 0.5f };
    
    [[nodiscard]] ContactManifold IntersectsOBB(const OBB& obb) const noexcept;
  };
  
} // namespace KanVerse::Physics
