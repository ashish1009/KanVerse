/*!
 * @file MemoryTracking.hpp
 * @brief Thread-safe memory allocation metrics and automated RAII allocation guards.
 * @details Provides hardware-level tracking for Apple Silicon unified memory usage,
 *          capturing allocation volume, peak heap watermarks, and scoped RAII wrappers.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>

namespace KanVerse {
  
  /*!
   * @struct MemoryStats
   * @brief Snapshot of active, peak, and historical engine allocation tallies.
   */
  struct MemoryStats {
    size_t   TotalAllocated{0};  /*!< Cumulative bytes allocated across the entire process lifetime. */
    size_t   TotalFreed{0};      /*!< Cumulative bytes released across the entire process lifetime. */
    size_t   CurrentUsage{0};    /*!< Current active byte allocation residency in memory. */
    size_t   PeakUsage{0};       /*!< High-water mark indicating maximum concurrent bytes allocated. */
    uint64_t AllocationCount{0}; /*!< Total number of distinct allocation events recorded. */
  };
  
  /*!
   * @class MemoryTracker
   * @brief Lock-free atomic tracker recording allocations, deallocations, and peak residency.
   */
  class MemoryTracker {
  public:
    /*!
     * @brief Atomically increments active memory usage and evaluates peak consumption.
     * @param size Number of bytes allocated by the caller.
     */
    static void RecordAllocation(size_t size) noexcept {
      s_TotalAllocated.fetch_add(size, std::memory_order_relaxed);
      s_AllocationCount.fetch_add(1, std::memory_order_relaxed);
      
      size_t current = s_CurrentUsage.fetch_add(size, std::memory_order_relaxed) + size;
      size_t peak = s_PeakUsage.load(std::memory_order_relaxed);
      while (current > peak && !s_PeakUsage.compare_exchange_weak(peak, current, std::memory_order_relaxed)) {
        // Loop until peak reflects the highest observed usage across all worker threads
      }
    }
    
    /*!
     * @brief Atomically decrements active memory usage and tallies freed memory.
     * @param size Number of bytes deallocated by the caller.
     */
    static void RecordDeallocation(size_t size) noexcept {
      s_TotalFreed.fetch_add(size, std::memory_order_relaxed);
      s_CurrentUsage.fetch_sub(size, std::memory_order_relaxed);
    }
    
    /*!
     * @brief Queries an atomic snapshot of current engine memory statistics.
     * @return MemoryStats struct populated with thread-safe atomic loads.
     */
    [[nodiscard]] static MemoryStats GetStats() noexcept {
      return MemoryStats{
        .TotalAllocated  = s_TotalAllocated.load(std::memory_order_relaxed),
        .TotalFreed      = s_TotalFreed.load(std::memory_order_relaxed),
        .CurrentUsage    = s_CurrentUsage.load(std::memory_order_relaxed),
        .PeakUsage       = s_PeakUsage.load(std::memory_order_relaxed),
        .AllocationCount = s_AllocationCount.load(std::memory_order_relaxed)
      };
    }
    
  private:
    inline static std::atomic<size_t>   s_TotalAllocated{0};  /*!< Process lifetime allocation counter. */
    inline static std::atomic<size_t>   s_TotalFreed{0};      /*!< Process lifetime deallocation counter. */
    inline static std::atomic<size_t>   s_CurrentUsage{0};    /*!< Instantaneous active heap residency. */
    inline static std::atomic<size_t>   s_PeakUsage{0};       /*!< Peak high-water mark residency. */
    inline static std::atomic<uint64_t> s_AllocationCount{0}; /*!< Cumulative allocation operation calls. */
  };

  /*!
   * @class ScopedMemoryAllocation
   * @brief RAII scoped guard that registers byte allocations on construction and releases them on destruction.
   * @note Designed to be embedded as a private member in GPU resource wrappers to automate tracking.
   */
  class ScopedMemoryAllocation {
  public:
    /*!
     * @brief Default constructor initializing an empty zero-byte allocation guard.
     */
    ScopedMemoryAllocation() = default;
    
    /*!
     * @brief Constructs an allocation guard and registers bytes with the global tracker.
     * @param size Number of bytes being tracked.
     */
    explicit ScopedMemoryAllocation(size_t size) : m_Size(size) {
      if (m_Size > 0) {
        MemoryTracker::RecordAllocation(m_Size);
      }
    }
    
    /*!
     * @brief Destructor automatically releasing all registered bytes from the tracker.
     */
    ~ScopedMemoryAllocation() {
      Release();
    }
    
    ScopedMemoryAllocation(const ScopedMemoryAllocation&) = delete;
    ScopedMemoryAllocation& operator=(const ScopedMemoryAllocation&) = delete;
    
    /*!
     * @brief Move constructor transferring tracked byte ownership without double counting.
     * @param other R-value reference to the source guard.
     */
    ScopedMemoryAllocation(ScopedMemoryAllocation&& other) noexcept
    : m_Size(other.m_Size) {
      other.m_Size = 0;
    }
    
    /*!
     * @brief Move assignment operator transferring tracked byte ownership safely.
     * @param other R-value reference to the source guard.
     * @return Reference to this guard instance.
     */
    ScopedMemoryAllocation& operator=(ScopedMemoryAllocation&& other) noexcept {
      if (this != &other) {
        Release();
        m_Size = other.m_Size;
        other.m_Size = 0;
      }
      return *this;
    }
    
    /*!
     * @brief Adjusts the tracked memory footprint, releasing previous bytes and recording new ones.
     * @param newSize New footprint in bytes (default 0).
     */
    void Reset(size_t newSize = 0) {
      Release();
      m_Size = newSize;
      if (m_Size > 0) {
        MemoryTracker::RecordAllocation(m_Size);
      }
    }
    
    /*!
     * @brief Queries the total bytes registered by this scoped guard.
     * @return Tracked byte footprint.
     */
    [[nodiscard]] size_t GetSize() const noexcept { return m_Size; }
    
  private:
    /*!
     * @brief Internal helper releasing registered memory tallies back to MemoryTracker.
     */
    void Release() noexcept {
      if (m_Size > 0) {
        MemoryTracker::RecordDeallocation(m_Size);
        m_Size = 0;
      }
    }
    
    size_t m_Size{0}; /*!< Total bytes currently registered by this guard instance. */
  };
} // namespace KanVerse
