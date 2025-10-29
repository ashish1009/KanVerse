//
//  ShaderUniforms.hpp
//  KanViz
//
//  Created by Ashish . on 04/11/24.
//

#pragma once

#include "Renderer/Graphics/Shader.hpp"

namespace KanViz
{
  /// This class is the interface to stores the uniform data
  /// --------------------------------------
  /// int uniformData1
  /// float uniformData2[3]
  /// Struct uniformData3
  /// --------------------------------------
  class ShaderUniformDeclaration
  {
  public:
    /// Virtual Default destcutor for Shader Uniform Declaration
    virtual ~ShaderUniformDeclaration() = default;

    /// This function update the offset of the field
    virtual void SetOffset(uint32_t offset) = 0;

    /// This function returns the name of field
    virtual const std::string& GetName() const = 0;
    /// This functionr returns the size of field
    virtual uint32_t GetSize() const = 0;
    /// This function returns the count of field
    virtual uint32_t GetCount() const = 0;
    /// This function returns the offset of field
    virtual uint32_t GetOffset() const = 0;
    /// This function returns the domain of shader
    virtual ShaderDomain GetDomain() const = 0;
  };
  
  /// This Interface class stores the uniform resource like sampler 2D ...
  class ShaderResourceDeclaration
  {
  public:
    /// This is the default destructor
    virtual ~ShaderResourceDeclaration() = default;
    
    /// This function returns the name of resrouce
    virtual const std::string& GetName() const = 0;
    /// This function returns the register ot resource
    virtual uint32_t GetRegister() const = 0;
    /// This function returns the count of array of resource
    virtual uint32_t GetCount() const = 0;
  };

  /// This Interface class stores all the uniforms of shader both Fundamental and Structures
  class ShaderUniformBufferDeclaration
  {
  public:
    /// This is the defualt destructor
    virtual ~ShaderUniformBufferDeclaration() = default;
    
    /// This function returns the name of buffer
    virtual const std::string& GetName() const = 0;
    /// This function returns the register of buffer
    virtual uint32_t GetRegister() const = 0;
    /// This function returns the size of buffer
    virtual uint32_t GetSize() const = 0;
    /// This function returns all the declaration in buffer
    virtual const std::vector<ShaderUniformDeclaration*>& GetUniformDeclarations() const = 0;
    /// This function finds the uniform declaretion by name
    /// - Parameter name: name of uniform
    virtual ShaderUniformDeclaration* FindUniform(const std::string& name) = 0;
  };
  /// This class stores the shader structure data.
  /// --------------------------------------------
  /// struct Name
  /// {
  ///   int   data1
  ///   float data2[4]
  /// };
  /// Stores the entire structure member variables, its type and size
  /// --------------------------------------------
  class ShaderStruct
  {
  public:
    /// This constructor crertes the Shader strcutre instance
    /// - Parameter name: name of shader structure
    ShaderStruct(std::string_view name);
    /// This destructor destroys the shader structure data
    ~ShaderStruct();
    
    // Funcdamentals ---------------------------------------
    /// This function adds the member field in structure
    /// - Parameter memebreField: member field to be added
    void AddMember(ShaderUniformDeclaration* memebreField);
    /// This function updates the offset value of structure
    /// - Parameter offset: new offset
    void SetOffset(uint32_t offset);

    // Getters ---------------------------------------------
    /// This function returns the size of strucutre
    uint32_t GetSize() const;
    /// This function returns the name of structure
    const std::string& GetName() const;
    /// This function returns the members of structure
    const std::vector<ShaderUniformDeclaration*>& GetMemberFields() const;
    /// This function returns the offset of structure
    uint32_t GetOffset() const;

  private:
    std::string m_name {};                                  // < Name of structure
    uint32_t m_size {0};                                    // < Size of structure
    uint32_t m_offset {0};                                  // < Offset of structure in shader
    std::vector<ShaderUniformDeclaration*> m_memberFields;  // < Member variables of structure
  };
} // namespace KanViz
