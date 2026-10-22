/*!
 * @file FrameSync.hpp
 * @brief Triple-buffered flight synchronizer using Grand Central Dispatch semaphores.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#pragma once

#include <cstdint>

namespace MTL {
  class CommandBuffer;
}

namespace KanVerse {
  
  /*!
   * @class FrameSynchronizer
   * @brief Manages CPU-GPU pacing across N frames in flight using dispatch semaphores.
   */
  class FrameSynchronizer {
  public:
    /*!
     * @brief Constructs a frame synchronizer for a specified maximum flight depth.
     * @param maxFramesInFlight Flight ring depth (default: 3).
     */
    explicit FrameSynchronizer(uint32_t maxFramesInFlight = 3);
    
    /*!
     * @brief Teardown releasing the underlying GCD semaphore handle.
     */
    ~FrameSynchronizer();
    
    FrameSynchronizer(const FrameSynchronizer&) = delete;
    FrameSynchronizer& operator=(const FrameSynchronizer&) = delete;
    FrameSynchronizer(FrameSynchronizer&&) noexcept;
    FrameSynchronizer& operator=(FrameSynchronizer&&) noexcept;
    
    /*!
     * @brief Blocks CPU thread until a flight slot becomes available and advances the ring index.
     * @return Current active frame ring index (0 to maxFramesInFlight - 1).
     */
    uint32_t BeginFrame();
    
    /*!
     * @brief Registers the completion handler on the command buffer to signal the semaphore upon GPU completion.
     * @param cmdBuffer Command buffer being committed.
     */
    void AttachToCommandBuffer(MTL::CommandBuffer* cmdBuffer);
    
    /*!
     * @brief Queries the current active frame ring index.
     * @return Active flight index.
     */
    [[nodiscard]] uint32_t GetCurrentFrameIndex() const noexcept { return m_CurrentFrameIndex; }
    
    /*!
     * @brief Queries the maximum flight depth.
     * @return Max frames in flight.
     */
    [[nodiscard]] uint32_t GetMaxFramesInFlight() const noexcept { return m_MaxFramesInFlight; }
    
  private:
    void*    m_Semaphore{nullptr};          ///< Opaque dispatch_semaphore_t handle.
    uint32_t m_MaxFramesInFlight{3};
    uint32_t m_CurrentFrameIndex{0};
  };
  
} // namespace KanVerse
