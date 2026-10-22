/*!
 * @file EntryPoint.mm
 * @brief Global engine application startup and teardown entry point.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

int main(int argc, char** argv) {
  KanVerse::Log::Init();
  KV_CORE_INFO("KanVerse Engine Bootstrap Initiated.");
  return 0;
}
