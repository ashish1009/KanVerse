/*!
 * @file Math.hpp
 * @brief High-performance SIMD linear algebra types and transformations for KanVerse.
 * @details Wraps Apple's <simd/simd.h> with zero-overhead abstractions configured
 *          specifically for Metal's right-handed coordinate system and [0, 1] clip depth.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#pragma once

#include <simd/simd.h>
#include <cmath>

namespace KanVerse::Math {
  
  /// @brief 2-component floating-point vector backed by Apple SIMD.
  using Vec2 = simd::float2;
  
  /// @brief 3-component floating-point vector backed by Apple SIMD.
  using Vec3 = simd::float3;
  
  /// @brief 4-component floating-point vector backed by Apple SIMD.
  using Vec4 = simd::float4;
  
  /// @brief 4x4 column-major floating-point matrix backed by Apple SIMD.
  using Mat4 = simd::float4x4;
  
  /*!
   * @brief Constructs an explicit 2-component floating-point SIMD vector.
   * @param x Horizontal component.
   * @param y Vertical component.
   * @return Initialized 2D vector.
   */
  [[nodiscard]] constexpr Vec2 MakeVec2(float x, float y) noexcept {
    return simd_make_float2(x, y);
  }
  
  /*!
   * @brief Constructs an explicit 3-component floating-point SIMD vector.
   * @param x X coordinate.
   * @param y Y coordinate.
   * @param z Z coordinate.
   * @return Initialized 3D vector.
   */
  [[nodiscard]] constexpr Vec3 MakeVec3(float x, float y, float z) noexcept {
    return simd_make_float3(x, y, z);
  }
  
  /*!
   * @brief Constructs an explicit 4-component floating-point SIMD vector.
   * @param x X coordinate.
   * @param y Y coordinate.
   * @param z Z coordinate.
   * @param w W component.
   * @return Initialized 4D vector.
   */
  [[nodiscard]] constexpr Vec4 MakeVec4(float x, float y, float z, float w) noexcept {
    return simd_make_float4(x, y, z, w);
  }
  
  /*!
   * @brief Returns a 4x4 identity matrix.
   * @return Identity matrix.
   */
  [[nodiscard]] inline Mat4 Identity() noexcept {
    return matrix_identity_float4x4;
  }
  
  /*!
   * @brief Computes a perspective projection matrix targeting Metal depth space [0, 1].
   * @param fovRadians Vertical field of view in radians.
   * @param aspect Viewport aspect ratio (width / height).
   * @param nearZ Distance to the near clipping plane.
   * @param farZ Distance to the far clipping plane.
   * @return 4x4 perspective projection matrix.
   */
  [[nodiscard]] inline Mat4 Perspective(float fovRadians, float aspect, float nearZ, float farZ) noexcept {
    const float ys = 1.0f / std::tan(fovRadians * 0.5f);
    const float xs = ys / aspect;
    const float zs = farZ / (nearZ - farZ);
    
    Mat4 m = {};
    m.columns[0] = MakeVec4(xs,   0.0f, 0.0f,          0.0f);
    m.columns[1] = MakeVec4(0.0f, ys,   0.0f,          0.0f);
    m.columns[2] = MakeVec4(0.0f, 0.0f, zs,           -1.0f);
    m.columns[3] = MakeVec4(0.0f, 0.0f, zs * nearZ,    0.0f);
    return m;
  }
  
  /*!
   * @brief Computes an orthographic projection matrix targeting Metal depth space [0, 1].
   * @param left Minimum horizontal boundary.
   * @param right Maximum horizontal boundary.
   * @param bottom Minimum vertical boundary.
   * @param top Maximum vertical boundary.
   * @param nearZ Distance to the near clipping plane.
   * @param farZ Distance to the far clipping plane.
   * @return 4x4 column-major orthographic projection matrix.
   */
  [[nodiscard]] inline Mat4 Ortho(float left, float right, float bottom, float top, float nearZ, float farZ) noexcept {
    const float rml = right - left;
    const float tmb = top - bottom;
    const float fmn = farZ - nearZ;
    
    Mat4 m = {};
    m.columns[0] = MakeVec4(2.0f / rml,           0.0f,                0.0f,          0.0f);
    m.columns[1] = MakeVec4(0.0f,                 2.0f / tmb,          0.0f,          0.0f);
    m.columns[2] = MakeVec4(0.0f,                 0.0f,               -1.0f / fmn,    0.0f);
    m.columns[3] = MakeVec4(-(right + left) / rml, -(top + bottom) / tmb, -nearZ / fmn,  1.0f);
    return m;
  }
  
  /*!
   * @brief Computes a right-handed LookAt view matrix.
   * @param eye Camera world position.
   * @param target Target point the camera looks toward.
   * @param up Normalized up direction vector.
   * @return 4x4 view matrix.
   */
  [[nodiscard]] inline Mat4 LookAt(Vec3 eye, Vec3 target, Vec3 up) noexcept {
    const Vec3 zAxis = simd::normalize(eye - target);
    const Vec3 xAxis = simd::normalize(simd::cross(up, zAxis));
    const Vec3 yAxis = simd::cross(zAxis, xAxis);
    
    Mat4 m = matrix_identity_float4x4;
    m.columns[0] = MakeVec4(xAxis.x, yAxis.x, zAxis.x, 0.0f);
    m.columns[1] = MakeVec4(xAxis.y, yAxis.y, zAxis.y, 0.0f);
    m.columns[2] = MakeVec4(xAxis.z, yAxis.z, zAxis.z, 0.0f);
    m.columns[3] = MakeVec4(-simd::dot(xAxis, eye), -simd::dot(yAxis, eye), -simd::dot(zAxis, eye), 1.0f);
    return m;
  }
  
  /*!
   * @brief Computes a 4x4 rotation matrix around an arbitrary unit axis.
   * @param angleRadians Angle of rotation in radians.
   * @param axis Axis about which rotation occurs.
   * @return 4x4 rotation matrix.
   */
  [[nodiscard]] inline Mat4 Rotate(float angleRadians, Vec3 axis) noexcept {
    const Vec3 a = simd::normalize(axis);
    const float cosA = std::cos(angleRadians);
    const float sinA = std::sin(angleRadians);
    const float k = 1.0f - cosA;
    
    Mat4 m = {};
    m.columns[0] = MakeVec4(a.x * a.x * k + cosA,       a.x * a.y * k + a.z * sinA, a.x * a.z * k - a.y * sinA, 0.0f);
    m.columns[1] = MakeVec4(a.x * a.y * k - a.z * sinA, a.y * a.y * k + cosA,       a.y * a.z * k + a.x * sinA, 0.0f);
    m.columns[2] = MakeVec4(a.x * a.z * k + a.y * sinA, a.y * a.z * k - a.x * sinA, a.z * a.z * k + cosA,       0.0f);
    m.columns[3] = MakeVec4(0.0f,                       0.0f,                       0.0f,                       1.0f);
    return m;
  }
  
  /*!
   * @brief Multiplies two 4x4 matrices using SIMD vector instructions.
   * @param lhs Left-hand matrix operand.
   * @param rhs Right-hand matrix operand.
   * @return Resulting 4x4 product matrix.
   */
  [[nodiscard]] inline Mat4 Multiply(Mat4 lhs, Mat4 rhs) noexcept {
    return matrix_multiply(lhs, rhs);
  }
  
  /*!
   * @brief Computes the inverse of a 4x4 matrix using SIMD acceleration.
   * @param matrix The source 4x4 transformation matrix.
   * @return Inverted 4x4 matrix.
   */
  [[nodiscard]] inline Mat4 Inverse(const Mat4& matrix) noexcept {
    return simd_inverse(matrix);
  }
  
  /*!
   * @brief Computes the transpose of a 4x4 matrix using SIMD acceleration.
   * @param matrix The source 4x4 transformation matrix.
   * @return Transposed 4x4 matrix.
   */
  [[nodiscard]] inline Mat4 Transpose(const Mat4& matrix) noexcept {
    return simd_transpose(matrix);
  }
} // namespace KanVerse::Math
