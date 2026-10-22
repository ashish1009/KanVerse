/*!
 * @file Material.hpp
 * @brief PBR Material abstraction managing shader uniforms, transparency state, and texture bindings.
 * @copyright KanVerse Engine
 */

#pragma once
#include "Core/Math/Math.hpp"
#include "Renderer/Pipeline/Shader.hpp"
#include "Renderer/Textures/Texture2D.hpp"
#include <memory>
#include <string>

namespace MTL {
  class Buffer;
}

namespace KanVerse {
  
  /*!
   * @brief 256-byte aligned uniform block uploaded per material to fragment shader buffer(2).
   */
  struct alignas(256) MaterialUniforms {
    Math::Vec4 BaseColor{1.0f, 1.0f, 1.0f, 1.0f};     ///< Albedo tint color (RGBA).
    Math::Vec4 SpecularColor{1.0f, 1.0f, 1.0f, 1.0f}; ///< Specular reflectance tint.
    Math::Vec4 EmissiveColor{0.0f, 0.0f, 0.0f, 0.0f}; ///< HDR emissive radiance color.
    Math::Vec3 TilingFactor{1.0f, 1.0f, 1.0f};        ///< Triplanar/UV tiling multiplier.
    float      Roughness{0.4f};                       ///< Microfacet surface roughness [0.04, 1.0].
    float      Metallic{0.0f};                        ///< Metalness factor [0.0, 1.0].
    int32_t    HasAlbedoMap{0};                       ///< Non-zero if Albedo texture is bound.
    int32_t    HasNormalMap{0};                       ///< Non-zero if Normal texture is bound.
    int32_t    HasRoughnessMap{0};                    ///< Non-zero if Roughness texture is bound.
    int32_t    HasMetallicMap{0};                     ///< Non-zero if Metallic texture is bound.
    int32_t    HasAOMap{0};                           ///< Non-zero if Ambient Occlusion texture is bound.
    int32_t    HasHeightMap{0};                       ///< Non-zero if Height texture is bound.
    float      Padding[1]{0.0f};                      ///< Alignment padding.
  };
  
  /*!
   * @brief Encapsulates PBR surface properties, textures, transparency flags, and triple-buffered Metal uniform buffers.
   */
  class Material {
  public:
    /*!
     * @brief Constructs a Material bound to a specific shader program.
     * @param shader Compiled shader program.
     * @param name Debug identifier for the material.
     */
    explicit Material(std::shared_ptr<Shader> shader, std::string name = "DefaultMaterial");
    virtual ~Material();
    
    /*!
     * @brief Sets the base albedo color and automatically updates transparency if alpha < 1.0.
     * @param color RGBA linear base color.
     */
    void SetBaseColor(const Math::Vec4& color) noexcept;
    [[nodiscard]] const Math::Vec4& GetBaseColor() const noexcept { return m_Uniforms.BaseColor; }
    
    /*!
     * @brief Sets the surface microfacet roughness.
     * @param roughness Roughness value in [0.0, 1.0].
     */
    void SetRoughness(float roughness) noexcept;
    [[nodiscard]] float GetRoughness() const noexcept { return m_Uniforms.Roughness; }
    
    /*!
     * @brief Sets the surface metalness factor.
     * @param metallic Metallic value in [0.0, 1.0].
     */
    void SetMetallic(float metallic) noexcept;
    [[nodiscard]] float GetMetallic() const noexcept { return m_Uniforms.Metallic; }
    
    /*!
     * @brief Sets the HDR emissive color for glowing surfaces and light fixtures.
     * @param emissive Emissive RGB radiance.
     */
    void SetEmissiveColor(const Math::Vec4& emissive) noexcept;
    [[nodiscard]] const Math::Vec4& GetEmissiveColor() const noexcept { return m_Uniforms.EmissiveColor; }
    
    /*!
     * @brief Explicitly sets whether this material should be treated as transparent (e.g., skipping shadow caster pass).
     * @param transparent True if transparent.
     */
    void SetTransparent(bool transparent) noexcept { m_IsTransparent = transparent; }
    [[nodiscard]] bool IsTransparent() const noexcept { return m_IsTransparent; }
    
