/*!
 * @file EntityID.hpp
 * @brief Generational 64-bit entity handle representation and bit-packing utilities.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#pragma once

#include <cstdint>

namespace KanVerse {
  
  /*!
   * @typedef EntityID
   * @brief 64-bit packed entity identifier consisting of a 32-bit slot index and a 32-bit generation counter.
   *
   * @details
   * Bit Layout:
   * - `[ 63 .. 32 ]` : Generation (32 bits) — Incremented whenever a slot is destroyed to detect dangling handles.
   * - `[ 31 ..  0 ]` : Entity Index (32 bits) — Direct $O(1)$ lookup index into the paged sparse array.
   */
  using EntityID = std::uint64_t;
  
  namespace ECS {
    
    /*!
     * @brief Sentinel constant denoting an invalid or null entity handle.
     */
    inline constexpr EntityID NullEntity = 0xFFFFFFFFFFFFFFFFULL;
    
    /*!
     * @brief Extracts the lower 32-bit slot index from a 64-bit EntityID.
     * @param entity Target 64-bit entity identifier.
     * @return 32-bit sparse slot index ($O(1)$ complexity).
     */
    [[nodiscard]] constexpr std::uint32_t GetIndex(EntityID entity) noexcept {
      return static_cast<std::uint32_t>(entity & 0xFFFFFFFFULL);
    }
    
    /*!
     * @brief Extracts the upper 32-bit generation counter from a 64-bit EntityID.
     * @param entity Target 64-bit entity identifier.
     * @return 32-bit generation counter ($O(1)$ complexity).
     */
    [[nodiscard]] constexpr std::uint32_t GetGeneration(EntityID entity) noexcept {
      return static_cast<std::uint32_t>((entity >> 32) & 0xFFFFFFFFULL);
    }
    
    /*!
     * @brief Packs a 32-bit slot index and 32-bit generation counter into a 64-bit EntityID.
     * @param index Slot index in the registry's sparse array.
     * @param generation Generation counter for slot-recycling safety.
     * @return 64-bit packed EntityID ($O(1)$ complexity).
     */
    [[nodiscard]] constexpr EntityID CreateID(std::uint32_t index, std::uint32_t generation) noexcept {
      return (static_cast<std::uint64_t>(generation) << 32) | static_cast<std::uint64_t>(index);
    }
    
  } // namespace ECS
  
} // namespace KanVerse
