/*!
 * @file Renderer.mm
 * @brief Metal implementation of the KanVerse Renderer with opaque, transparent, shadow, IBL, and point light support.
 * @copyright KanVerse Engine
 */
#include "Renderer.hpp"
#include "Renderer/Renderer/RenderCommand.hpp"
#include "Core/Diagnostics/Log.hpp"
#include "Core/Diagnostics/Assert.hpp"
#include "Core/Application/Application.hpp"
#include "Platform/Window/NativeWindow.hpp"
#import <MetalKit/MetalKit.h>
#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include <algorithm>
#include <cstring>

namespace KanVerse {
  
  std::unique_ptr<Renderer::RendererStorage> Renderer::s_Storage = nullptr;
  
  /*!
   * @brief Allocates triple-buffered uniform/instance buffers, samplers, framebuffers, and pipelines.
   */
  void Renderer::Init() {
    KV_CORE_ASSERT(!s_Storage, "[KanVerse Renderer] Renderer already initialized!");
    s_Storage = std::make_unique<RendererStorage>();
    
    MTL::Device* device = Application::Get().GetRenderContext()->GetDevice();
    const uint32_t maxFrames = s_Storage->Sync.GetMaxFramesInFlight();
    
    s_Storage->UniformBuffers.resize(maxFrames);
    s_Storage->ShadowUniformBuffers.resize(maxFrames);
    s_Storage->InstanceBuffers.resize(maxFrames);
    
    for (uint32_t i = 0; i < maxFrames; ++i) {
      s_Storage->UniformBuffers[i] = device->newBuffer(sizeof(GPUUniforms), MTL::ResourceStorageModeShared);
      KV_CORE_ASSERT(s_Storage->UniformBuffers[i] != nullptr, "[KanVerse Renderer] Failed to allocate uniform buffer!");
      
      s_Storage->ShadowUniformBuffers[i] = device->newBuffer(sizeof(ShadowUniforms), MTL::ResourceStorageModeShared);
      KV_CORE_ASSERT(s_Storage->ShadowUniformBuffers[i] != nullptr, "[KanVerse Renderer] Failed to allocate shadow uniform buffer!");
      
      s_Storage->InstanceBuffers[i] = device->newBuffer(RendererStorage::InstanceBufferSize, MTL::ResourceStorageModeShared);
      KV_CORE_ASSERT(s_Storage->InstanceBuffers[i] != nullptr, "[KanVerse Renderer] Failed to allocate instancing buffer!");
    }
    
    MTL::SamplerDescriptor* shadowSamplerDesc = MTL::SamplerDescriptor::alloc()->init();
    shadowSamplerDesc->setMinFilter(MTL::SamplerMinMagFilterLinear);
    shadowSamplerDesc->setMagFilter(MTL::SamplerMinMagFilterLinear);
    shadowSamplerDesc->setMipFilter(MTL::SamplerMipFilterNotMipmapped);
    shadowSamplerDesc->setSAddressMode(MTL::SamplerAddressModeClampToEdge);
    shadowSamplerDesc->setTAddressMode(MTL::SamplerAddressModeClampToEdge);
    shadowSamplerDesc->setCompareFunction(MTL::CompareFunctionLessEqual);
    s_Storage->ShadowSampler = device->newSamplerState(shadowSamplerDesc);
    shadowSamplerDesc->release();
    
    MTL::SamplerDescriptor* linearSamplerDesc = MTL::SamplerDescriptor::alloc()->init();
    linearSamplerDesc->setMinFilter(MTL::SamplerMinMagFilterLinear);
    linearSamplerDesc->setMagFilter(MTL::SamplerMinMagFilterLinear);
    linearSamplerDesc->setMipFilter(MTL::SamplerMipFilterLinear);
    linearSamplerDesc->setSAddressMode(MTL::SamplerAddressModeClampToEdge);
    linearSamplerDesc->setTAddressMode(MTL::SamplerAddressModeClampToEdge);
    s_Storage->LinearSampler = device->newSamplerState(linearSamplerDesc);
    linearSamplerDesc->release();
    
    s_Storage->ActiveProceduralMap = TextureCube::CreateProcedural(256);
    s_Storage->Bloom.Init();
    
    TextureSpecification whiteSpec{.Width = 1, .Height = 1, .GenerateMipmaps = false};
    s_Storage->DefaultWhiteTexture = Texture2D::Create(whiteSpec);
    uint32_t whitePixel = 0xFFFFFFFF;
    s_Storage->DefaultWhiteTexture->SetData(&whitePixel, sizeof(uint32_t));
    
    MTL::Library* defaultLib = device->newDefaultLibrary();
    if (defaultLib) {
      auto vertStr = NS::String::string("tonemap_vertex", NS::UTF8StringEncoding);
      auto fragStr = NS::String::string("tonemap_fragment", NS::UTF8StringEncoding);
      MTL::Function* vertFunc = defaultLib->newFunction(vertStr);
      MTL::Function* fragFunc = defaultLib->newFunction(fragStr);
      vertStr->release();
      fragStr->release();
      
      if (vertFunc && fragFunc) {
        auto pipeDesc = MTL::RenderPipelineDescriptor::alloc()->init();
        pipeDesc->setVertexFunction(vertFunc);
        pipeDesc->setFragmentFunction(fragFunc);
        pipeDesc->colorAttachments()->object(0)->setPixelFormat(MTL::PixelFormatBGRA8Unorm_sRGB);
        pipeDesc->setDepthAttachmentPixelFormat(MTL::PixelFormatDepth32Float);
        NS::Error* err = nullptr;
        s_Storage->TonemapPipeline = device->newRenderPipelineState(pipeDesc, &err);
        pipeDesc->release();
        if (!s_Storage->TonemapPipeline) {
          KV_CORE_ERROR("Failed to compile TonemapPipeline: %s",
                        err ? err->localizedDescription()->utf8String() : "Unknown error");
        }
      }
      if (vertFunc) vertFunc->release();
      if (fragFunc) fragFunc->release();
      
      auto skyVertStr = NS::String::string("skybox_vertex", NS::UTF8StringEncoding);
      auto skyFragStr = NS::String::string("skybox_fragment", NS::UTF8StringEncoding);
      MTL::Function* skyVert = defaultLib->newFunction(skyVertStr);
      MTL::Function* skyFrag = defaultLib->newFunction(skyFragStr);
      skyVertStr->release();
      skyFragStr->release();
      
      if (skyVert && skyFrag) {
        auto skyDesc = MTL::RenderPipelineDescriptor::alloc()->init();
        skyDesc->setVertexFunction(skyVert);
        skyDesc->setFragmentFunction(skyFrag);
        skyDesc->colorAttachments()->object(0)->setPixelFormat(MTL::PixelFormatRGBA16Float);
        skyDesc->setDepthAttachmentPixelFormat(MTL::PixelFormatDepth32Float);
        NS::Error* err = nullptr;
        s_Storage->SkyboxPipeline = device->newRenderPipelineState(skyDesc, &err);
        skyDesc->release();
        
        auto depthDesc = MTL::DepthStencilDescriptor::alloc()->init();
        depthDesc->setDepthCompareFunction(MTL::CompareFunctionLessEqual);
        depthDesc->setDepthWriteEnabled(false);
        s_Storage->SkyboxDepthState = device->newDepthStencilState(depthDesc);
        depthDesc->release();
      }
      if (skyVert) skyVert->release();
      if (skyFrag) skyFrag->release();
      defaultLib->release();
    }
    
    FramebufferSpecification hdrSpec{
      .Width = 1280, .Height = 720, .Samples = 1,
      .Format = FramebufferPixelFormat::RGBA16Float,
      .SwapChainTarget = false,
      .DepthOnly = false,
      .ClearColor = {0.06f, 0.07f, 0.09f, 1.0f}
    };
    s_Storage->HDRFramebuffer = Framebuffer::Create(hdrSpec);
    
    FramebufferSpecification shadowSpec{
      .Width = 2048, .Height = 2048, .DepthOnly = true
    };
    s_Storage->ShadowMap = Framebuffer::Create(shadowSpec);
    SetShadowMap(s_Storage->ShadowMap);
    
    auto standardShader = Shader::CreateFromDefaultLibrary("vertex_main", "fragment_main");
    
    PipelineSpecification pipelineSpec;
    pipelineSpec.ShaderProgram    = standardShader;
    pipelineSpec.UseHDRTarget     = true;
    pipelineSpec.EnableDepthWrite = true;
    pipelineSpec.EnableCulling    = true;
    pipelineSpec.Layout           = GetStandardMeshLayout();
    s_Storage->StandardPipeline   = Pipeline::Create(pipelineSpec);
    
    PipelineSpecification transparentSpec;
    transparentSpec.ShaderProgram    = standardShader;
    transparentSpec.UseHDRTarget     = true;
    transparentSpec.EnableDepthWrite = false;
    transparentSpec.EnableCulling    = false;
    transparentSpec.Layout           = GetStandardMeshLayout();
    s_Storage->TransparentPipeline   = Pipeline::Create(transparentSpec);
    
    PipelineSpecification shadowSpecPipe;
    shadowSpecPipe.ShaderProgram = Shader::CreateVertexOnly("shadow_vertex_main");
    shadowSpecPipe.Layout        = GetStandardMeshLayout();
    shadowSpecPipe.UseHDRTarget  = false;
    s_Storage->ShadowPipeline    = Pipeline::Create(shadowSpecPipe);
    
    KV_CORE_INFO("Renderer initialized successfully with Compute Bloom, ACES tonemapping, Transparency, Point Lights, and IBL.");
  }
  
