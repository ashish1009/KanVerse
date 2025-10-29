//
//  DesignHelper.h
//  KanViz
//
//  Created by Ashish . on 29/10/25.
//

#pragma once

namespace KanViz
{
  /// This class is the vector iterator for any class containing vector. Provides basic APIs regarding vector data.
  template <typename T>
  class VectorIterator
  {
  public:
    // Lifecycle
    explicit VectorIterator(std::vector<T>& data) noexcept : m_data(data) {}
    
    // Modifiers --------------------------------------------------------------------------------------------------------------------------------
    /// This function clears the vector data
    void Clear() noexcept
    {
      m_data.clear();
    }
    
    /// This function emplace back the element
    /// - Parameter args: elements
    template <typename... Args> void EmplaceBack(Args&&... args)
    {
      m_data.emplace_back(std::forward<Args>(args)...);
    }
    
    /// This function erase the data from position
    /// - Parameter position: data position
    void Erase(typename std::vector<T>::iterator position)
    {
      m_data.erase(position);
    }
    
    // Capacity -------------------------------------------------------------------------------------------------------------------------------------
    /// This function returns the size of vector
    size_t Size() const noexcept
    {
      return m_data.size();
    }
    /// This function returns the capacity of vector
    size_t Capacity() const noexcept
    {
      return m_data.capacity();
    }
    /// This function returns if vector is empty
    bool Empty() const noexcept
    {
      return m_data.empty();
    }
    
    // Element Access --------------------------------------------------------------------------------------------------------------------------------
    /// This function returns the element at index
    /// - Parameter index: index which element to be returned
    T& At(size_t index)
    {
      if (index >= m_data.size()) throw std::out_of_range("Index out of range");
      return m_data[index];
    }
    
    /// This function returns the element at index
    /// - Parameter index: index which element to be returned
    const T& At(size_t index) const
    {
      if (index >= m_data.size()) throw std::out_of_range("Index out of range");
      return m_data[index];
    }
    
    /// This function returns the front element
    T& Front() noexcept { return m_data.front(); }
    const T& Front() const noexcept { return m_data.front(); }
    
    /// This function returns the last element
    T& Back() noexcept { return m_data.back(); }
    const T& Back() const noexcept { return m_data.back(); }
    
    /// This function returns the data pointer
    T* Data() noexcept { return m_data.data(); }
    const T* Data() const noexcept { return m_data.data(); }
    
    // Iterators
    using iterator               = typename std::vector<T>::iterator;
    using const_iterator         = typename std::vector<T>::const_iterator;
    using reverse_iterator       = typename std::vector<T>::reverse_iterator;
    using const_reverse_iterator = typename std::vector<T>::const_reverse_iterator;
    
    // Forward Iterators ---------------------------------------------------------------------------------------------------------------------------
    iterator begin() { return m_data.begin(); }
    iterator end() { return m_data.end(); }
    const_iterator begin() const { return m_data.begin(); }
    const_iterator end() const { return m_data.end(); }
    
    // Reverse Iterators ---------------------------------------------------------------------------------------------------------------------------
    const_reverse_iterator rbegin() const { return m_data.rbegin(); }
    const_reverse_iterator rend() const { return m_data.rend(); }
    reverse_iterator rbegin() { return m_data.rbegin(); }
    reverse_iterator rend() { return m_data.rend(); }
    
  protected:
    // Vector Data
    std::vector<T>& m_data;
  };
} // namespace KanViz
