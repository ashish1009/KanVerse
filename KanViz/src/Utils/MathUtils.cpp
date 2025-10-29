//
//  MathUtils.cpp
//  KanViz
//
//  Created by Ashish . on 25/11/24.
//

#include "MathUtils.hpp"

namespace KanViz::Utils::Math
{
  glm::mat4 GetTransformMatrix(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
  {
    IK_PERFORMANCE("Math::GetTransformMatrix");

    // Convert Euler rotation to quaternion and then to rotation matrix
    glm::mat4 quadRotation {glm::toMat4(glm::quat(rotation))};

    // Combine translation, rotation, and scale into a single transformation matrix
    return glm::translate(glm::mat4(1.0f), position) * quadRotation * glm::scale(glm::mat4(1.0f), scale);
  }
  
  void DecomposeTransform(const glm::mat4& transform, glm::vec3& outTranslation, glm::vec3& outRotation, glm::vec3& outScale)
  {
    IK_PERFORMANCE("Math::DecomposeTransform");
    
    // Adapted from glm::decompose in matrix_decompose.inl
    using namespace glm;
    using T = float;
    
    // Copy the input matrix for modification
    mat4 LocalMatrix(transform);
    
    // Normalize the matrix by checking if the bottom-right value is valid
    if (epsilonEqual(LocalMatrix[3][3], static_cast<float>(0), epsilon<T>()))
    {
      return;
    }
    
    // Remove any perspective component from the matrix (if present)
    if ( epsilonNotEqual(LocalMatrix[0][3], static_cast<T>(0), epsilon<T>()) or
        epsilonNotEqual(LocalMatrix[1][3], static_cast<T>(0), epsilon<T>()) or
        epsilonNotEqual(LocalMatrix[2][3], static_cast<T>(0), epsilon<T>()))
    {
      // Clear the perspective partition
      LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<T>(0);
      LocalMatrix[3][3] = static_cast<T>(1);
    }
    
    // Extract translation from the matrix
    outTranslation = vec3(LocalMatrix[3]);
    LocalMatrix[3] = vec4(0, 0, 0, LocalMatrix[3].w);
    
    vec3 Row[3];
    
    // Copy upper-left 3x3 matrix (rotation + scale)
    for (length_t i = 0; i < 3; ++i)
    {
      for (length_t j = 0; j < 3; ++j)
      {
        Row[i][j] = LocalMatrix[i][j];
      }
    }
    
    // Compute scale factors and normalize the rows to remove scale
    outScale.x = length(Row[0]);
    Row[0] = detail::scale(Row[0], static_cast<T>(1));
    
    outScale.y = length(Row[1]);
    Row[1] = detail::scale(Row[1], static_cast<T>(1));
    
    outScale.z = length(Row[2]);
    Row[2] = detail::scale(Row[2], static_cast<T>(1));
    
    // Extract Euler angles (rotation)
    outRotation.y = asin(-Row[0][2]);

    // Handle Gimbal lock
    if (cos(outRotation.y) != 0)
    {
      outRotation.x = atan2(Row[1][2], Row[2][2]);
      outRotation.z = atan2(Row[0][1], Row[0][0]);
    }
    else
    {
      outRotation.x = atan2(-Row[2][0], Row[1][1]);
      outRotation.z = 0;
    }
  }
  
  float GetDistanceSquared(float x1, float y1, float x2, float y2)
  {
    return ((x2 - x1) * (x2 - x1)) + ((y2 - y1) * (y2 - y1));
  }
  
  float GetDistance(float x1, float y1, float x2, float y2)
  {
    return sqrt(GetDistanceSquared(x1, y1, x2, y2));
  }
  
} // namespace KanViz::Utils::Math