  /*!
   * @brief Tears down all GPU pipelines, buffers, and framebuffers.
   */
  void Renderer::Shutdown() {
    if (!s_Storage) return;
    
    if (s_Storage->TonemapPipeline) {
      s_Storage->TonemapPipeline->release();
      s_Storage->TonemapPipeline = nullptr;
    }
    if (s_Storage->LinearSampler) {
      s_Storage->LinearSampler->release();
      s_Storage->LinearSampler = nullptr;
    }
    if (s_Storage->ShadowSampler) {
      s_Storage->ShadowSampler->release();
      s_Storage->ShadowSampler = nullptr;
    }
    if (s_Storage->SkyboxPipeline) {
      s_Storage->SkyboxPipeline->release();
      s_Storage->SkyboxPipeline = nullptr;
    }
    if (s_Storage->SkyboxDepthState) {
      s_Storage->SkyboxDepthState->release();
      s_Storage->SkyboxDepthState = nullptr;
    }
    
    for (auto* buf : s_Storage->UniformBuffers) if (buf) buf->release();
    for (auto* buf : s_Storage->ShadowUniformBuffers) if (buf) buf->release();
    for (auto* buf : s_Storage->InstanceBuffers) if (buf) buf->release();
    
    s_Storage->UniformBuffers.clear();
    s_Storage->ShadowUniformBuffers.clear();
    s_Storage->InstanceBuffers.clear();
    
    s_Storage->StandardPipeline.reset();
    s_Storage->TransparentPipeline.reset();
    s_Storage->ShadowPipeline.reset();
    s_Storage->HDRFramebuffer.reset();
    s_Storage->ShadowMap.reset();
    s_Storage->ActiveShadowMap.reset();
    s_Storage.reset();
  }
  
