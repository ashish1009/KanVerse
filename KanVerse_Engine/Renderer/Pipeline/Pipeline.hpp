/*!
 * @file Pipeline.hpp
 * @brief PipelineState interface coordinating shaders, vertex layouts, and raster states.
 * @details Configures hardware rasterizer state, depth comparisons, backface culling,
 *          and target render surface pixel formats.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#pragma once

#include "Renderer/Buffers/BufferLayout.hpp"
#include "Renderer/Pipeline/Shader.hpp"
#include <memory>

namespace MTL {
  class RenderPipelineState;
  class DepthStencilState;
  class RenderCommandEncoder;
}

namespace KanVerse {
  
  /*!
   * @struct PipelineSpecification
   * @brief Configuration specifications for building a graphics pipeline state.
   */
  struct PipelineSpecification {
    std::shared_ptr<Shader> ShaderProgram{nullptr}; ///< Linked vertex and fragment shaders.
    BufferLayout            Layout{};               ///< Interleaved vertex stream layout schema.
    bool                    EnableDepthWrite{true}; ///< Depth buffer write enable flag.
    bool                    EnableCulling{true};    ///< Back-face culling toggle.
    bool                    Wireframe{false};       ///< Wireframe polygon rasterization toggle.
    float                   DepthBiasConstant{0.0f};   ///< Constant bias offset for shadow mapping.
    float                   DepthBiasSlopeScale{0.0f}; ///< Slope-scaled bias offset.
    float                   DepthBiasClamp{0.0f};      ///< Maximum bias clamp.
    bool                    UseHDRTarget{true};     ///< Configures color attachment for MTL::PixelFormatRGBA16Float.
  };
  
  /*!
   * @class Pipeline
   * @brief Encapsulates pipeline configuration, translated vertex layouts, and depth states.
   */
  class Pipeline {
  public:
    virtual ~Pipeline() = default;
    
    /*!
     * @brief Binds this pipeline state and its depth/stencil configuration to an active render encoder.
     * @param encoder Active Metal command encoder.
     */
    virtual void Bind(MTL::RenderCommandEncoder* encoder) const = 0;
    
    /*!
     * @brief Queries configuration specification.
     */
    [[nodiscard]] virtual const PipelineSpecification& GetSpecification() const noexcept = 0;
    
    /*!
     * @brief Accesses the native underlying Metal pipeline state.
     */
    [[nodiscard]] virtual MTL::RenderPipelineState* GetNativePipelineState() const noexcept = 0;
    
    /*!
     * @brief Accesses the native underlying Metal depth stencil state.
     */
    [[nodiscard]] virtual MTL::DepthStencilState* GetNativeDepthState() const noexcept = 0;
    
    /*!
     * @brief Factory creating a compiled graphics Pipeline.
     * @param spec Configuration specification.
     * @return Unique pointer owning the Pipeline.
     */
    [[nodiscard]] static std::unique_ptr<Pipeline> Create(const PipelineSpecification& spec);
  };
  
} // namespace KanVerse
