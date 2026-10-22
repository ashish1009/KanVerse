/*!
 * @file GameControllerBridge.hpp
 * @brief Hardware gamepad and raw keyboard input bridge for Apple OS frameworks.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#pragma once

#include <simd/simd.h>
#include <cstdint>

namespace KanVerse {
  
  /*!
   * @struct GamepadInputState
   * @brief Polled instantaneous hardware inputs exclusively from connected Gamepads.
   *
   * This struct represents raw hardware state. It does not contain any game-specific
   * logic or keyboard fallbacks.
   */
  struct GamepadInputState {
    simd::float2 LeftStick{0.0f, 0.0f};  ///< Normalized move axis [-1.0, 1.0].
    simd::float2 RightStick{0.0f, 0.0f}; ///< Normalized aim/camera axis [-1.0, 1.0].
    bool ButtonSouth{false};             ///< Primary face button (e.g., Cross / A).
    bool ButtonEast{false};              ///< Right face button (e.g., Circle / B).
    bool ButtonWest{false};              ///< Left face button (e.g., Square / X).
    bool ButtonNorth{false};             ///< Top face button (e.g., Triangle / Y).
    bool LeftTrigger{false};             ///< Left analog trigger / bumper.
    bool RightTrigger{false};            ///< Right analog trigger / bumper.
    bool Connected{false};               ///< True if an active controller is paired.
  };
  
  /*!
   * @class InputBridge
   * @brief Central OS-level hardware polling bridge.
   *
   * Abstracts Apple's GameController.framework and macOS keyboard event pumping.
   * Adheres to SOLID by strictly reporting raw hardware states without gameplay context.
   */
  class InputBridge {
  public:
    /*! @brief Discovers connected wireless and wired controllers. */
    static void Init();
    
    /*! @brief Ceases controller discovery and clears cached hardware states. */
    static void Shutdown();
    
    /*!
     * @brief Registers raw hardware keypresses received from the OS window event pump.
     * @param keyCode Native macOS virtual keycode.
     * @param isPressed True if the key is currently held down.
     */
    static void SetKeyState(uint16_t keyCode, bool isPressed);
    
    /*!
     * @brief Queries whether a specific raw OS keycode is currently physically depressed.
     * @param keyCode Native macOS virtual keycode.
     * @return True if the key is pressed.
     */
    [[nodiscard]] static bool IsKeyPressed(uint16_t keyCode);
    
    /*!
     * @brief Polls connected physical gamepads for analog stick and face button states.
     * @return Current hardware snapshot of the primary connected gamepad.
     */
    [[nodiscard]] static GamepadInputState PollInput();
  };
  
} // namespace KanVerse
