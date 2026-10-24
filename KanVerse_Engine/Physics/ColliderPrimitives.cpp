/*!
 * @file ColliderPrimitives.cpp
 * @brief Implementation of capsule and cylinder narrowphase contact generation.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#include "Physics/ColliderPrimitives.hpp"
#include <algorithm>
#include <cmath>

namespace KanVerse::Physics {
  
  // --- CapsuleCollider Implementation ---
  
  simd::float3 CapsuleCollider::ClosestPointOnSegment(simd::float3 point) const noexcept {
    const simd::float3 ab = TipPoint - BasePoint;
    const float abLenSq = simd::length_squared(ab);
    
    if (abLenSq < 1e-6f) {
      return BasePoint;
    }
    
    const float t = std::clamp(simd::dot(point - BasePoint, ab) / abLenSq, 0.0f, 1.0f);
    return BasePoint + ab * t;
  }
  
  ContactManifold CapsuleCollider::IntersectsOBB(const OBB& obb) const noexcept {
    ContactManifold manifold;
    
    // Transform capsule segment into OBB's local coordinate frame
    const simd::float3 dBase = BasePoint - obb.Center;
    const simd::float3 dTip  = TipPoint - obb.Center;
    
    const simd::float3 localBase{
      simd::dot(dBase, obb.Axes[0]),
      simd::dot(dBase, obb.Axes[1]),
      simd::dot(dBase, obb.Axes[2])
    };
    
    const simd::float3 localTip{
      simd::dot(dTip, obb.Axes[0]),
      simd::dot(dTip, obb.Axes[1]),
      simd::dot(dTip, obb.Axes[2])
    };
    
    const AABB localAABB{-obb.HalfExtents, obb.HalfExtents};
    
    // Iteratively find closest points between the line segment and the box
    simd::float3 segPoint = (localBase + localTip) * 0.5f;
    simd::float3 boxPoint = localAABB.ClosestPoint(segPoint);
    
    // Refine segment point towards closest box point
    const simd::float3 localAB = localTip - localBase;
    const float abLenSq = simd::length_squared(localAB);
    if (abLenSq > 1e-6f) {
      const float t = std::clamp(simd::dot(boxPoint - localBase, localAB) / abLenSq, 0.0f, 1.0f);
      segPoint = localBase + localAB * t;
      boxPoint = localAABB.ClosestPoint(segPoint);
    }
    
    const simd::float3 localDelta = segPoint - boxPoint;
    const float distSq = simd::length_squared(localDelta);
    
    if (distSq < Radius * Radius) {
      const float dist = std::sqrt(std::max(distSq, 1e-8f));
      const simd::float3 localNormal = (dist > 1e-4f) ? (localDelta / dist) : simd::float3{0.0f, 1.0f, 0.0f};
      
      // Convert normal and contact point back to world space
      manifold.Normal = obb.Axes[0] * localNormal.x +
      obb.Axes[1] * localNormal.y +
      obb.Axes[2] * localNormal.z;
      
      manifold.Point = obb.Center +
      obb.Axes[0] * boxPoint.x +
      obb.Axes[1] * boxPoint.y +
      obb.Axes[2] * boxPoint.z;
      
      manifold.PenetrationDepth = Radius - dist;
      manifold.HasContact = true;
    }
    
    return manifold;
  }
  
  ContactManifold CapsuleCollider::IntersectsAABB(const AABB& aabb) const noexcept {
    OBB obb;
    obb.Center = aabb.GetCenter();
    obb.HalfExtents = aabb.GetExtents();
    obb.Axes[0] = simd::float3{1.0f, 0.0f, 0.0f};
    obb.Axes[1] = simd::float3{0.0f, 1.0f, 0.0f};
    obb.Axes[2] = simd::float3{0.0f, 0.0f, 1.0f};
    return IntersectsOBB(obb);
  }
  
  // --- CylinderCollider Implementation ---
  
  ContactManifold CylinderCollider::IntersectsOBB(const OBB& obb) const noexcept {
    [[maybe_unused]] ContactManifold manifold;
    
    // Approximate cylinder core segment
    const simd::float3 tip  = Center + Axis * HalfHeight;
    const simd::float3 base = Center - Axis * HalfHeight;
    
    CapsuleCollider proxyCapsule{
      .BasePoint = base,
      .TipPoint = tip,
      .Radius = Radius
    };
    
    return proxyCapsule.IntersectsOBB(obb);
  }
  
} // namespace KanVerse::Physics
