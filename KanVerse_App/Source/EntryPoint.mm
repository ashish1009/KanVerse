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

/*!
 * @brief Client-implemented application factory.
 * @return Dynamically created unique pointer to the Application subclass.
 */
extern std::unique_ptr<KanVerse::Application> KanVerse::CreateApplication();

int main(int argc, char** argv) {
  KanVerse::Log::Init();
  KV_INFO("KanVerse Engine Bootstrap Initiated.");

  // Creating core application and run the game loop
  {
    std::unique_ptr<KanVerse::Application> app = KanVerse::CreateApplication();
    if (!app) {
      KV_FATAL("Failed to instantiate KanVerse Application subclass!");
      return -1;
    }
    
    app->Run();
  }

  // At this point, app, its layers, buffers, and all destructors have completed
  auto stats = KanVerse::MemoryTracker::GetStats();
  if (stats.CurrentUsage > 0) {
    KV_WARN("Memory leak detected! {} bytes remaining in memory tracker.", stats.CurrentUsage);
  } else {
    KV_INFO("Teardown clean: 0 bytes leaked. Peak memory: {:.2f} MB",
                 static_cast<double>(stats.PeakUsage) / (1024.0 * 1024.0));
  }
  
  KV_INFO("KanVerse Engine Terminated Cleanly: All destructors executed.");

#if DEBUG
  KanVerse::Testing::RunPhase1Tests();
#endif

  KanVerse::Log::Shutdown();
  return 0;
}
