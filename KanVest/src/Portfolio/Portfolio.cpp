//
//  Portfolio.cpp
//  KanVest
//
//  Created by Ashish . on 05/11/25.
//

#include "Portfolio.hpp"

namespace KanVest
{
  void Portfolio::AddHolding(const Holding& holding)
  {
    // Check if holding already exists
    auto it = std::find_if(m_holdings.begin(), m_holdings.end(), [&](const Holding& existing) { return existing.symbol == holding.symbol; });
    
    if (it != m_holdings.end())
    {
      // Update existing holding (weighted average)
      int32_t totalQty = it->quantity + holding.quantity;
      if (totalQty > 0)
      {
        double totalValue = (it->averagePrice * it->quantity) + (holding.averagePrice * holding.quantity);
        it->averagePrice = totalValue / totalQty;
      }
      
      it->quantity = totalQty;
      
      // Optionally update vision if new one is different
      if (it->vision != holding.vision)
      {
        it->vision = holding.vision;
      }
    }
    else
    {
      // Add new holding
      m_holdings.push_back(holding);
    }
  }
  
  std::vector<Holding>& Portfolio::GetHoldings()
  {
    return m_holdings;
  }
  
  const std::vector<Holding>& Portfolio::GetHoldings() const
  {
    return m_holdings;
  }
  
  const Holding& Portfolio::GetHolding(const std::string &symbol) const
  {
    static Holding EmptyHolding;
    for (const auto& h : m_holdings)
    {
      if (h.symbol == symbol)
      {
        return h;
      }
    }
    return EmptyHolding;
  }
  
  //--------------------------------------------
  // Save Portfolio → YAML
  //--------------------------------------------
  bool PortfolioSerializer::SaveToYAML(const std::filesystem::path& path, const Portfolio& portfolio)
  {
    try
    {
      YAML::Emitter out;
      out << YAML::BeginMap;
      out << YAML::Key << "Holdings" << YAML::Value << YAML::BeginSeq;
      
      for (const auto& h : portfolio.GetHoldings())
      {
        out << YAML::BeginMap;
        out << YAML::Key << "Symbol"       << YAML::Value << h.symbol;
        out << YAML::Key << "AveragePrice" << YAML::Value << h.averagePrice;
        out << YAML::Key << "Quantity"     << YAML::Value << h.quantity;
        out << YAML::Key << "Vision"       << YAML::Value << PortfolioUtils::VisionToString(h.vision);
        out << YAML::EndMap;
      }
      
      out << YAML::EndSeq;
      out << YAML::EndMap;
      
      std::ofstream fout(path);
      if (!fout.is_open())
      {
        std::cerr << "❌ Cannot open file for saving: " << path << "\n";
        return false;
      }
      fout << out.c_str();
      fout.close();
      
      std::cout << "✅ Portfolio saved to " << path << "\n";
      return true;
    }
    catch (const std::exception& e)
    {
      std::cerr << "❌ Exception in SaveToYAML: " << e.what() << "\n";
      return false;
    }
  }
  
  //--------------------------------------------
  // Load Portfolio ← YAML
  //--------------------------------------------
  bool PortfolioSerializer::LoadFromYAML(const std::filesystem::path& path, Portfolio& portfolio)
  {
    if (!std::filesystem::exists(path))
    {
      std::cerr << "⚠️ Portfolio file does not exist: " << path << "\n";
      return false;
    }
    
    try
    {
      YAML::Node data = YAML::LoadFile(path);
      if (!data["Holdings"])
      {
        std::cerr << "⚠️ No holdings found in YAML: " << path << "\n";
        return false;
      }
      
      portfolio.GetHoldings().clear();
      for (const auto& node : data["Holdings"])
      {
        Holding h;
        h.symbol       = node["Symbol"].as<std::string>();
        h.averagePrice = node["AveragePrice"].as<double>();
        h.quantity     = node["Quantity"].as<int32_t>();
        h.vision       = PortfolioUtils::StringToVision(node["Vision"].as<std::string>());
        portfolio.AddHolding(h);
      }
      
      std::cout << "✅ Portfolio loaded from " << path << "\n";
      return true;
    }
    catch (const std::exception& e)
    {
      std::cerr << "❌ Exception in LoadFromYAML: " << e.what() << "\n";
      return false;
    }
  }

  // Helper conversion for Vision enum
  namespace PortfolioUtils
  {
    std::string VisionToString(Holding::Vision vision)
    {
      switch (vision)
      {
        case Holding::Vision::LongTerm:  return "LongTerm";
        case Holding::Vision::MidTerm:   return "MidTerm";
        case Holding::Vision::ShortTerm: return "ShortTerm";
      }
      return "Unknown";
    }
    Holding::Vision StringToVision(const std::string& str)
    {
      if (str == "LongTerm")  return Holding::Vision::LongTerm;
      if (str == "MidTerm")   return Holding::Vision::MidTerm;
      if (str == "ShortTerm") return Holding::Vision::ShortTerm;
      return Holding::Vision::LongTerm;
    }
  } // namespace PortfolioUtils
} // namespace KanVest