  BufferLayout Renderer::GetStandardMeshLayout() noexcept {
    return {
      { ShaderDataType::Float3, "position" },
      { ShaderDataType::Float3, "normal" },
      { ShaderDataType::Float4, "color" },
      { ShaderDataType::Float2, "texCoord" }
    };
  }
  
  std::shared_ptr<Pipeline> Renderer::GetStandardPipeline() noexcept {
    return s_Storage->StandardPipeline;
  }
  
  std::shared_ptr<Pipeline> Renderer::GetTransparentPipeline() noexcept {
    return s_Storage->TransparentPipeline;
  }
  
  std::shared_ptr<Pipeline> Renderer::GetShadowPipeline() noexcept {
    return s_Storage->ShadowPipeline;
  }
  
  std::shared_ptr<Framebuffer> Renderer::GetHDRFramebuffer() noexcept {
    return s_Storage->HDRFramebuffer;
  }
  
  std::shared_ptr<Framebuffer> Renderer::GetShadowMapFramebuffer() noexcept {
    return s_Storage->ShadowMap;
  }
  
  void Renderer::OnWindowResize(uint32_t width, uint32_t height) {
    if (s_Storage && s_Storage->HDRFramebuffer) {
      s_Storage->HDRFramebuffer->Resize(width, height);
    }
  }
  
