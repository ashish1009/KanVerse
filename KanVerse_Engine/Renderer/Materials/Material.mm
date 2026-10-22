/*!
 * @file Material.mm
 * @brief Metal implementation of the PBR Material class.
 * @copyright KanVerse Engine
 */
#include "Renderer/Materials/Material.hpp"
#include "Core/Diagnostics/Assert.hpp"
#include "Core/Application/Application.hpp"
#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <cassert>
#include <cstring>

namespace KanVerse {
  
  Material::Material(std::shared_ptr<Shader> shader, std::string name)
  : m_Name(std::move(name)), m_Shader(std::move(shader)) {
    KV_CORE_ASSERT(m_Shader != nullptr, "[KanVerse] Material must be bound to a valid Shader!");
    MTL::Device* device = Application::Get().GetRenderContext()->GetDevice();
    KV_CORE_ASSERT(device != nullptr, "[KanVerse] Active Metal device is null!");
    
    for (uint32_t i = 0; i < kMaxFramesInFlight; ++i) {
      m_UniformBuffers[i] = device->newBuffer(sizeof(MaterialUniforms), MTL::ResourceStorageModeShared);
      KV_CORE_ASSERT(m_UniformBuffers[i] != nullptr, "[KanVerse] Failed to allocate Material uniform buffer!");
    }
    
    for (uint32_t i = 0; i < kMaxFramesInFlight; ++i) {
      Sync(i);
    }
  }
  
  Material::~Material() {
    for (uint32_t i = 0; i < kMaxFramesInFlight; ++i) {
      if (m_UniformBuffers[i]) {
        m_UniformBuffers[i]->release();
        m_UniformBuffers[i] = nullptr;
      }
    }
  }
  
  void Material::SetBaseColor(const Math::Vec4& color) noexcept {
    m_Uniforms.BaseColor = color;
    m_IsTransparent = (color.w < 0.99f);
    m_Dirty = true;
  }
  
  void Material::SetRoughness(float roughness) noexcept {
    m_Uniforms.Roughness = roughness;
    m_Dirty = true;
  }
  
  void Material::SetMetallic(float metallic) noexcept {
    m_Uniforms.Metallic = metallic;
    m_Dirty = true;
  }
  
  void Material::SetEmissiveColor(const Math::Vec4& emissive) noexcept {
    m_Uniforms.EmissiveColor = emissive;
    m_Dirty = true;
  }
  
  void Material::SetAlbedoTexture(std::shared_ptr<Texture2D> texture) noexcept {
    m_AlbedoTexture = std::move(texture);
    m_Uniforms.HasAlbedoMap = (m_AlbedoTexture != nullptr) ? 1 : 0;
    m_Dirty = true;
  }
  
  void Material::SetNormalTexture(std::shared_ptr<Texture2D> texture) noexcept {
    m_NormalTexture = std::move(texture);
    m_Uniforms.HasNormalMap = (m_NormalTexture != nullptr) ? 1 : 0;
    m_Dirty = true;
  }
  
  void Material::SetRoughnessTexture(std::shared_ptr<Texture2D> texture) noexcept {
    m_RoughnessTexture = std::move(texture);
    m_Uniforms.HasRoughnessMap = (m_RoughnessTexture != nullptr) ? 1 : 0;
    m_Dirty = true;
  }
  
  void Material::SetMetallicTexture(std::shared_ptr<Texture2D> texture) noexcept {
    m_MetallicTexture = std::move(texture);
    m_Uniforms.HasMetallicMap = (m_MetallicTexture != nullptr) ? 1 : 0;
    m_Dirty = true;
  }
  
  void Material::SetAOTexture(std::shared_ptr<Texture2D> texture) noexcept {
    m_AOTexture = std::move(texture);
    m_Uniforms.HasAOMap = (m_AOTexture != nullptr) ? 1 : 0;
    m_Dirty = true;
  }
  
  void Material::SetHeightTexture(std::shared_ptr<Texture2D> texture) noexcept {
    m_HeightTexture = std::move(texture);
    m_Uniforms.HasHeightMap = (m_HeightTexture != nullptr) ? 1 : 0;
    m_Dirty = true;
  }
  
  void Material::SetTilingFactor(const Math::Vec3& tiling) noexcept {
    m_Uniforms.TilingFactor = tiling;
    m_Dirty = true;
  }
  
  void Material::Sync(uint32_t frameIndex) {
    KV_CORE_ASSERT(frameIndex < kMaxFramesInFlight, "[KanVerse] Frame index out of flight range!");
    if (!m_UniformBuffers[frameIndex]) return;
    uint8_t* dest = static_cast<uint8_t*>(m_UniformBuffers[frameIndex]->contents());
    std::memcpy(dest, &m_Uniforms, sizeof(MaterialUniforms));
  }
  
  MTL::Buffer* Material::GetUniformBuffer(uint32_t frameIndex) const noexcept {
    KV_CORE_ASSERT(frameIndex < kMaxFramesInFlight, "[KanVerse] Frame index out of flight range!");
    return m_UniformBuffers[frameIndex];
  }
  
  std::shared_ptr<Material> Material::Create(std::shared_ptr<Shader> shader, std::string name) {
    return std::make_shared<Material>(std::move(shader), std::move(name));
  }
  
} // namespace KanVerse
