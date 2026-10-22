/*!
 * @file KeyCodes.hpp
 * @brief Strongly-typed virtual key code identifiers for macOS / KanVerse input management.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#pragma once

#include <cstdint>

namespace KanVerse {
  
  /*!
   * @enum Key
   * @brief Strongly-typed enumeration of macOS virtual key codes.
   */
  enum class Key : uint16_t {
    // --- Alphabet Keys ---
    A       = 0,
    S       = 1,
    D       = 2,
    F       = 3,
    H       = 4,
    G       = 5,
    Z       = 6,
    X       = 7,
    C       = 8,
    V       = 9,
    B       = 11,
    Q       = 12,
    W       = 13,
    E       = 14,
    R       = 15,
    Y       = 16,
    T       = 17,
    D1      = 18,
    D2      = 19,
    D3      = 20,
    D4      = 21,
    D6      = 22,
    D5      = 23,
    Equal   = 24,
    D9      = 25,
    D7      = 26,
    Minus   = 27,
    D8      = 28,
    D0      = 29,
    RightBracket = 30,
    O       = 31,
    U       = 32,
    LeftBracket  = 33,
    I       = 34,
    P       = 35,
    Return  = 36,
    L       = 37,
    J       = 38,
    Quote   = 39,
    K       = 40,
    Semicolon = 41,
    Backslash = 42,
    Comma   = 43,
    Slash   = 44,
    N       = 45,
    Period  = 46,
    Grave   = 50,
    Space   = 49,
    
    // --- Modifier & Special Keys ---
    Tab     = 48,
    Backspace = 51,
    Escape  = 53,
    Command = 55,
    Shift   = 56,
    Capslock = 57,
    Option  = 58,
    Control = 59,
    
    // --- Navigation & Arrows ---
    LeftArrow  = 123,
    RightArrow = 124,
    DownArrow  = 125,
    UpArrow    = 126,
    
    // --- Function Keys ---
    F1  = 122,
    F2  = 120,
    F3  = 99,
    F4  = 118,
    F5  = 96,
    F6  = 97,
    F7  = 98,
    F8  = 100,
    F9  = 101,
    F10 = 109,
    F11 = 103,
    F12 = 111
  };
  
} // namespace KanVerse