  BloomPass& Renderer::GetBloomPass() noexcept {
    KV_CORE_ASSERT(s_Storage != nullptr, "[KanVerse Renderer] Renderer not initialized!");
    return s_Storage->Bloom;
  }
  
  void Renderer::SetExposure(float exposure) noexcept {
    KV_CORE_ASSERT(s_Storage != nullptr, "[KanVerse Renderer] Renderer not initialized!");
    s_Storage->Exposure = exposure;
  }
  
  float Renderer::GetExposure() noexcept {
    KV_CORE_ASSERT(s_Storage != nullptr, "[KanVerse Renderer] Renderer not initialized!");
    return s_Storage->Exposure;
  }
  
  void Renderer::SetIBLEnabled(bool enabled) noexcept {
    KV_CORE_ASSERT(s_Storage, "[KanVerse Renderer] Renderer not initialized!");
    s_Storage->EnableIBL = enabled;
    KV_CORE_INFO("Renderer: Image-Based Lighting (IBL) toggled {}", enabled ? "ON" : "OFF");
  }
  
  bool Renderer::IsIBLEnabled() noexcept {
    KV_CORE_ASSERT(s_Storage, "[KanVerse Renderer] Renderer not initialized!");
    return s_Storage->EnableIBL;
  }
  
  void Renderer::SetShadowMap(const std::shared_ptr<Framebuffer>& shadowFramebuffer) {
    KV_CORE_ASSERT(s_Storage, "[KanVerse Renderer] Renderer not initialized!");
    s_Storage->ActiveShadowMap = shadowFramebuffer;
  }
  
  void Renderer::SetEnvironmentMap(const std::shared_ptr<TextureCube>& textureCube) {
    KV_CORE_ASSERT(s_Storage, "[KanVerse Renderer] Renderer not initialized!");
    s_Storage->ActiveEnvironmentMap = textureCube;
  }
  
  void Renderer::SetIrradianceMap(const std::shared_ptr<TextureCube>& irradianceMap) {
    KV_CORE_ASSERT(s_Storage, "[KanVerse Renderer] Renderer not initialized!");
    s_Storage->ActiveIrradianceMap = irradianceMap;
  }
  
  void Renderer::SetSkyboxToIrradiance(bool useIrradiance) {
    KV_CORE_ASSERT(s_Storage, "[KanVerse Renderer] Renderer not initialized!");
    s_Storage->RenderIrradianceSkybox = useIrradiance;
  }
  
  void Renderer::SetEnvironmentMode(EnvironmentMode mode) {
    KV_CORE_ASSERT(s_Storage, "[KanVerse Renderer] Renderer not initialized!");
    s_Storage->CurrentEnvMode = mode;
    switch (mode) {
      case EnvironmentMode::CrispHDR:
        SetSkyboxToIrradiance(false);
        break;
      case EnvironmentMode::Irradiance:
        SetSkyboxToIrradiance(true);
        break;
      case EnvironmentMode::Procedural:
        SetSkyboxToIrradiance(false);
        break;
    }
    KV_CORE_INFO("Renderer: Environment mode updated successfully.");
  }
  
  EnvironmentMode Renderer::GetEnvironmentMode() noexcept {
    KV_CORE_ASSERT(s_Storage, "[KanVerse Renderer] Renderer not initialized!");
    return s_Storage->CurrentEnvMode;
  }
  
  void Renderer::SetUIRenderCallback(UIRenderCallbackFn callback) {
    KV_CORE_ASSERT(s_Storage != nullptr, "[KanVerse Renderer] Renderer not initialized!");
    s_Storage->UIRenderCallback = std::move(callback);
  }
  
