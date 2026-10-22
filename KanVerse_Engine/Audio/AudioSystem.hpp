/*!
 * @file AudioSystem.hpp
 * @brief CoreAudio sound effect manager for KanVerse.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#pragma once

#include <string>
#include <string_view>
#include <unordered_map>
#include <memory>

namespace KanVerse {
  
  /*!
   * @enum SoundEffect
   * @brief Enumerated identifiers for game audio cues.
   */
  enum class SoundEffect : uint8_t {
    Jump = 0,
    DoubleJump,
    Slide,
    GroundSlam,
    Shatter,
    PrismCollect
    /* Add More */
  };
  
  /*!
   * @class AudioSystem
   * @brief Manages loading and immediate playback of sound effects.
   */
  class AudioSystem {
  public:
    /*!
     * @brief Initializes the audio subsystem.
     */
    static void Init();
    
    /*!
     * @brief Cleans up cached audio players.
     */
    static void Shutdown();
    
    /*!
     * @brief Loads a sound effect from file path and binds it to an effect ID.
     */
    static void LoadSound(SoundEffect effect, std::string_view filepath);
    
    /*!
     * @brief Triggers playback of a loaded sound effect.
     */
    static void Play(SoundEffect effect);
  };
  
} // namespace KanVerse
