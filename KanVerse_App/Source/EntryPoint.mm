/*!
 * @file EntryPoint.mm
 * @brief Global engine application startup and teardown entry point.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#define UNIT_TESTING 1

#if UNIT_TESTING
#include "UnitTests/Phase1Test.hpp"
#endif

int main(int argc, char** argv) {
  KanVerse::Log::Init();
  KV_CORE_INFO("KanVerse Engine Bootstrap Initiated.");

#if DEBUG
  KanVerse::Testing::RunPhase1Tests();
#endif

  KanVerse::Log::Shutdown();
  return 0;
}