  void Renderer::BeginShadowPass(const DirectionalLight& sun,
                                 const std::shared_ptr<Framebuffer>& shadowTarget) {
    KV_CORE_ASSERT(s_Storage, "[KanVerse Renderer] Renderer not initialized!");
    KV_CORE_ASSERT(shadowTarget != nullptr, "[KanVerse Renderer] Shadow render target cannot be null!");
    
    s_Storage->IsShadowPass = true;
    s_Storage->ActiveFramebuffer = shadowTarget;
    s_Storage->ActiveFrameIndex = s_Storage->Sync.BeginFrame();
    s_Storage->CurrentInstanceOffset = 0;
    
    auto& app = Application::Get();
    s_Storage->ActiveCommandBuffer = app.GetRenderContext()->GetCommandQueue()->commandBuffer();
    s_Storage->Sync.AttachToCommandBuffer(s_Storage->ActiveCommandBuffer);
    
    ShadowUniforms shadowUniforms{};
    shadowUniforms.LightViewProjectionMatrix = sun.GetViewProjectionMatrix();
    MTL::Buffer* shadowBuf = s_Storage->ShadowUniformBuffers[s_Storage->ActiveFrameIndex];
    std::memcpy(shadowBuf->contents(), &shadowUniforms, sizeof(ShadowUniforms));
    
    s_Storage->ActiveCommandEncoder = s_Storage->ActiveCommandBuffer->renderCommandEncoder(
                                                                                           shadowTarget->GetRenderPassDescriptor()
                                                                                           );
    if (s_Storage->ActiveCommandEncoder) {
      s_Storage->ActiveCommandEncoder->setVertexBuffer(shadowBuf, 0, 1);
    }
  }
  
