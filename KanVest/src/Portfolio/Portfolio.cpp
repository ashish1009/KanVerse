//
//  Portfolio.cpp
//  KanVest
//
//  Created by Ashish . on 21/11/25.
//

#include "Portfolio.hpp"

namespace KanVest
{
  void Portfolio::AddHolding(const Holding &holding)
  {
    // Check if holding already exists
    auto it = std::find_if(m_holdings.begin(), m_holdings.end(),
                           [&](const Holding& existing) {
      return existing.symbolName == holding.symbolName;
    });
    
    if (it != m_holdings.end())
    {
      // Update existing holding (weighted average)
      int32_t totalQuantity = holding.quantity + it->quantity;
      if (totalQuantity > 0)
      {
        float totalValue = (it->averagePrice * it->quantity) + (holding.averagePrice * holding.quantity);
        it->averagePrice = totalValue / totalQuantity;
      }
      
      // Update total of holding
      it->quantity = totalQuantity;
    }
    else
    {
      // Add new holding
      m_holdings.emplace_back(holding);
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
  
  const Holding& Portfolio::GetHolding(const std::string &symbolName) const
  {
    static Holding EmptyHolding;
    for (const auto& h : m_holdings)
    {
      if (h.symbolName == symbolName)
      {
        return h;
      }
    }
    return EmptyHolding;
  }
  
  float Portfolio::GetTotalInvestment()
  {
    return std::accumulate(m_holdings.begin(),m_holdings.end(),0.0f,
                           [](float total, const Holding& h) {
                             return total + h.investment;
                           });
  }
  
  float Portfolio::GetTotalValue()
  {
    return std::accumulate(m_holdings.begin(),m_holdings.end(),0.0f,
                           [](float total, const Holding& h) {
                             return total + h.value;
                           });
  }
  
  bool PortfolioSerializer::SaveToYAML(const std::filesystem::path& path, const Portfolio& portfolio)
  {
    try {
      YAML::Emitter out;
      out << YAML::BeginMap;
      out << YAML::Key << "Holdings" << YAML::Value << YAML::BeginSeq;

      for (const auto& h : portfolio.GetHoldings())
      {
        out << YAML::BeginMap;
        out << YAML::Key << "Symbol"       << YAML::Value << h.symbolName;
        out << YAML::Key << "AveragePrice" << YAML::Value << h.averagePrice;
        out << YAML::Key << "Quantity"     << YAML::Value << h.quantity;
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
        h.symbolName   = node["Symbol"].as<std::string>();
        h.averagePrice = node["AveragePrice"].as<double>();
        h.quantity     = node["Quantity"].as<int32_t>();
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
} // namespace KanVest
