/*!
 * @file Renderer.hpp
 * @brief High-level Metal rendering orchestrator with instancing, shadows, transparency, IBL, point lights, and bloom.
 * @copyright KanVerse Engine
 */

#pragma once

#include "Core/Math/Math.hpp"
#include "Renderer/Buffers/IndexBuffer.hpp"
#include "Renderer/Buffers/VertexBuffer.hpp"
#include "Renderer/Framebuffer/Framebuffer.hpp"
#include "Renderer/Geometry/Mesh.hpp"
#include "Renderer/Lighting/DirectionalLight.hpp"
#include "Renderer/Lighting/EnvironmentLighting.hpp"
#include "Renderer/Materials/Material.hpp"
#include "Renderer/Pipeline/FrameSync.hpp"
#include "Renderer/Pipeline/Pipeline.hpp"
#include "Renderer/Camera/PerspectiveCamera.hpp"

namespace MTL {
  class Buffer;
  class CommandBuffer;
  class RenderCommandEncoder;
  class SamplerState;
  class RenderPipelineState;
  class DepthStencilState;
}

namespace KanVerse {
  /*!
   * @brief Per-instance transformation matrices uploaded to vertex buffer(3).
   */
  struct alignas(16) InstanceData {
    Math::Mat4 ModelMatrix;  ///< Local-to-world transformation matrix.
    Math::Mat4 NormalMatrix; ///< Inverse-transpose matrix for normal transformation.
  };
  
  /*!
   * @brief GPU-aligned point light structure carrying world position, color, intensity, and physical attenuation.
   */
  struct alignas(16) GPUPointLight {
    Math::Vec4 PositionAndRadius;  ///< xyz = World Position, w = Cutoff Radius.
    Math::Vec4 ColorAndIntensity;  ///< rgb = Linear Color, a = Luminous Intensity.
    Math::Vec4 Attenuation;        ///< x = Constant, y = Linear, z = Quadratic, w = Unused.
  };
  
  /*!
   * @brief Per-frame scene parameters cached during BeginScene.
   */
  struct SceneData {
    Math::Mat4          ViewProjectionMatrix;
    DirectionalLight    SunLight;
    EnvironmentLighting Environment;
  };

  /*!
   * @brief Static Metal rendering engine managing command buffers, passes, and global GPU state.
   */
  class Renderer {
  public:
    static constexpr uint32_t kMaxPointLights = 64; ///< Maximum active point lights per frame.
    
    /*!
     * @brief Initializes global Metal buffers, samplers, framebuffers, and pipelines.
     */
    static void Init();
    
    /*!
     * @brief Releases all GPU resources owned by the Renderer.
     */
    static void Shutdown();
    
  private:
    struct RendererStorage {
    };
    static std::unique_ptr<RendererStorage> s_Storage;
  };
} // namespace KanVerse