  /*!
   * @brief Begins the primary scene forward pass and uploads global lighting & point light uniforms.
   * @param camera Active perspective camera.
   * @param sun Primary directional sun light.
   * @param env Ambient environment lighting settings.
   * @param target Optional HDR framebuffer target.
   * @param pointLights Span of active GPU point lights for indoor/local illumination.
   */
  void Renderer::BeginScene(const PerspectiveCamera& camera,
                            const DirectionalLight& sun,
                            const EnvironmentLighting& env,
                            const std::shared_ptr<Framebuffer>& target,
                            std::span<const GPUPointLight> pointLights) {
    KV_CORE_ASSERT(s_Storage, "[KanVerse Renderer] Renderer not initialized!");
    s_Storage->IsShadowPass                     = false;
    s_Storage->ActiveScene.ViewProjectionMatrix = camera.GetViewProjectionMatrix();
    s_Storage->ActiveScene.SunLight             = sun;
    s_Storage->ActiveScene.Environment          = env;
    s_Storage->ActiveFramebuffer                = target;
    
    if (!s_Storage->ActiveCommandBuffer) {
      s_Storage->ActiveFrameIndex = s_Storage->Sync.BeginFrame();
      s_Storage->CurrentInstanceOffset = 0;
      auto& app = Application::Get();
      s_Storage->ActiveCommandBuffer = app.GetRenderContext()->GetCommandQueue()->commandBuffer();
      s_Storage->Sync.AttachToCommandBuffer(s_Storage->ActiveCommandBuffer);
    }
    
    const auto& sunLight = s_Storage->ActiveScene.SunLight;
    const Math::Vec4 directionalColor = Math::MakeVec4(
                                                       sunLight.GetColor().x * sunLight.GetIntensity(),
                                                       sunLight.GetColor().y * sunLight.GetIntensity(),
                                                       sunLight.GetColor().z * sunLight.GetIntensity(),
                                                       1.0f
                                                       );
    
    GPUUniforms uniforms = {};
    uniforms.ViewProjectionMatrix = camera.GetViewProjectionMatrix();
    uniforms.LightSpaceMatrix     = sunLight.GetViewProjectionMatrix();
    uniforms.CameraPosition       = camera.GetPosition();
    uniforms.LightDirection       = sunLight.GetDirection();
    uniforms.DirectionalColor     = directionalColor;
    uniforms.AmbientColor         = env.GetEffectiveAmbient();
    
    const uint32_t numPointLights = static_cast<uint32_t>(
                                                          std::min<size_t>(pointLights.size(), kMaxPointLights)
                                                          );
    uniforms.PointLightCount = numPointLights;
    if (numPointLights > 0) {
      std::memcpy(uniforms.PointLights, pointLights.data(), numPointLights * sizeof(GPUPointLight));
    }
    
    MTL::Buffer* uniformBuffer = s_Storage->UniformBuffers[s_Storage->ActiveFrameIndex];
    std::memcpy(uniformBuffer->contents(), &uniforms, sizeof(GPUUniforms));
    
    if (target) {
      s_Storage->ActiveCommandEncoder = s_Storage->ActiveCommandBuffer->renderCommandEncoder(
                                                                                             target->GetRenderPassDescriptor()
                                                                                             );
    } else {
      auto& app = Application::Get();
      auto* nativeWin = static_cast<const NativeWindow*>(&app.GetWindow());
      MTKView* metalView = (__bridge MTKView*)nativeWin->GetNativeView();
      MTL::RenderPassDescriptor* passDesc = (__bridge MTL::RenderPassDescriptor*)metalView.currentRenderPassDescriptor;
      s_Storage->ActiveCommandEncoder = passDesc ? s_Storage->ActiveCommandBuffer->renderCommandEncoder(passDesc) : nullptr;
    }
    
    if (s_Storage->ActiveCommandEncoder) {
      if (!s_Storage->IsShadowPass && s_Storage->SkyboxPipeline) {
        std::shared_ptr<TextureCube> skyboxTex = nullptr;
        if (s_Storage->EnableIBL) {
          if (s_Storage->CurrentEnvMode == EnvironmentMode::Procedural) {
            skyboxTex = s_Storage->ActiveProceduralMap;
          } else {
            skyboxTex = s_Storage->RenderIrradianceSkybox ? s_Storage->ActiveIrradianceMap : s_Storage->ActiveEnvironmentMap;
          }
        }
        if (skyboxTex && s_Storage->LinearSampler) {
          s_Storage->ActiveCommandEncoder->setRenderPipelineState(s_Storage->SkyboxPipeline);
          s_Storage->ActiveCommandEncoder->setDepthStencilState(s_Storage->SkyboxDepthState);
          struct SkyboxUniforms {
            Math::Mat4 InverseView;
            Math::Mat4 InverseProjection;
          };
          SkyboxUniforms skyUbo{
            Math::Inverse(camera.GetViewMatrix()),
            Math::Inverse(camera.GetProjection())
          };
          s_Storage->ActiveCommandEncoder->setVertexBytes(&skyUbo, sizeof(SkyboxUniforms), 0);
          skyboxTex->Bind(s_Storage->ActiveCommandEncoder, 0, 0);
          s_Storage->ActiveCommandEncoder->drawPrimitives(MTL::PrimitiveTypeTriangle, (NS::UInteger)0, (NS::UInteger)3);
        }
      }
      
      s_Storage->ActiveCommandEncoder->setVertexBuffer(uniformBuffer, 0, 1);
      s_Storage->ActiveCommandEncoder->setFragmentBuffer(uniformBuffer, 0, 1);
      
      if (s_Storage->ActiveShadowMap && s_Storage->ShadowSampler) {
        s_Storage->ActiveCommandEncoder->setFragmentTexture(s_Storage->ActiveShadowMap->GetDepthAttachment(), 6);
        s_Storage->ActiveCommandEncoder->setFragmentSamplerState(s_Storage->ShadowSampler, 1);
      }
      
      if (s_Storage->EnableIBL) {
        std::shared_ptr<TextureCube> iblMap = s_Storage->ActiveEnvironmentMap;
        if (s_Storage->CurrentEnvMode == EnvironmentMode::Procedural) {
          iblMap = s_Storage->ActiveProceduralMap;
        }
        if (iblMap && s_Storage->LinearSampler) {
          iblMap->Bind(s_Storage->ActiveCommandEncoder, 7, 2);
          iblMap->Bind(s_Storage->ActiveCommandEncoder, 8, 2);
        } else {
          if (s_Storage->ActiveEnvironmentMap && s_Storage->LinearSampler) {
            s_Storage->ActiveEnvironmentMap->Bind(s_Storage->ActiveCommandEncoder, 7, 2);
          }
          if (s_Storage->ActiveIrradianceMap && s_Storage->LinearSampler) {
            s_Storage->ActiveIrradianceMap->Bind(s_Storage->ActiveCommandEncoder, 8, 2);
          }
        }
      }
    }
  }
  
