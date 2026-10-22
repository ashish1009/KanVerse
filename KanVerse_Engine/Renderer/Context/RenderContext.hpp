/*!
 * @file RenderContext.hpp
 * @brief RAII encapsulation of Apple Metal device and command queue hardware handles.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#pragma once

#include <cstdint>
#include <memory>

namespace MTL {
  class Device;
  class CommandQueue;
}

namespace KanVerse {
  
  /*!
   * @class RenderContext
   * @brief Manages the primary Metal hardware context, device querying, and command queue.
   */
  class RenderContext {
  public:
    /*!
     * @brief Queries the system default Metal device and initializes the primary command queue.
     */
    RenderContext();
    
    /*!
     * @brief Releases the command queue and device hardware handles.
     */
    ~RenderContext();
    
    RenderContext(const RenderContext&) = delete;
    RenderContext& operator=(const RenderContext&) = delete;
    RenderContext(RenderContext&&) noexcept;
    RenderContext& operator=(RenderContext&&) noexcept;
    
    /*!
     * @brief Retrieves the active Metal system device pointer.
     * @return Raw pointer to MTL::Device.
     */
    [[nodiscard]] MTL::Device* GetDevice() const noexcept { return m_Device; }
    
    /*!
     * @brief Retrieves the active primary command queue pointer.
     * @return Raw pointer to MTL::CommandQueue.
     */
    [[nodiscard]] MTL::CommandQueue* GetCommandQueue() const noexcept { return m_CommandQueue; }
    
    /*!
     * @brief Factory constructor generating an initialized RenderContext instance.
     * @return Unique pointer owning the initialized RenderContext.
     */
    [[nodiscard]] static std::unique_ptr<RenderContext> Create();
    
  private:
    MTL::Device*       m_Device{nullptr};
    MTL::CommandQueue* m_CommandQueue{nullptr};
  };
  
} // namespace KanVerse
