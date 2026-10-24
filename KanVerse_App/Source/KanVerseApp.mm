/*!
 * @file KanVerseApp.mm
 * @brief KanVerse client application bootstrap and entry point harness.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#include "Layers/EditorLayer.hpp"

/*!
 * @class KanVerseApp
 * @brief Concrete client specialization of the core engine application.
 */
class KanVerseKanVerseApp : public KanVerse::Application {
public:
  /*!
   * @brief Configures application properties and registers runtime layers.
   */
  KanVerseKanVerseApp()
  : Application(KanVerse::ApplicationProps{
    .Name = "KanVerse",
    .WindowWidth = 1280,
    .WindowHeight = 720,
    .VSync = true,
    .Maximized = true
  }) {
    PushLayer(std::make_unique<KanVerse::EditorLayer>());
  }
  
  /*!
   * @brief Destructor for client application harness.
   */
  ~KanVerseKanVerseApp() override = default;
};

namespace KanVerse {
  
  /*!
   * @brief Factory implementation declared in Core/EntryPoint.hpp.
   * @return Unique pointer owning the initialized Application instance.
   */
  std::unique_ptr<Application> CreateApplication() {
    return std::make_unique<KanVerseKanVerseApp>();
  }
  
} // namespace KanVerse