  void Renderer::SubmitInstanced(const std::shared_ptr<Pipeline>& pipeline,
                                 const std::shared_ptr<Material>& material,
                                 const std::shared_ptr<Mesh>& mesh,
                                 std::span<const InstanceData> instances) {
    KV_CORE_ASSERT(s_Storage, "[KanVerse Renderer] Renderer not initialized!");
    if (!s_Storage->ActiveCommandEncoder || instances.empty()) return;
    KV_CORE_ASSERT(pipeline != nullptr, "[KanVerse Renderer] Pipeline is null!");
    KV_CORE_ASSERT(mesh != nullptr,     "[KanVerse Renderer] Mesh is null!");
    
    const size_t count = instances.size();
    const size_t bytesRequired = count * sizeof(InstanceData);
    KV_CORE_ASSERT(s_Storage->CurrentInstanceOffset + bytesRequired <= RendererStorage::InstanceBufferSize,
                   "[KanVerse Renderer] Exceeded frame instancing buffer capacity!");
    
    MTL::Buffer* currentInstanceBuffer = s_Storage->InstanceBuffers[s_Storage->ActiveFrameIndex];
    const size_t bufferOffset = s_Storage->CurrentInstanceOffset;
    uint8_t* dest = static_cast<uint8_t*>(currentInstanceBuffer->contents()) + bufferOffset;
    std::memcpy(dest, instances.data(), bytesRequired);
    s_Storage->CurrentInstanceOffset += bytesRequired;
    
    if (!s_Storage->IsShadowPass && material) {
      material->Sync(s_Storage->ActiveFrameIndex);
      s_Storage->ActiveCommandEncoder->setFragmentBuffer(material->GetUniformBuffer(s_Storage->ActiveFrameIndex), 0, 2);
      
      if (material->GetAlbedoTexture()) {
        material->GetAlbedoTexture()->Bind(s_Storage->ActiveCommandEncoder, 0, 0);
      } else if (s_Storage->DefaultWhiteTexture) {
        s_Storage->DefaultWhiteTexture->Bind(s_Storage->ActiveCommandEncoder, 0, 0);
      }
      
      if (material->GetNormalTexture()) {
        material->GetNormalTexture()->Bind(s_Storage->ActiveCommandEncoder, 1, 0);
      } else if (s_Storage->DefaultWhiteTexture) {
        s_Storage->DefaultWhiteTexture->Bind(s_Storage->ActiveCommandEncoder, 1, 0);
      }
      
      if (material->GetRoughnessTexture()) {
        material->GetRoughnessTexture()->Bind(s_Storage->ActiveCommandEncoder, 2, 0);
      } else if (s_Storage->DefaultWhiteTexture) {
        s_Storage->DefaultWhiteTexture->Bind(s_Storage->ActiveCommandEncoder, 2, 0);
      }
      
      if (material->GetMetallicTexture()) {
        material->GetMetallicTexture()->Bind(s_Storage->ActiveCommandEncoder, 3, 0);
      } else if (s_Storage->DefaultWhiteTexture) {
        s_Storage->DefaultWhiteTexture->Bind(s_Storage->ActiveCommandEncoder, 3, 0);
      }
      
      if (material->GetAOTexture()) {
        material->GetAOTexture()->Bind(s_Storage->ActiveCommandEncoder, 4, 0);
      } else if (s_Storage->DefaultWhiteTexture) {
        s_Storage->DefaultWhiteTexture->Bind(s_Storage->ActiveCommandEncoder, 4, 0);
      }
      
      if (material->GetHeightTexture()) {
        material->GetHeightTexture()->Bind(s_Storage->ActiveCommandEncoder, 5, 0);
      } else if (s_Storage->DefaultWhiteTexture) {
        s_Storage->DefaultWhiteTexture->Bind(s_Storage->ActiveCommandEncoder, 5, 0);
      }
    }
    
    s_Storage->ActiveCommandEncoder->setVertexBuffer(currentInstanceBuffer, bufferOffset, 3);
    for (const auto& submesh : mesh->GetSubmeshes()) {
      (void)submesh;
      RenderCommand::DrawIndexedInstanced(
                                          s_Storage->ActiveCommandEncoder,
                                          pipeline,
                                          mesh->GetVertexBuffer(),
                                          mesh->GetIndexBuffer(),
                                          static_cast<uint32_t>(count)
                                          );
    }
  }
  
