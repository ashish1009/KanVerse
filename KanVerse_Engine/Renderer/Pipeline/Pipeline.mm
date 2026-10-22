/*!
 * @file Pipeline.mm
 * @brief Apple Metal implementation of graphics Pipeline and depth pre-pass translation.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#include "Renderer/Pipeline/Pipeline.hpp"
#include "Core/Diagnostics/Log.hpp"
#include "Core/Diagnostics/Assert.hpp"
#include "Core/Application/Application.hpp"

#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#import <Foundation/Foundation.h>
#include <cassert>

namespace KanVerse {
  
  static MTL::VertexFormat ShaderDataTypeToMetalFormat(ShaderDataType type) {
    switch (type) {
      case ShaderDataType::Float:   return MTL::VertexFormatFloat;
      case ShaderDataType::Float2:  return MTL::VertexFormatFloat2;
      case ShaderDataType::Float3:  return MTL::VertexFormatFloat3;
      case ShaderDataType::Float4:  return MTL::VertexFormatFloat4;
      case ShaderDataType::Int:     return MTL::VertexFormatInt;
      case ShaderDataType::Int2:    return MTL::VertexFormatInt2;
      case ShaderDataType::Int3:    return MTL::VertexFormatInt3;
      case ShaderDataType::Int4:    return MTL::VertexFormatInt4;
      default: break;
    }
    KV_CORE_ASSERT(false, "Unsupported ShaderDataType for vertex layout!");
    return MTL::VertexFormatInvalid;
  }
  
  class MetalPipeline : public Pipeline {
  public:
    explicit MetalPipeline(const PipelineSpecification& spec)
    : m_Spec(spec) {
      Invalidate();
    }
    
    ~MetalPipeline() override {
      if (m_DepthState) {
        m_DepthState->release();
        m_DepthState = nullptr;
      }
      if (m_PipelineState) {
        m_PipelineState->release();
        m_PipelineState = nullptr;
      }
    }
    
    void Bind(MTL::RenderCommandEncoder* encoder) const override {
      encoder->setRenderPipelineState(m_PipelineState);
      encoder->setDepthStencilState(m_DepthState);
      encoder->setCullMode(m_Spec.EnableCulling ? MTL::CullModeBack : MTL::CullModeNone);
      encoder->setFrontFacingWinding(MTL::WindingCounterClockwise);
      
      if (m_Spec.DepthBiasSlopeScale > 0.0f || m_Spec.DepthBiasConstant > 0.0f) {
        encoder->setDepthBias(m_Spec.DepthBiasConstant, m_Spec.DepthBiasSlopeScale, m_Spec.DepthBiasClamp);
      }
      
      if (m_Spec.Wireframe) {
        encoder->setTriangleFillMode(MTL::TriangleFillModeLines);
      } else {
        encoder->setTriangleFillMode(MTL::TriangleFillModeFill);
      }
    }
    
    [[nodiscard]] const PipelineSpecification& GetSpecification() const noexcept override {
      return m_Spec;
    }
    
    [[nodiscard]] MTL::RenderPipelineState* GetNativePipelineState() const noexcept override {
      return m_PipelineState;
    }
    
    [[nodiscard]] MTL::DepthStencilState* GetNativeDepthState() const noexcept override {
      return m_DepthState;
    }
    
  private:
    void Invalidate() {
      MTL::Device* device = Application::Get().GetRenderContext()->GetDevice();
      KV_CORE_ASSERT(device != nullptr, "[KanVerse] Active Metal device is null!");
      KV_CORE_ASSERT(m_Spec.ShaderProgram != nullptr, "[KanVerse] Pipeline requires a valid ShaderProgram!");
      
      // 1. Build MTL::VertexDescriptor
      auto vertexDescriptor = MTL::VertexDescriptor::vertexDescriptor();
      uint32_t attributeIndex = 0;
      
      for (const auto& element : m_Spec.Layout) {
        auto attr = vertexDescriptor->attributes()->object(attributeIndex);
        attr->setFormat(ShaderDataTypeToMetalFormat(element.Type));
        attr->setOffset(element.Offset);
        attr->setBufferIndex(0);
        ++attributeIndex;
      }
      
      const uint32_t rawStride = m_Spec.Layout.GetStride();
      const uint32_t alignedStride = (rawStride + 15) & ~15;
      
      auto layoutDesc = vertexDescriptor->layouts()->object(0);
      layoutDesc->setStride(alignedStride);
      layoutDesc->setStepFunction(MTL::VertexStepFunctionPerVertex);
      
      // 2. Build MTL::RenderPipelineDescriptor
      // Inside KanVerse_Engine/Renderer/Pipeline/Pipeline.mm (in MetalPipeline::Invalidate)
      auto pipelineDescriptor = MTL::RenderPipelineDescriptor::alloc()->init();
      pipelineDescriptor->setVertexFunction(m_Spec.ShaderProgram->GetVertexFunction());
      if (m_Spec.ShaderProgram->GetFragmentFunction()) {
        pipelineDescriptor->setFragmentFunction(m_Spec.ShaderProgram->GetFragmentFunction());
        const MTL::PixelFormat targetPixelFormat = m_Spec.UseHDRTarget
        ? MTL::PixelFormatRGBA16Float
        : MTL::PixelFormatBGRA8Unorm_sRGB;
        
        auto colorAttach = pipelineDescriptor->colorAttachments()->object(0);
        colorAttach->setPixelFormat(targetPixelFormat);
        
        // ---> ADD THESE LINES TO ENABLE ALPHA BLENDING <---
        colorAttach->setBlendingEnabled(true);
        colorAttach->setSourceRGBBlendFactor(MTL::BlendFactorSourceAlpha);
        colorAttach->setDestinationRGBBlendFactor(MTL::BlendFactorOneMinusSourceAlpha);
        colorAttach->setSourceAlphaBlendFactor(MTL::BlendFactorSourceAlpha);
        colorAttach->setDestinationAlphaBlendFactor(MTL::BlendFactorOneMinusSourceAlpha);
      } else {
        pipelineDescriptor->setFragmentFunction(nullptr);
      }
      
      pipelineDescriptor->setVertexDescriptor(vertexDescriptor);
      pipelineDescriptor->setDepthAttachmentPixelFormat(MTL::PixelFormatDepth32Float);
      
      NS::Error* error = nullptr;
      m_PipelineState = device->newRenderPipelineState(pipelineDescriptor, &error);
      pipelineDescriptor->release();
      
      if (!m_PipelineState) {
        KV_CORE_ERROR("Pipeline compilation error: %s",
                      error ? error->localizedDescription()->utf8String() : "Unknown error");
        KV_CORE_ASSERT(false, "Failed to create MTL::RenderPipelineState!");
        return;
      }
      
      // 3. Build DepthStencilState
      auto depthDescriptor = MTL::DepthStencilDescriptor::alloc()->init();
      depthDescriptor->setDepthCompareFunction(MTL::CompareFunctionLessEqual);
      depthDescriptor->setDepthWriteEnabled(m_Spec.EnableDepthWrite);
      m_DepthState = device->newDepthStencilState(depthDescriptor);
      depthDescriptor->release();
    }
    
    PipelineSpecification     m_Spec;
    MTL::RenderPipelineState* m_PipelineState{nullptr};
    MTL::DepthStencilState*   m_DepthState{nullptr};
  };
  
  std::unique_ptr<Pipeline> Pipeline::Create(const PipelineSpecification& spec) {
    return std::make_unique<MetalPipeline>(spec);
  }
  
} // namespace KanVerse
