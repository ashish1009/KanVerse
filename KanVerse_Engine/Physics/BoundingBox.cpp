/*!
 * @file BoundingBox.cpp
 * @brief Implementation of AABB and OBB spatial queries and raycast projections.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#include "Physics/BoundingBox.hpp"
#include <algorithm>
#include <cmath>

namespace KanVerse::Physics {
  
  // --- AABB Implementation ---
  
  simd::float3 AABB::ClosestPoint(simd::float3 point) const noexcept {
    return simd::clamp(point, Min, Max);
  }
  
  bool AABB::Contains(simd::float3 point) const noexcept {
    return (point.x >= Min.x && point.x <= Max.x) &&
    (point.y >= Min.y && point.y <= Max.y) &&
    (point.z >= Min.z && point.z <= Max.z);
  }
  
  bool AABB::Intersects(const AABB& other) const noexcept {
    return (Min.x <= other.Max.x && Max.x >= other.Min.x) &&
    (Min.y <= other.Max.y && Max.y >= other.Min.y) &&
    (Min.z <= other.Max.z && Max.z >= other.Min.z);
  }
  
  std::optional<RaycastHit> AABB::IntersectsRay(const Ray& ray, float maxDistance) const noexcept {
    float tMin = 0.0f;
    float tMax = maxDistance;
    simd::float3 hitNormal{0.0f, 0.0f, 0.0f};
    
    for (int i = 0; i < 3; ++i) {
      const float originVal = ray.Origin[i];
      const float dirVal = ray.Direction[i];
      const float minVal = Min[i];
      const float maxVal = Max[i];
      
      if (std::abs(dirVal) < 1e-6f) {
        // Ray is parallel to slab
        if (originVal < minVal || originVal > maxVal) {
          return std::nullopt;
        }
      } else {
        const float invDir = 1.0f / dirVal;
        float t1 = (minVal - originVal) * invDir;
        float t2 = (maxVal - originVal) * invDir;
        simd::float3 normal1{0.0f, 0.0f, 0.0f};
        simd::float3 normal2{0.0f, 0.0f, 0.0f};
        normal1[i] = -1.0f;
        normal2[i] = 1.0f;
        
        if (t1 > t2) {
          std::swap(t1, t2);
          std::swap(normal1, normal2);
        }
        
        if (t1 > tMin) {
          tMin = t1;
          hitNormal = normal1;
        }
        tMax = std::min(tMax, t2);
        
        if (tMin > tMax) {
          return std::nullopt;
        }
      }
    }
    
    if (tMin < 0.0f || tMin > maxDistance) {
      return std::nullopt;
    }
    
    RaycastHit hit;
    hit.Distance = tMin;
    hit.Point = ray.Origin + ray.Direction * tMin;
    hit.Normal = hitNormal;
    return hit;
  }
  
  // --- OBB Implementation ---
  
  OBB OBB::FromTransform(simd::float3 center, simd::float3 halfExtents, const simd::float4x4& rotationMatrix) noexcept {
    OBB obb;
    obb.Center = center;
    obb.HalfExtents = halfExtents;
    obb.Axes[0] = simd::normalize(simd::float3{rotationMatrix.columns[0].x, rotationMatrix.columns[0].y, rotationMatrix.columns[0].z});
    obb.Axes[1] = simd::normalize(simd::float3{rotationMatrix.columns[1].x, rotationMatrix.columns[1].y, rotationMatrix.columns[1].z});
    obb.Axes[2] = simd::normalize(simd::float3{rotationMatrix.columns[2].x, rotationMatrix.columns[2].y, rotationMatrix.columns[2].z});
    return obb;
  }
  
  simd::float3 OBB::ClosestPoint(simd::float3 point) const noexcept {
    const simd::float3 d = point - Center;
    simd::float3 closest = Center;
    
    for (int i = 0; i < 3; ++i) {
      const float dist = simd::dot(d, Axes[i]);
      const float clampedDist = std::clamp(dist, -HalfExtents[i], HalfExtents[i]);
      closest += Axes[i] * clampedDist;
    }
    return closest;
  }
  
  bool OBB::Contains(simd::float3 point) const noexcept {
    const simd::float3 d = point - Center;
    for (int i = 0; i < 3; ++i) {
      const float dist = simd::dot(d, Axes[i]);
      if (std::abs(dist) > HalfExtents[i]) {
        return false;
      }
    }
    return true;
  }
  
  bool OBB::Intersects(const OBB& other) const noexcept {
    // 15 Separating Axis Theorem (SAT) tests
    float R[3][3];
    float AbsR[3][3];
    
    for (int i = 0; i < 3; ++i) {
      for (int j = 0; j < 3; ++j) {
        R[i][j] = simd::dot(Axes[i], other.Axes[j]);
        AbsR[i][j] = std::abs(R[i][j]) + 1e-5f; // Small epsilon against parallel edge cases
      }
    }
    
    const simd::float3 translation = other.Center - Center;
    const simd::float3 t{
      simd::dot(translation, Axes[0]),
      simd::dot(translation, Axes[1]),
      simd::dot(translation, Axes[2])
    };
    
    // Test axes L = Axes[0], Axes[1], Axes[2]
    for (int i = 0; i < 3; ++i) {
      const float ra = HalfExtents[i];
      const float rb = other.HalfExtents[0] * AbsR[i][0] + other.HalfExtents[1] * AbsR[i][1] + other.HalfExtents[2] * AbsR[i][2];
      if (std::abs(t[i]) > ra + rb) return false;
    }
    
    // Test axes L = other.Axes[0], other.Axes[1], other.Axes[2]
    for (int j = 0; j < 3; ++j) {
      const float ra = HalfExtents[0] * AbsR[0][j] + HalfExtents[1] * AbsR[1][j] + HalfExtents[2] * AbsR[2][j];
      const float rb = other.HalfExtents[j];
      const float proj = std::abs(t[0] * R[0][j] + t[1] * R[1][j] + t[2] * R[2][j]);
      if (proj > ra + rb) return false;
    }
    
    // Test 9 cross-product axes: Axes[i] x other.Axes[j]
    // L = Axes[0] x other.Axes[0]
    if (std::abs(t[2] * R[1][0] - t[1] * R[2][0]) >
        HalfExtents[1] * AbsR[2][0] + HalfExtents[2] * AbsR[1][0] +
        other.HalfExtents[1] * AbsR[0][2] + other.HalfExtents[2] * AbsR[0][1]) return false;
    
    // L = Axes[0] x other.Axes[1]
    if (std::abs(t[2] * R[1][1] - t[1] * R[2][1]) >
        HalfExtents[1] * AbsR[2][1] + HalfExtents[2] * AbsR[1][1] +
        other.HalfExtents[0] * AbsR[0][2] + other.HalfExtents[2] * AbsR[0][0]) return false;
    
    // L = Axes[0] x other.Axes[2]
    if (std::abs(t[2] * R[1][2] - t[1] * R[2][2]) >
        HalfExtents[1] * AbsR[2][2] + HalfExtents[2] * AbsR[1][2] +
        other.HalfExtents[0] * AbsR[0][1] + other.HalfExtents[1] * AbsR[0][0]) return false;
    
    // L = Axes[1] x other.Axes[0]
    if (std::abs(t[0] * R[2][0] - t[2] * R[0][0]) >
        HalfExtents[0] * AbsR[2][0] + HalfExtents[2] * AbsR[0][0] +
        other.HalfExtents[1] * AbsR[1][2] + other.HalfExtents[2] * AbsR[1][1]) return false;
    
    // L = Axes[1] x other.Axes[1]
    if (std::abs(t[0] * R[2][1] - t[2] * R[0][1]) >
        HalfExtents[0] * AbsR[2][1] + HalfExtents[2] * AbsR[0][1] +
        other.HalfExtents[0] * AbsR[1][2] + other.HalfExtents[2] * AbsR[1][0]) return false;
    
    // L = Axes[1] x other.Axes[2]
    if (std::abs(t[0] * R[2][2] - t[2] * R[0][2]) >
        HalfExtents[0] * AbsR[2][2] + HalfExtents[2] * AbsR[0][2] +
        other.HalfExtents[0] * AbsR[1][1] + other.HalfExtents[1] * AbsR[1][0]) return false;
    
    // L = Axes[2] x other.Axes[0]
    if (std::abs(t[1] * R[0][0] - t[0] * R[1][0]) >
        HalfExtents[0] * AbsR[1][0] + HalfExtents[1] * AbsR[0][0] +
        other.HalfExtents[1] * AbsR[2][2] + other.HalfExtents[2] * AbsR[2][1]) return false;
    
    // L = Axes[2] x other.Axes[1]
    if (std::abs(t[1] * R[0][1] - t[0] * R[1][1]) >
        HalfExtents[0] * AbsR[1][1] + HalfExtents[1] * AbsR[0][1] +
        other.HalfExtents[0] * AbsR[2][2] + other.HalfExtents[2] * AbsR[2][0]) return false;
    
    // L = Axes[2] x other.Axes[2]
    if (std::abs(t[1] * R[0][2] - t[0] * R[1][2]) >
        HalfExtents[0] * AbsR[1][2] + HalfExtents[1] * AbsR[0][2] +
        other.HalfExtents[0] * AbsR[2][1] + other.HalfExtents[1] * AbsR[2][0]) return false;
    
    return true;
  }
  
  std::optional<RaycastHit> OBB::IntersectsRay(const Ray& ray, float maxDistance) const noexcept {
    // Transform ray into OBB's local coordinate frame
    const simd::float3 d = ray.Origin - Center;
    Ray localRay;
    localRay.Origin = simd::float3{
      simd::dot(d, Axes[0]),
      simd::dot(d, Axes[1]),
      simd::dot(d, Axes[2])
    };
    localRay.Direction = simd::float3{
      simd::dot(ray.Direction, Axes[0]),
      simd::dot(ray.Direction, Axes[1]),
      simd::dot(ray.Direction, Axes[2])
    };
    
    const AABB localAABB{-HalfExtents, HalfExtents};
    auto hit = localAABB.IntersectsRay(localRay, maxDistance);
    
    if (!hit) {
      return std::nullopt;
    }
    
    // Transform normal back to world coordinates
    const simd::float3 localNormal = hit->Normal;
    hit->Normal = Axes[0] * localNormal.x + Axes[1] * localNormal.y + Axes[2] * localNormal.z;
    hit->Point = ray.Origin + ray.Direction * hit->Distance;
    return hit;
  }
  
} // namespace KanVerse::Physics