    void SetAlbedoTexture(std::shared_ptr<Texture2D> texture) noexcept;
    [[nodiscard]] const std::shared_ptr<Texture2D>& GetAlbedoTexture() const noexcept { return m_AlbedoTexture; }
    
    void SetNormalTexture(std::shared_ptr<Texture2D> texture) noexcept;
    [[nodiscard]] const std::shared_ptr<Texture2D>& GetNormalTexture() const noexcept { return m_NormalTexture; }
    
    void SetRoughnessTexture(std::shared_ptr<Texture2D> texture) noexcept;
    [[nodiscard]] const std::shared_ptr<Texture2D>& GetRoughnessTexture() const noexcept { return m_RoughnessTexture; }
    
    void SetMetallicTexture(std::shared_ptr<Texture2D> texture) noexcept;
    [[nodiscard]] const std::shared_ptr<Texture2D>& GetMetallicTexture() const noexcept { return m_MetallicTexture; }
    
    void SetAOTexture(std::shared_ptr<Texture2D> texture) noexcept;
    [[nodiscard]] const std::shared_ptr<Texture2D>& GetAOTexture() const noexcept { return m_AOTexture; }
    
    void SetHeightTexture(std::shared_ptr<Texture2D> texture) noexcept;
    [[nodiscard]] const std::shared_ptr<Texture2D>& GetHeightTexture() const noexcept { return m_HeightTexture; }
    
    void SetTilingFactor(const Math::Vec3& tiling) noexcept;
    [[nodiscard]] const Math::Vec3& GetTilingFactor() const noexcept { return m_Uniforms.TilingFactor; }
    
    /*!
     * @brief Synchronizes CPU material parameters into the GPU buffer for the given frame index.
     * @param frameIndex In-flight frame index [0, kMaxFramesInFlight - 1].
     */
    void Sync(uint32_t frameIndex);
    
    [[nodiscard]] MTL::Buffer* GetUniformBuffer(uint32_t frameIndex) const noexcept;
    [[nodiscard]] const std::shared_ptr<Shader>& GetShader() const noexcept { return m_Shader; }
    [[nodiscard]] const std::string& GetName() const noexcept { return m_Name; }
    
    /*!
     * @brief Factory method to instantiate a shared Material.
     * @param shader Associated shader program.
     * @param name Material identifier.
     * @return Shared pointer to the newly created Material.
     */
    [[nodiscard]] static std::shared_ptr<Material> Create(std::shared_ptr<Shader> shader,
                                                          std::string name = "Material");
    
  private:
    std::string                m_Name;                                     ///< Material debug name.
    std::shared_ptr<Shader>    m_Shader;                                   ///< Bound shader program.
    MaterialUniforms           m_Uniforms;                                 ///< CPU-side uniform data.
    bool                       m_Dirty{true};                              ///< Dirty flag for uniform sync.
    bool                       m_IsTransparent{false};                     ///< True if alpha-blended / non-shadow-casting.
    static constexpr uint32_t  kMaxFramesInFlight = 3;                     ///< Triple buffering count.
    MTL::Buffer*               m_UniformBuffers[kMaxFramesInFlight]{nullptr}; ///< Per-frame Metal uniform buffers.
    std::shared_ptr<Texture2D> m_AlbedoTexture;                            ///< Albedo map.
    std::shared_ptr<Texture2D> m_NormalTexture;                            ///< Normal map.
    std::shared_ptr<Texture2D> m_RoughnessTexture;                         ///< Roughness map.
    std::shared_ptr<Texture2D> m_MetallicTexture;                          ///< Metallic map.
    std::shared_ptr<Texture2D> m_AOTexture;                                ///< Ambient occlusion map.
    std::shared_ptr<Texture2D> m_HeightTexture;                            ///< Height / displacement map.
  };
  
} // namespace KanVerse
