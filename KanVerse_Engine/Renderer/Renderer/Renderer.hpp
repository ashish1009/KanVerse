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
#include "Renderer/PostProcess/BloomPass.hpp"
#include "Renderer/Textures/TextureCube.hpp"
#include "Renderer/Camera/PerspectiveCamera.hpp"
#include <span>
#include <vector>

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
   * @brief Environment lighting source mode.
   */
  enum class EnvironmentMode : uint8_t {
    CrispHDR = 0, ///< High-dynamic-range equirectangular skybox.
    Irradiance,   ///< Convolved diffuse irradiance skybox.
    Procedural    ///< Analytic gradient procedural skybox.
  };
  
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
    
    /*!
     * @brief Begins the main HDR forward rendering pass.
     * @param camera Active perspective camera.
     * @param sun Primary directional light.
     * @param env Global environment ambient settings.
     * @param target Optional target framebuffer (defaults to swapchain if nullptr).
     * @param pointLights Span of active point lights in the scene.
     */
    static void BeginScene(const PerspectiveCamera& camera,
                           const DirectionalLight& sun,
                           const EnvironmentLighting& env,
                           const std::shared_ptr<Framebuffer>& target = nullptr,
                           std::span<const GPUPointLight> pointLights = {});
    
    /*!
     * @brief Begins the depth-only directional shadow map pass.
     * @param sun Primary directional light.
     * @param shadowTarget Depth-only shadow framebuffer.
     */
    static void BeginShadowPass(const DirectionalLight& sun, const std::shared_ptr<Framebuffer>& shadowTarget);
    
    /*!
     * @brief Submits an instanced draw call for a mesh and material batch.
     * @param pipeline Pipeline state to bind.
     * @param material Surface material (ignored during shadow pass).
     * @param mesh Geometry mesh to draw.
     * @param instances Span of world and normal matrices for all instances.
     */
    static void SubmitInstanced(const std::shared_ptr<Pipeline>& pipeline,
                                const std::shared_ptr<Material>& material,
                                const std::shared_ptr<Mesh>& mesh,
                                std::span<const InstanceData> instances);
    
    /*!
     * @brief Ends the current render pass, executes compute bloom and tonemapping, and presents the drawable.
     */
    static void EndScene();
    
    static void SetShadowMap(const std::shared_ptr<Framebuffer>& shadowFramebuffer);
    static void SetEnvironmentMap(const std::shared_ptr<TextureCube>& textureCube);
    static void SetIrradianceMap(const std::shared_ptr<TextureCube>& irradianceMap);
    static void SetSkyboxToIrradiance(bool useIrradiance);
    static void SetEnvironmentMode(EnvironmentMode mode);
    [[nodiscard]] static EnvironmentMode GetEnvironmentMode() noexcept;
    
    [[nodiscard]] static std::shared_ptr<Pipeline> GetStandardPipeline() noexcept;
    [[nodiscard]] static std::shared_ptr<Pipeline> GetTransparentPipeline() noexcept;
    [[nodiscard]] static std::shared_ptr<Pipeline> GetShadowPipeline() noexcept;
    [[nodiscard]] static std::shared_ptr<Framebuffer> GetHDRFramebuffer() noexcept;
    [[nodiscard]] static std::shared_ptr<Framebuffer> GetShadowMapFramebuffer() noexcept;
    [[nodiscard]] static BufferLayout GetStandardMeshLayout() noexcept;
    
    static void OnWindowResize(uint32_t width, uint32_t height);
    [[nodiscard]] static BloomPass& GetBloomPass() noexcept;
    static void SetExposure(float exposure) noexcept;
    [[nodiscard]] static float GetExposure() noexcept;
    static void SetIBLEnabled(bool enabled) noexcept;
    [[nodiscard]] static bool IsIBLEnabled() noexcept;
    
  private:
    struct alignas(256) GPUUniforms {
      Math::Mat4    ViewProjectionMatrix;
      Math::Mat4    LightSpaceMatrix;
      Math::Vec3    CameraPosition;
      Math::Vec3    LightDirection;
      Math::Vec4    DirectionalColor;
      Math::Vec4    AmbientColor;
      uint32_t      PointLightCount{0};
      uint32_t      Padding[3]{0, 0, 0};
      GPUPointLight PointLights[kMaxPointLights];
    };
    
    struct alignas(256) ShadowUniforms {
      Math::Mat4 LightViewProjectionMatrix;
    };
    
    struct alignas(16) TonemapUniforms {
      float BloomIntensity{0.8f};
      float Exposure{1.0f};
      float Padding[2]{0.0f, 0.0f};
    };
    
    struct RendererStorage {
      static constexpr size_t MaxInstancesPerFrame = 16384;
      static constexpr size_t InstanceBufferSize   = MaxInstancesPerFrame * sizeof(InstanceData);
      SceneData                       ActiveScene;
      FrameSynchronizer               Sync{3};
      std::vector<MTL::Buffer*>       UniformBuffers;
      std::vector<MTL::Buffer*>       ShadowUniformBuffers;
      std::vector<MTL::Buffer*>       InstanceBuffers;
      size_t                          CurrentInstanceOffset{0};
      MTL::SamplerState*              ShadowSampler{nullptr};
      MTL::SamplerState*              LinearSampler{nullptr};
      MTL::RenderPipelineState*       TonemapPipeline{nullptr};
      MTL::RenderPipelineState*       SkyboxPipeline{nullptr};
      MTL::DepthStencilState*         SkyboxDepthState{nullptr};
      std::shared_ptr<Pipeline>       StandardPipeline{nullptr};
      std::shared_ptr<Pipeline>       TransparentPipeline{nullptr};
      std::shared_ptr<Pipeline>       ShadowPipeline{nullptr};
      std::shared_ptr<Framebuffer>    HDRFramebuffer{nullptr};
      std::shared_ptr<Framebuffer>    ShadowMap{nullptr};
      std::shared_ptr<Framebuffer>    ActiveShadowMap{nullptr};
      std::shared_ptr<TextureCube>    ActiveEnvironmentMap{nullptr};
      std::shared_ptr<TextureCube>    ActiveIrradianceMap{nullptr};
      std::shared_ptr<TextureCube>    ActiveProceduralMap{nullptr};
      EnvironmentMode                 CurrentEnvMode{EnvironmentMode::CrispHDR};
      bool                            EnableIBL{true};
      std::shared_ptr<Texture2D>      DefaultWhiteTexture{nullptr};
      bool                            RenderIrradianceSkybox{false};
      BloomPass                       Bloom;
      float                           Exposure{1.0f};
      std::shared_ptr<Framebuffer>    ActiveFramebuffer{nullptr};
      MTL::CommandBuffer*             ActiveCommandBuffer{nullptr};
      MTL::RenderCommandEncoder*      ActiveCommandEncoder{nullptr};
      uint32_t                        ActiveFrameIndex{0};
      bool                            IsShadowPass{false};
    };
    
    static std::unique_ptr<RendererStorage> s_Storage;
  };
  
} // namespace KanVerse
