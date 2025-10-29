//
//  YAMLUtils.hpp
//  KanViz
//
//  Created by Ashish . on 10/01/25.
//

#pragma once

#include <yaml-cpp/yaml.h>

namespace YAML
{
  template<>
  struct convert<glm::vec2>
  {
    static Node encode(const glm::vec2& rhs)
    {
      Node node;
      node.push_back(rhs.x);
      node.push_back(rhs.y);
      return node;
    }
    
    static bool decode(const Node& node, glm::vec2& rhs)
    {
      if (!node.IsSequence() or node.size() != 2)
        return false;
      
      rhs.x = node[0].as<float>();
      rhs.y = node[1].as<float>();
      return true;
    }
  };
  
  template<>
  struct convert<glm::vec3>
  {
    static Node encode(const glm::vec3& rhs)
    {
      Node node;
      node.push_back(rhs.x);
      node.push_back(rhs.y);
      node.push_back(rhs.z);
      return node;
    }
    
    static bool decode(const Node& node, glm::vec3& rhs)
    {
      if (!node.IsSequence() or node.size() != 3)
        return false;
      
      rhs.x = node[0].as<float>();
      rhs.y = node[1].as<float>();
      rhs.z = node[2].as<float>();
      return true;
    }
  };
  
  template<>
  struct convert<glm::vec4>
  {
    static Node encode(const glm::vec4& rhs)
    {
      Node node;
      node.push_back(rhs.x);
      node.push_back(rhs.y);
      node.push_back(rhs.z);
      node.push_back(rhs.w);
      return node;
    }
    
    static bool decode(const Node& node, glm::vec4& rhs)
    {
      if (!node.IsSequence() or node.size() != 4)
        return false;
      
      rhs.x = node[0].as<float>();
      rhs.y = node[1].as<float>();
      rhs.z = node[2].as<float>();
      rhs.w = node[3].as<float>();
      return true;
    }
  };
  
  template<>
  struct convert<glm::quat>
  {
    static Node encode(const glm::quat& rhs)
    {
      Node node;
      node.push_back(rhs.w);
      node.push_back(rhs.x);
      node.push_back(rhs.y);
      node.push_back(rhs.z);
      return node;
    }
    
    static bool decode(const Node& node, glm::quat& rhs)
    {
      if (!node.IsSequence() or node.size() != 4)
        return false;
      
      rhs.w = node[0].as<float>();
      rhs.x = node[1].as<float>();
      rhs.y = node[2].as<float>();
      rhs.z = node[3].as<float>();
      return true;
    }
  };
} // namespace YAML

namespace KanViz
{
  inline YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v)
  {
    out << YAML::Flow;
    out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
    return out;
  }
  
  inline YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
  {
    out << YAML::Flow;
    out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
    return out;
  }
  
  
  inline YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
  {
    out << YAML::Flow;
    out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
    return out;
  }
  
  inline YAML::Emitter& operator<<(YAML::Emitter& out, const glm::quat& v)
  {
    out << YAML::Flow;
    out << YAML::BeginSeq << v.w << v.x << v.y << v.z << YAML::EndSeq;
    return out;
  }

  /// This class creates scoped YAML map and insert elements
  class ScopedYAMLMap
  {
  public:
    /// This constructor creates scoped YAML instance with out emitter and begins the map
    /// - Parameter out: out emitter
    explicit ScopedYAMLMap(YAML::Emitter& out)
    : m_out(out)
    {
      m_out << YAML::BeginMap;
    }
    /// This destructor ends the YAML map
    ~ScopedYAMLMap()
    {
      m_out << YAML::EndMap;
    }
    
    /// This function inserts the element in map
    /// - Parameters:
    ///   - key: key of map
    ///   - value: values of map
    template <typename T> void Insert(std::string_view key, T value)
    {
      m_out << YAML::Key << key.data() << YAML::Value << value;
    }
    
    /// This funciton insertes the element in map to create list of map as value
    /// - Parameter key: key of map
    void InsertMapKey(std::string_view key)
    {
      m_out << YAML::Key << key.data() << YAML::Value;
    }
    /// This funciton insertes the element in map to create list of map as value
    /// - Parameter key: key of map
    void InsertSeqKey(std::string_view key)
    {
      m_out << YAML::Key << key.data() << YAML::BeginSeq;
    }
    
    void EndSeq()
    {
      m_out << YAML::EndSeq;
    }

  private:
    YAML::Emitter& m_out;
  };
} // namespace KanViz
