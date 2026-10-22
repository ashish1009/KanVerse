/*!
 * @file Assert.hpp
 * @brief Branch-predicted assert macros with diagnostic output.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#pragma once

#include <iostream>

#if defined(DEBUG) || !defined(NDEBUG)
#define KV_ENABLE_ASSERTS 1
#else
#define KV_ENABLE_ASSERTS 0
#endif

#if KV_ENABLE_ASSERTS
#if defined(__has_builtin) && __has_builtin(__builtin_trap)
#define KV_BREAK() __builtin_trap()
#else
#include <csignal>
#define KV_BREAK() raise(SIGTRAP)
#endif

#define KV_CORE_ASSERT(condition, msg) \
do { \
if (__builtin_expect(!(condition), 0)) { \
std::cerr << "\033[31m[KanVerse Assert] Assertion Failed: (" #condition ") at " \
<< __FILE__ << ":" << __LINE__ << " - " << msg << "\033[0m" << std::endl; \
KV_BREAK(); \
} \
} while (false)

#define KV_ASSERT(condition, msg) KV_CORE_ASSERT(condition, msg)
#else
#define KV_CORE_ASSERT(condition, msg) ((void)0)
#define KV_ASSERT(condition, msg)      ((void)0)
#define KV_BREAK()                     ((void)0)
#endif
