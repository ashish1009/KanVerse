//
//  MathUtils.hpp
//  KanViz
//
//  Created by Ashish . on 25/11/24.
//

#pragma once

namespace KanViz::Utils::Math
{
  /// This enum stores the Axis name
  enum Axis : uint8_t
  {
    X, Y, Z
  };

  static constexpr glm::vec2 ZeroVec2 {0.0f};
  static constexpr glm::vec3 ZeroVec3 {0.0f};
  static constexpr glm::vec4 ZeroVec4 {0.0f};
  static constexpr glm::mat2 ZeroMat2 {0.0f};
  static constexpr glm::mat3 ZeroMat3 {0.0f};
  static constexpr glm::mat4 ZeroMat4 {0.0f};
  
  static constexpr glm::vec2 UnitVec2 {1.0f};
  static constexpr glm::vec3 UnitVec3 {1.0f};
  static constexpr glm::vec4 UnitVec4 {1.0f};
  static constexpr glm::mat2 UnitMat2 {1.0f};
  static constexpr glm::mat3 UnitMat3 {1.0f};
  static constexpr glm::mat4 UnitMat4 {1.0f};
  
  /// This function generates and returns the transform matrix from position scale and rotation
  /// - Parameters:
  ///   - position: Position of the component:
  ///   - rotation: Rotation of the component. Default glm vec3(0.0f):
  ///   - scale: Size of the component. Default glm vec3(1.0f):
  ///  - Important: Return value should not be discarded.
  ///  - Important: This function cost in CPU cycle
  [[nodiscard]] glm::mat4 GetTransformMatrix(const glm::vec3& position, const glm::vec3& rotation = ZeroVec3, const glm::vec3& scale = UnitVec3);
  
  /// This funciton decomposes the Transform matrix as position rotation and scale
  /// - Parameters:
  ///   - transform: Transfrom matrix to be decomposed:
  ///   - outTranslation: Position output
  ///   - outRotation: Rotation output
  ///   - outScale: Scale output
  ///  - Important: This function cost too much in CPU
  void DecomposeTransform(const glm::mat4& transform, glm::vec3& outTranslation, glm::vec3& outRotation, glm::vec3& outScale);

  /// This function calculates the squared distance of 2 points
  /// - Parameters:
  ///   - x1: x1 description
  ///   - y1: y1 description
  ///   - x2: x2 description
  ///   - y2: y2 description
  float GetDistanceSquared(float x1, float y1, float x2, float y2);
  /// This function calculates the distance of 2 points
  /// - Parameters:
  ///   - x1: x1 description
  ///   - y1: y1 description
  ///   - x2: x2 description
  ///   - y2: y2 description
  float GetDistance(float x1, float y1, float x2, float y2);
} // namespace KanViz::Utils::Math