  void Renderer::EndScene() {
    KV_CORE_ASSERT(s_Storage, "[KanVerse Renderer] Renderer not initialized!");
    if (s_Storage->ActiveCommandEncoder) {
      s_Storage->ActiveCommandEncoder->endEncoding();
      s_Storage->ActiveCommandEncoder = nullptr;
    }
    
    if (s_Storage->IsShadowPass) {
      s_Storage->ActiveFramebuffer = nullptr;
      return;
    }
    
    auto& app = Application::Get();
    auto* nativeWin = static_cast<const NativeWindow*>(&app.GetWindow());
    MTKView* metalView = (__bridge MTKView*)nativeWin->GetNativeView();
    
    if (s_Storage->ActiveCommandBuffer) {
      id<CAMetalDrawable> currentDrawable = nil;
      if (metalView && metalView.bounds.size.width > 1.0 && metalView.bounds.size.height > 1.0) {
        currentDrawable = metalView.currentDrawable;
      }
      
      if (s_Storage->ActiveFramebuffer && currentDrawable && s_Storage->TonemapPipeline) {
        MTL::Texture* hdrColor = s_Storage->ActiveFramebuffer->GetColorAttachment(0);
        if (hdrColor) {
          s_Storage->Bloom.Resize(static_cast<uint32_t>(hdrColor->width()),
                                  static_cast<uint32_t>(hdrColor->height()));
          s_Storage->Bloom.Execute(s_Storage->ActiveCommandBuffer, hdrColor);
        }
        
        MTL::RenderPassDescriptor* displayPassDesc = (__bridge MTL::RenderPassDescriptor*)metalView.currentRenderPassDescriptor;
        if (displayPassDesc) {
          MTL::RenderCommandEncoder* tonemapEncoder = s_Storage->ActiveCommandBuffer->renderCommandEncoder(displayPassDesc);
          if (tonemapEncoder) {
            tonemapEncoder->setRenderPipelineState(s_Storage->TonemapPipeline);
            tonemapEncoder->setFragmentTexture(hdrColor, 0);
            tonemapEncoder->setFragmentTexture(s_Storage->Bloom.GetBloomTexture(), 1);
            tonemapEncoder->setFragmentSamplerState(s_Storage->LinearSampler, 0);
            
            TonemapUniforms tonemapUniforms;
            tonemapUniforms.BloomIntensity = s_Storage->Bloom.GetIntensity();
            tonemapUniforms.Exposure       = s_Storage->Exposure;
            tonemapEncoder->setFragmentBytes(&tonemapUniforms, sizeof(TonemapUniforms), 0);
            tonemapEncoder->drawPrimitives(MTL::PrimitiveTypeTriangle, (NS::UInteger)0, (NS::UInteger)3);
            
            // Inside Renderer::EndScene() right after drawing the tonemap triangle, BEFORE tonemapEncoder->endEncoding():
            if (s_Storage->UIRenderCallback) {
              s_Storage->UIRenderCallback(s_Storage->ActiveCommandBuffer, tonemapEncoder, displayPassDesc);
            }
            tonemapEncoder->endEncoding();
          }
        }
      }
      
      if (currentDrawable) {
        s_Storage->ActiveCommandBuffer->presentDrawable((__bridge CA::MetalDrawable*)currentDrawable);
      }
      s_Storage->ActiveCommandBuffer->commit();
      s_Storage->ActiveCommandBuffer = nullptr;
    }
    s_Storage->ActiveFramebuffer = nullptr;
  }
  
} // namespace KanVerse
