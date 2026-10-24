/*!
 * @file BoundingBox.hpp
 * @brief Axis-Aligned (AABB) and Oriented Bounding Box (OBB) geometric primitives.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#pragma once

#include <simd/simd.h>
#include <optional>

namespace KanVerse::Physics {
  
  struct Ray {
    simd::float3 Origin{ 0.0f, 0.0f, 0.0f };
    simd::float3 Direction{ 0.0f, 0.0f, -1.0f };
  };
  
  struct RaycastHit {
    float        Distance{ 0.0f };
    simd::float3 Point{ 0.0f, 0.0f, 0.0f };
    simd::float3 Normal{ 0.0f, 1.0f, 0.0f };
  };
  
  struct AABB {
    simd::float3 Min{ -0.5f, -0.5f, -0.5f };
    simd::float3 Max{  0.5f,  0.5f,  0.5f };
    
    [[nodiscard]] constexpr simd::float3 GetCenter() const noexcept { return (Min + Max) * 0.5f; }
    [[nodiscard]] constexpr simd::float3 GetExtents() const noexcept { return (Max - Min) * 0.5f; }
    [[nodiscard]] simd::float3 ClosestPoint(simd::float3 point) const noexcept;
    [[nodiscard]] bool Contains(simd::float3 point) const noexcept;
    [[nodiscard]] bool Intersects(const AABB& other) const noexcept;
    [[nodiscard]] std::optional<RaycastHit> IntersectsRay(const Ray& ray, float maxDistance = 1000.0f) const noexcept;
  };
  
  struct OBB {
    simd::float3 Center{ 0.0f, 0.0f, 0.0f };
    simd::float3 HalfExtents{ 0.5f, 0.5f, 0.5f };
    simd::float3 Axes[3]{
      { 1.0f, 0.0f, 0.0f },
      { 0.0f, 1.0f, 0.0f },
      { 0.0f, 0.0f, 1.0f }
    };
    
    [[nodiscard]] static OBB FromTransform(simd::float3 center, simd::float3 halfExtents, const simd::float4x4& rotationMatrix) noexcept;
    [[nodiscard]] simd::float3 ClosestPoint(simd::float3 point) const noexcept;
    [[nodiscard]] bool Contains(simd::float3 point) const noexcept;
    [[nodiscard]] bool Intersects(const OBB& other) const noexcept;
    [[nodiscard]] std::optional<RaycastHit> IntersectsRay(const Ray& ray, float maxDistance = 1000.0f) const noexcept;
  };
  
} // namespace KanVerse::Physics
