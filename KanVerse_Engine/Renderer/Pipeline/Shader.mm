/*!
 * @file Shader.mm
 * @brief Implementation of Apple Metal shading programs and function compilation.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#include "Renderer/Pipeline/Shader.hpp"
#include "Core/Diagnostics/Assert.hpp"
#include "Core/Application/Application.hpp"

#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>

#include <cassert>
#include <iostream>
#include <string>

namespace KanVerse {
  
  class MetalShader : public Shader {
  public:
    MetalShader(std::string_view name,
                std::string_view source,
                std::string_view vertexEntry,
                std::string_view fragmentEntry)
    : m_Name(name) {
      MTL::Device* device = Application::Get().GetRenderContext()->GetDevice();
      KV_CORE_ASSERT(device != nullptr, "[KanVerse] Active Metal device is null!");
      
      auto sourceStr = NS::String::string(std::string(source).c_str(), NS::UTF8StringEncoding);
      NS::Error* error = nullptr;
      
      MTL::CompileOptions* options = MTL::CompileOptions::alloc()->init();
      options->setFastMathEnabled(true);
      
      MTL::Library* library = device->newLibrary(sourceStr, options, &error);
      options->release();
      sourceStr->release();
      
      if (!library) {
        if (error) {
          std::cerr << "[KanVerse] MSL Compilation Error: "
          << error->localizedDescription()->utf8String() << std::endl;
        }
        KV_CORE_ASSERT(false, "[KanVerse] Failed to compile MSL shader source!");
      }
      
      auto vertName = NS::String::string(std::string(vertexEntry).c_str(), NS::UTF8StringEncoding);
      m_VertexFunction = library->newFunction(vertName);
      vertName->release();
      KV_CORE_ASSERT(m_VertexFunction != nullptr, "[KanVerse] Vertex entry point symbol not found in library!");
      
      if (!fragmentEntry.empty()) {
        auto fragName = NS::String::string(std::string(fragmentEntry).c_str(), NS::UTF8StringEncoding);
        m_FragmentFunction = library->newFunction(fragName);
        fragName->release();
        KV_CORE_ASSERT(m_FragmentFunction != nullptr, "[KanVerse] Fragment entry point symbol not found in library!");
      }
      
      library->release();
    }
    
    MetalShader(std::string_view name,
                MTL::Library* library,
                std::string_view vertexEntry,
                std::string_view fragmentEntry)
    : m_Name(name) {
      KV_CORE_ASSERT(library != nullptr, "[KanVerse] Metal library handle is null!");
      
      auto vertName = NS::String::string(std::string(vertexEntry).c_str(), NS::UTF8StringEncoding);
      m_VertexFunction = library->newFunction(vertName);
      vertName->release();
      KV_CORE_ASSERT(m_VertexFunction != nullptr, "[KanVerse] Vertex entry point symbol not found in library!");
      
      if (!fragmentEntry.empty()) {
        auto fragName = NS::String::string(std::string(fragmentEntry).c_str(), NS::UTF8StringEncoding);
        m_FragmentFunction = library->newFunction(fragName);
        fragName->release();
        KV_CORE_ASSERT(m_FragmentFunction != nullptr, "[KanVerse] Fragment entry point symbol not found in library!");
      }
    }
    
    ~MetalShader() override {
      if (m_VertexFunction) {
        m_VertexFunction->release();
        m_VertexFunction = nullptr;
      }
      if (m_FragmentFunction) {
        m_FragmentFunction->release();
        m_FragmentFunction = nullptr;
      }
    }
    
    [[nodiscard]] const std::string& GetName() const noexcept override {
      return m_Name;
    }
    
    [[nodiscard]] MTL::Function* GetVertexFunction() const noexcept override {
      return m_VertexFunction;
    }
    
    [[nodiscard]] MTL::Function* GetFragmentFunction() const noexcept override {
      return m_FragmentFunction;
    }
    
  private:
    std::string    m_Name;
    MTL::Function* m_VertexFunction{nullptr};
    MTL::Function* m_FragmentFunction{nullptr};
  };
  
  std::shared_ptr<Shader> Shader::CreateFromSource(std::string_view name,
                                                   std::string_view source,
                                                   std::string_view vertexEntry,
                                                   std::string_view fragmentEntry) {
    return std::make_shared<MetalShader>(name, source, vertexEntry, fragmentEntry);
  }
  
  std::shared_ptr<Shader> Shader::CreateFromDefaultLibrary(const std::string& vertexName,
                                                           const std::string& fragmentName) {
    MTL::Device* device = Application::Get().GetRenderContext()->GetDevice();
    KV_CORE_ASSERT(device != nullptr, "[KanVerse] Active Metal device is null!");
    
    MTL::Library* defaultLib = device->newDefaultLibrary();
    KV_CORE_ASSERT(defaultLib != nullptr, "[KanVerse] Failed to load default.metallib from application bundle!");
    
    auto shader = std::make_shared<MetalShader>("DefaultBundleShader", defaultLib, vertexName, fragmentName);
    defaultLib->release();
    return shader;
  }
  
  std::shared_ptr<Shader> Shader::CreateVertexOnly(std::string_view vertexName) {
    MTL::Device* device = Application::Get().GetRenderContext()->GetDevice();
    KV_CORE_ASSERT(device != nullptr, "[KanVerse] Active Metal device is null!");
    
    MTL::Library* defaultLib = device->newDefaultLibrary();
    KV_CORE_ASSERT(defaultLib != nullptr, "[KanVerse] Failed to load default.metallib from bundle!");
    
    auto shader = std::make_shared<MetalShader>(vertexName, defaultLib, vertexName, "");
    defaultLib->release();
    return shader;
  }
  
} // namespace KanVerse
