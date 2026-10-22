# KanVerse

A stylized 3D action-platformer engine built from scratch for Apple Silicon (macOS M2) using modern C++23 and Apple metal-cpp[cite: 1]. Features a Sparse-Set ECS, Position-Based Dynamics (PBD) for procedural kinematics and rigid fractures, a 3-frame flight sync ring, and purely procedural geometry with zero external 3D assets[cite: 1].

---

## 📊 Status Tracking Legend

Use the following Markdown checkbox states to track your daily progress:
- `[ ]` **Pending** — Not started yet
- `[-]` **Ongoing** — Currently in progress / debugging
- `[x]` **Done** — Implemented, tested, and verified (0 memory leaks)

### Phase Progress Overview
- [x] **Phase 1:** Engine Core, Diagnostics, Memory & SIMD Math (Days 1–12)
- [x] **Phase 2:** Platform Events, Input Polling & Cocoa/Metal Windowing (Days 13–23)
- [-] **Phase 3:** Metal RHI — Buffers, Shaders, Pipelines, Textures & First 3D Draw (Days 24–40)
- [ ] **Phase 4:** Dear ImGui Backend, Docking, Offscreen Framebuffers & Editor Viewport (Days 41–48)
- [ ] **Phase 5:** Generational ECS, Components & Asset Manager (Days 49–56)
- [ ] **Phase 6:** KanVerse Studio Editor — Hierarchy, Inspector, Content Browser, Gizmos & Picking (Days 57–70)
- [ ] **Phase 7:** Physics Engine, Colliders, PBD Solver & Visual Debug Renderer (Days 71–78)
- [ ] **Phase 8:** Advanced PBR, Shadows, IBL, Compute Bloom, ACES Tonemapping & Atmosphere (Days 79–90)
- [ ] **Phase 9:** Scene Serialization, Prefabs, Undo/Redo & Play-In-Editor (PIE) State Machine (Days 91–98)
- [ ] **Phase 10:** Procedural Architectural Builders & In-Editor Level Design Tools (Days 99–108)
- [ ] **Phase 11:** Gameplay Systems — Player Rig, SpringArm Camera, Doors, AI, Combat & Destruction (Days 109–120)

---

## 🗺️ 120-Day Master Implementation Roadmap

### Phase 1: Engine Core, Diagnostics, Memory & SIMD Math (Days 1–12)
*Goal: Build the zero-dependency foundational primitives that every other file in the engine includes.*

- [x] **Day 1 — Workspace & Xcode/CMake Setup**
  - **Files:** Project Configuration (`KanVerse_Engine` static lib & `KanVerse_App` bundle)
  - **Concepts:** C++23 (`-std=c++2b`), Objective-C++ (`.mm`), ARC vs Manual Retain/Release, linking `Metal`, `AppKit`, `QuartzCore`, `GameController`, and `AVFoundation` frameworks.
  - **Milestone:** Download `metal-cpp` and `stb_image.h` into `Vendor/` and compile an empty target.
- [x] **Day 2 — Branch-Predicted Assertions**
  - **Files:** `Core/Diagnostics/Assert.hpp`
  - **Concepts:** Branch prediction hints (`__builtin_expect`), hardware traps (`__builtin_trap()`), ANSI escape color codes (`\033[31m`), preprocessor `do { ... } while(false)` hygiene.
  - **Milestone:** Write `KV_CORE_ASSERT` and `KV_ASSERT`; verify LLDB trap in Xcode.
- [x] **Day 3 — Tri-Sink Structured Logging**
  - **Files:** `Core/Diagnostics/Log.hpp`, `Core/Diagnostics/Log.cpp`
  - **Concepts:** C++23 `std::vformat` and `std::make_format_args`, variadic templates, triple-sink routing (`std::cout`, `std::ofstream`, and Apple Unified Logging `<os/log.h>`).
  - **Milestone:** Log all 5 severity levels (`Trace` to `Fatal`) and verify `KanVerse_Runtime.log` is created on disk.
- [x] **Day 4 — Lock-Free Atomic Memory Tracker**
  - **Files:** `Core/Memory/MemoryTracking.hpp` (Part 1)
  - **Concepts:** Lock-free concurrency with `std::atomic`, `std::memory_order_relaxed`, Compare-And-Swap (`compare_exchange_weak`) loop for high-water mark (`PeakUsage`).
  - **Milestone:** Implement `MemoryStats` and `MemoryTracker` (`RecordAllocation`, `RecordDeallocation`, `GetStats`).
- [x] **Day 5 — Automated RAII Memory Guards**
  - **Files:** `Core/Memory/MemoryTracking.hpp` (Part 2)
  - **Concepts:** Strict RAII design, deleting copy constructors/assignments, move semantics (`&& noexcept`) without double-counting bytes.
  - **Milestone:** Implement `ScopedMemoryAllocation`; verify `CurrentUsage` returns to `0` upon exiting a test scope.
- [x] **Day 6 — High-Resolution Monotonic Frame Timer**
  - **Files:** `Core/Time/TimeStep.hpp`, `Core/Time/Clock.hpp`, `Core/Time/Clock.cpp`
  - **Concepts:** `std::chrono::steady_clock`, floating-point duration casting, `constexpr` and `[[nodiscard]]` attributes, implicit `operator float()`.
  - **Milestone:** Run a 60-iteration loop with `usleep(16666)` and log `dt.GetMilliseconds()`.
- [x] **Day 7 — Apple Silicon SIMD Vector & Matrix Types**
  - **Files:** `Core/Math/Math.hpp` (Part 1)
  - **Concepts:** `<simd/simd.h>` register types (`simd::float2`, `float3`, `float4`, `float4x4`), 16-byte alignment rules (`sizeof(simd::float3) == 16`), column-major layout.
  - **Milestone:** Implement vector aliases, `MakeVec*` helpers, `Identity()`, `Multiply()`, `Inverse()`, and `Transpose()`.
- [x] **Day 8 — Metal [0, 1] Clip-Space Projection Matrices**
  - **Files:** `Core/Math/Math.hpp` (Part 2)
  - **Concepts:** Right-handed coordinate systems, Metal's `[0, 1]` NDC Z-depth, Rodrigues' rotation formula, `LookAt` view matrix derivation.
  - **Milestone:** Implement `Perspective()`, `Ortho()`, `LookAt()`, and `Rotate()`; verify `-nearZ` maps to `0.0f` and `-farZ` maps to `1.0f`.
- [x] **Day 9 — Engine Precompiled Header**
  - **Files:** `KanVerse_Engine/PrefixHeader.pch`
  - **Concepts:** Precompiled header mechanics, accelerating `.cpp` and `.mm` compilation, suppressing clang warnings (`-Wdocumentation`, `-Wformat-security`).
  - **Milestone:** Populate `PrefixHeader.pch` and enable precompilation in Xcode.
- [x] **Day 10 — Metal-CPP Implementation & Hardware Context**
  - **Files:** `Source/MetalImpl.cpp`, `Renderer/Context/RenderContext.hpp`, `Renderer/Context/RenderContext.mm`
  - **Concepts:** Generating `metal-cpp` symbols via `*_PRIVATE_IMPLEMENTATION` macros, `__bridge MTL::Device*`, `MTL::CommandQueue`.
  - **Milestone:** Instantiate `RenderContext::Create()` and print the Apple M-series GPU name to the console.
- [x] **Day 11 — Low-Latency CoreAudio / AVFoundation Subsystem**
  - **Files:** `Audio/AudioSystem.hpp`, `Audio/AudioSystem.mm`
  - **Concepts:** Objective-C++ `@autoreleasepool`, `AVAudioPlayer`, rapid re-triggering (`player.currentTime = 0`), ARC memory management with STL maps.
  - **Milestone:** Implement `AudioSystem::Init`, `LoadSound`, `Play`, and `Shutdown`.
- [x] **Day 12 — Phase 1 Review & Unit Testing**
  - **Files:** Test harness in `Source/EntryPoint.mm`
  - **Concepts:** Memory leak verification, log file inspection, math edge-case validation.
  - **Milestone:** Exercise all Phase 1 systems in a test run and verify `0 bytes leaked` at exit.

---

### Phase 2: Platform Events, Input Polling & Cocoa/Metal Windowing (Days 13–23)
*Goal: Establish OS communication, keyboard/mouse/gamepad input, and a live 120Hz Retina `MTKView` window.*

- [x] **Day 13 — Base Event Hierarchy & Type-Safe Dispatcher**
  - **Files:** `Platform/Events/Event.hpp`
  - **Concepts:** Polymorphic events, bitflag categories (`1 << 0`), token pasting (`#type`) in `EVENT_CLASS_TYPE`, template dispatch (`EventDispatcher::Dispatch<T>`).
  - **Milestone:** Implement `EventType`, `EventCategory`, `Event`, and `EventDispatcher`.
- [x] **Day 14 — Window, Keyboard & Mouse Event Payloads**
  - **Files:** `Platform/Events/WindowEvents.hpp`, `Platform/Events/KeyEvents.hpp`, `Platform/Events/MouseEvents.hpp`
  - **Concepts:** Concrete event classes (`WindowResizeEvent`, `WindowCloseEvent`, `KeyPressedEvent`, `MouseMovedEvent`, `MouseScrolledEvent`, `MouseButtonPressedEvent`).
  - **Milestone:** Implement all three event headers and test routing through `EventDispatcher`.
- [x] **Day 15 — Hardware Gamepad & Keyboard Polling Bridge**
  - **Files:** `Platform/Input/KeyCodes.hpp`, `Platform/Input/GameControllerBridge.hpp`, `Platform/Input/GameControllerBridge.mm`
  - **Concepts:** macOS virtual keycodes (`Key::W = 13`, `Key::Space = 49`), `GameController.framework` (`GCController`, `GCExtendedGamepad`), `0.15f` radial thumbstick deadzone.
  - **Milestone:** Implement `KeyCodes.hpp` and `InputBridge`; test polling with a Bluetooth controller.
- [x] **Day 16 — Abstract Window Interface**
  - **Files:** `Platform/Window/Window.hpp`, `Platform/Window/NativeWindow.hpp`
  - **Concepts:** Platform-independent `Window` contract, `WindowProps`, `EventCallbackFn`, opaque `void*` handles (`m_WindowHandle`, `m_ViewHandle`, `m_DelegateHandle`).
  - **Milestone:** Write `Window.hpp` and `NativeWindow.hpp`.
- [x] **Day 17 — Custom `KanVerseMetalView` Subclass**
  - **Files:** `Platform/Window/NativeWindow.mm` (Part 1)
  - **Concepts:** Subclassing `MTKView`, `NSTrackingArea` for mouse movement, overriding `keyDown:`, `keyUp:`, `flagsChanged:`, `mouseDown:`, `mouseMoved:`, `scrollWheel:`.
  - **Milestone:** Implement `@interface KanVerseMetalView : MTKView` and connect OS input callbacks.
- [x] **Day 18 — Cocoa Window & Application Lifecycle Delegate**
  - **Files:** `Platform/Window/NativeWindow.mm` (Part 2)
  - **Concepts:** `NSWindowDelegate` & `NSApplicationDelegate`, intercepting `applicationShouldTerminate:` and returning `NSTerminateCancel` so C++ destructors run, Retina backing conversion (`convertRectToBacking:`).
  - **Milestone:** Implement `@interface KanVerseWindowDelegate`.
- [x] **Day 19 — Programmatic Cocoa Window Creation & Event Pump**
  - **Files:** `Platform/Window/NativeWindow.mm` (Part 3)
  - **Concepts:** Storyboard-less `NSApp` setup, P-Core QoS pinning (`pthread_set_qos_class_self_np(QOS_CLASS_USER_INTERACTIVE, 0)`), `__bridge_retained` vs `__bridge_transfer` ARC lifetimes, draining `nextEventMatchingMask:`.
  - **Milestone:** Complete `NativeWindow::Init`, `Shutdown`, `OnUpdate`, `SetVSync`, and `Maximize`.
- [x] **Day 20 — Dual-Partitioned Layer Stack**
  - **Files:** `Core/Layers/Layer.hpp`, `Core/Layers/LayerStack.hpp`, `Core/Layers/LayerStack.cpp`
  - **Concepts:** Partitioning standard layers vs overlays using `m_LayerInsertIndex`, reverse event iteration (`rbegin()`) so overlays consume events first.
  - **Milestone:** Implement `Layer` and `LayerStack`.
- [x] **Day 21 — Master Application Singleton & Run Loop**
  - **Files:** `Core/Application/Application.hpp`, `Core/Application/Application.mm`
  - **Concepts:** Singleton orchestrator (`Application::Get()`), coordinating `RenderContext`, `Window`, `LayerStack`, and `Clock` inside `Application::Run()`.
  - **Milestone:** Implement `Application`.
- [x] **Day 22 — Client Application Bootstrap & Entry Point**
  - **Files:** `Source/EntryPoint.mm`, `Source/KanVerseApp.mm`
  - **Concepts:** Engine `main()` vs Client `CreateApplication()`, scoped lifetime block for post-teardown `MemoryTracker::GetStats()` verification.
  - **Milestone:** Implement `EntryPoint.mm` and `KanVerseApp.mm`.
- [x] **Day 23 — Phase 2 Integration Milestone**
  - **Files:** Full Phase 1 + Phase 2 Build
  - **Concepts:** End-to-end event loop, Cmd+Q clean shutdown, P-Core scheduling, Retina resize events.
  - **Milestone:** Launch the 120Hz macOS window, test input/resize logs, quit via `Cmd+Q`, and verify `0 bytes leaked`.

---

### Phase 3: Metal RHI — Buffers, Shaders, Pipelines, Textures & First 3D Draw (Days 24–40)
*Goal: Build the Render Hardware Interface (RHI) right now so we can put 3D geometry on screen immediately.*

- [x] **Day 24 — Vertex Attribute Layout & SIMD Stride Reflection**
  - **Files:** `Renderer/Buffers/BufferLayout.hpp`
  - **Concepts:** `ShaderDataType`, Metal 16-byte alignment (`ShaderDataType::Float3 == 16`), automatic offset and stride calculation.
  - **Milestone:** Implement `ShaderDataType`, `ShaderDataTypeSize`, `BufferElement`, and `BufferLayout`.
- [x] **Day 25 — Unified Memory Vertex & Index Buffers**
  - **Files:** `Renderer/Buffers/VertexBuffer.hpp`, `Renderer/Buffers/VertexBuffer.mm`, `Renderer/Buffers/IndexBuffer.hpp`, `Renderer/Buffers/IndexBuffer.mm`
  - **Concepts:** `MTL::ResourceStorageModeShared` zero-copy unified memory buffers on Apple Silicon, 16-bit vs 32-bit index buffers, `ScopedMemoryAllocation`.
  - **Milestone:** Implement `MetalVertexBuffer` and `MetalIndexBuffer`.
- [x] **Day 26 — Triple-Buffered GCD Semaphore Synchronizer**
  - **Files:** `Renderer/Pipeline/FrameSync.hpp`, `Renderer/Pipeline/FrameSync.mm`
  - **Concepts:** Triple buffering (`kMaxFramesInFlight = 3`), `dispatch_semaphore_create(3)`, `dispatch_semaphore_wait`, `cmdBuffer->addCompletedHandler` signaling.
  - **Milestone:** Implement `FrameSynchronizer`.
- [x] **Day 27 — Metal Shading Language (MSL) Compiler & Library Loader**
  - **Files:** `Renderer/Pipeline/Shader.hpp`, `Renderer/Pipeline/Shader.mm`
  - **Concepts:** Runtime MSL compilation (`newLibrary` with `setFastMathEnabled(true)`) vs precompiled `newDefaultLibrary()`, vertex-only shadow shaders (`CreateVertexOnly`).
  - **Milestone:** Implement `MetalShader` and all factory methods.
- [x] **Day 28 — Graphics Pipeline State & Depth/Blend Configuration**
  - **Files:** `Renderer/Pipeline/Pipeline.hpp`, `Renderer/Pipeline/Pipeline.mm`
  - **Concepts:** `MTL::VertexDescriptor` (16-byte aligned stride), `MTL::RenderPipelineDescriptor` (HDR `RGBA16Float` vs SDR `BGRA8Unorm_sRGB`, alpha blending), `MTL::DepthStencilState`, shadow depth bias.
  - **Milestone:** Implement `MetalPipeline`.
- [ ] **Day 29 — Low-Level Render Command Dispatcher**
  - **Files:** `Renderer/Renderer/RenderCommand.hpp`, `Renderer/Renderer/RenderCommand.mm`
  - **Concepts:** Binding pipeline states and vertex streams, encoding indexed and hardware-instanced draw calls (`drawIndexedPrimitives` with `instanceCount`).
  - **Milestone:** Implement `RenderCommand`.
- [ ] **Day 30 — 2D Texture Loader (ImageIO / CoreGraphics & sRGB vs Linear)**
  - **Files:** `Renderer/Textures/Texture.hpp`, `Renderer/Textures/Texture2D.hpp`, `Renderer/Textures/Texture2D.mm`
  - **Concepts:** Decoding PNG/JPG via `CGImageSourceCreateWithURL` and `CGBitmapContextCreate`, `RGBA8Unorm_sRGB` (Albedo) vs `RGBA8Unorm` (Data maps), procedural checkerboard fallback.
  - **Milestone:** Implement `Texture.hpp`, `Texture2D.hpp`, and `MetalTexture2D`.
- [ ] **Day 31 — PBR Material System & Triple-Buffered Uniforms**
  - **Files:** `Renderer/Materials/Material.hpp`, `Renderer/Materials/Material.mm`
  - **Concepts:** 256-byte aligned `MaterialUniforms`, 6 PBR texture bindings, automatic transparency flagging (`color.w < 0.99f`), `Sync(frameIndex)`.
  - **Milestone:** Implement `Material`.
- [ ] **Day 32 — Procedural Chamfered Box Generator**
  - **Files:** `Renderer/Procedural/ChamferBox.hpp`, `Renderer/Procedural/ChamferBox.cpp`
  - **Concepts:** 64-byte SIMD-aligned `Vertex` struct, 6 inner faces + 12 curved bevel edges with smooth normal interpolation.
  - **Milestone:** Implement `ChamferBoxGenerator::Generate`.
- [ ] **Day 33 — Procedural UV Capsule Generator**
  - **Files:** `Renderer/Procedural/Capsule.hpp`, `Renderer/Procedural/Capsule.cpp`
  - **Concepts:** Spherical-to-Cartesian UV mesh generation (`phi`, `theta`), splitting top and bottom hemispheres around a cylindrical equator, CCW winding.
  - **Milestone:** Implement `Capsule::Create`.
- [ ] **Day 34 — High-Level Mesh Container & SubMesh Bounds**
  - **Files:** `Renderer/Geometry/SubMesh.hpp`, `Renderer/Geometry/Mesh.hpp`, `Renderer/Geometry/Mesh.cpp`
  - **Concepts:** `Submesh` partitioning and local AABB encapsulation, binding `VertexBuffer` and `IndexBuffer` into `Mesh`.
  - **Milestone:** Implement `SubMesh.hpp` and `Mesh`.
- [ ] **Day 35 — Perspective Camera & View-Projection Matrices**
  - **Files:** `Renderer/Camera/Camera.hpp`, `Renderer/Camera/PerspectiveCamera.hpp`, `Renderer/Camera/PerspectiveCamera.cpp`
  - **Concepts:** Base `Camera` contract, caching `m_ViewMatrix` (`Math::LookAt`) and `m_ViewProjectionMatrix`, viewport resize handling.
  - **Milestone:** Implement `Camera` and `PerspectiveCamera`.
- [ ] **Day 36 — Orbit Camera Controller & Screen-Shake Trauma**
  - **Files:** `Renderer/Camera/CameraController.hpp`, `Renderer/Camera/CameraController.mm`
  - **Concepts:** Spherical orbit angles (`m_Yaw`, `m_Pitch`, `m_Distance`), mouse arcball pan/zoom, non-linear quadratic screen-shake trauma (`m_Trauma * m_Trauma`).
  - **Milestone:** Implement `CameraController`.
- [ ] **Day 37 — Photometric Directional, Point & Environment Lights**
  - **Files:** `Renderer/Lighting/Light.hpp`, `Renderer/Lighting/DirectionalLight.hpp`, `Renderer/Lighting/PointLight.hpp`, `Renderer/Lighting/PointLight.cpp`, `Renderer/Lighting/EnvironmentLighting.hpp`
  - **Concepts:** Orthographic shadow frustum matrix (`DirectionalLight::RecalculateMatrices`), constant-linear-quadratic point light attenuation, ambient scaling.
  - **Milestone:** Implement all files in `Renderer/Lighting/`.
- [ ] **Day 38 — Basic Forward Vertex & Fragment Shader**
  - **Files:** `KanVerse_App/Shaders/TexturedMaterial.metal` (Initial Forward Pass)
  - **Concepts:** Writing `vertex_main` (instanced `modelMatrix` & `normalMatrix`) and a basic Lambertian + Blinn-Phong `fragment_main` to test our RHI before full PBR.
  - **Milestone:** Compile `TexturedMaterial.metal` into `default.metallib`.
- [ ] **Day 39 — Core Renderer (Swapchain Forward Pass & Hardware Instancing)**
  - **Files:** `Renderer/Renderer/Renderer.hpp`, `Renderer/Renderer/Renderer.mm` (Core Swapchain Pass)
  - **Concepts:** Allocating triple-buffered `GPUUniforms` and `InstanceBuffers`, implementing `BeginScene`, `SubmitInstanced`, and `EndScene` rendering directly to `MTKView`.
  - **Milestone:** Uncomment `Renderer::Init()` in `Application.mm`.
- [ ] **Day 40 — Phase 3 Visual Milestone: Interactive 3D Cube!**
  - **Files:** Test Layer in `KanVerse_App`
  - **Concepts:** Connecting `CameraController`, `Mesh::CreateChamferBox`, `Material`, and `Renderer::SubmitInstanced`.
  - **Milestone:** Orbit and zoom around a lit, textured 3D ChamferBox in your macOS Metal window at 120 FPS!

---

### Phase 4: Dear ImGui Backend, Docking, Offscreen Framebuffers & Editor Viewport (Days 41–48)
*Goal: Render our 3D scene into an offscreen texture and display it inside a dockable Blender/Unreal-style Dear ImGui Editor window.*

- [ ] **Day 41 — Offscreen HDR & SDR Framebuffers**
  - **Files:** `Renderer/Framebuffer/Framebuffer.hpp`, `Renderer/Framebuffer/Framebuffer.mm`
  - **Concepts:** `MTL::StorageModePrivate` textures (`MTL::TextureUsageRenderTarget | MTL::TextureUsageShaderRead`), `RGBA16Float` and `BGRA8Unorm_sRGB` attachments, `Depth32Float`, `Resize()`.
  - **Milestone:** Implement `MetalFramebuffer`.
- [ ] **Day 42 — Dear ImGui & ImGuizmo Vendor Integration**
  - **Files:** `Vendor/imgui/`, `Vendor/imguizmo/`
  - **Concepts:** Adding Dear ImGui (`docking` branch) with `imgui_impl_metal.mm` and `imgui_impl_osx.mm`, plus `ImGuizmo.h`/`.cpp` to the build target.
  - **Milestone:** Compile `Vendor/imgui` and `Vendor/imguizmo` with Objective-C++ enabled.
- [ ] **Day 43 — `ImGuiLayer` Lifecycle & Metal Render Pass Encoding**
  - **Files:** `Renderer/UI/ImGuiLayer.hpp`, `Renderer/UI/ImGuiLayer.mm`
  - **Concepts:** Initializing `ImGui::CreateContext()`, enabling `ImGuiConfigFlags_DockingEnable` and `ImGuiConfigFlags_ViewportsEnable`, encoding `ImGui_ImplMetal_RenderDrawData` inside a dedicated UI render pass on the swapchain.
  - **Milestone:** Push `ImGuiLayer` as an Overlay (`PushOverlay`) and render `ImGui::ShowDemoWindow()` over your 3D view.
- [ ] **Day 44 — Event Blocking & Input Routing Between ImGui and 3D Viewport**
  - **Files:** `Renderer/UI/ImGuiLayer.mm`, `Platform/Window/NativeWindow.mm`
  - **Concepts:** Forwarding `NSEvent`s to `ImGui_ImplOSX_HandleEvent`, checking `io.WantCaptureMouse` and `io.WantCaptureKeyboard` so clicking UI buttons doesn't move the 3D camera.
  - **Milestone:** Verify typing in an ImGui text box or dragging a slider blocks underlying gameplay events when hovered outside the 3D viewport.
- [ ] **Day 45 — Custom Studio Dark Theme, Fonts & Reusable UI Widgets**
  - **Files:** `Renderer/UI/ImGuiTheme.hpp`, `Renderer/UI/ImGuiTheme.cpp`, `Renderer/UI/UIWidgets.hpp`
  - **Concepts:** Designing a sleek Blender/Unreal-inspired charcoal dark theme, loading crisp Retina `.ttf` fonts (Inter / JetBrains Mono), and creating `UI::DrawVec3Control` (with red/green/blue X/Y/Z reset buttons).
  - **Milestone:** Apply `ImGuiTheme::ApplyDarkTheme()` on startup.
- [ ] **Day 46 — Master DockSpace & Main Menu Bar**
  - **Files:** `Layers/EditorLayer.hpp`, `Layers/EditorLayer.mm` (Part 1)
  - **Concepts:** Setting up a fullscreen persistent `ImGui::DockSpaceOverViewport`, top menu bar (`File -> New/Open/Save Scene`, `View -> Panels`, `Build -> Lighting`), and layout persistence (`imgui.ini`).
  - **Milestone:** Create `EditorLayer` with a dockable workspace.
- [ ] **Day 47 — Interactive 3D Editor Viewport Panel**
  - **Files:** `Layers/EditorLayer.mm` (Part 2)
  - **Concepts:** Rendering the 3D scene into an offscreen `Framebuffer`, passing `(ImTextureID)framebuffer->GetColorAttachment(0)` into `ImGui::Image()`, and dynamically calling `framebuffer->Resize(viewportSize.x, viewportSize.y)` when the ImGui panel is dragged/resized!
  - **Milestone:** Resize the docked 3D Viewport tab inside ImGui and watch the Metal render target resize smoothly at 120Hz.
- [ ] **Day 48 — Phase 4 Milestone: Blender/Maya-Style `EditorCamera`**
  - **Files:** `Editor/Viewport/EditorCamera.hpp`, `Editor/Viewport/EditorCamera.cpp`
  - **Concepts:** Free-fly WASD + Right-Click look and Alt + Left/Middle/Right-Click Maya-style Orbit/Pan/Zoom camera exclusively for level editing when the Viewport panel is focused/hovered.
  - **Milestone:** Fly around your 3D scene inside the docked Viewport panel!

---

### Phase 5: Generational ECS, Components & Asset Manager (Days 49–56)
*Goal: Build the data-oriented ECS and centralized Asset Manager that power both the Editor and Runtime.*

- [ ] **Day 49 — 64-Bit Generational Entity Handles**
  - **Files:** `Scene/EntityID.hpp`
  - **Concepts:** Bit-packing 32-bit slot index (`GetIndex`) and 32-bit generation counter (`GetGeneration`) into `uint64_t`.
  - **Milestone:** Implement `EntityID`, `NullEntity`, `GetIndex`, `GetGeneration`, and `CreateID`.
- [ ] **Day 50 — Paged Sparse Set Container (Part 1: Allocation & Emplace)**
  - **Files:** `Scene/SparseSet.hpp` (Part 1)
  - **Concepts:** Paged Sparse Sets (`PageSize = 4096`) allocating `uint32_t*` pages on demand (`AssurePage`), $O(1)$ `EntityID -> Dense Index` mapping.
  - **Milestone:** Implement `ISparseSet`, `SparseSet` constructors, `AssurePage`, `Contains`, and `Emplace`.
- [ ] **Day 51 — Paged Sparse Set Container (Part 2: Swap-and-Pop & Memory Guard)**
  - **Files:** `Scene/SparseSet.hpp` (Part 2)
  - **Concepts:** $O(1)$ Swap-and-Pop removal in `Remove()` to keep `m_DenseEntities` and `m_DenseComponents` 100% contiguous, `UpdateMemoryTracking()`.
  - **Milestone:** Implement `Get`, `Remove`, `Clear`, and `UpdateMemoryTracking`.
- [ ] **Day 52 — Generational Registry & Variadic Query Views**
  - **Files:** `Scene/Registry.hpp`
  - **Concepts:** Type-erased component pool map, slot recycling (`m_FreeList`), C++20 fold expressions to find the smallest pool (`FindSmallestPool`) in `Registry::View<Components...>`.
  - **Milestone:** Implement `Registry` and `GetView<...>()` queries.
- [ ] **Day 53 — High-Level Entity Wrapper & Core ECS Components**
  - **Files:** `Scene/Entity.hpp`, `Scene/Components.hpp`
  - **Concepts:** Facade `Entity` handle, `TagComponent`, `TransformComponent` (`T * Rz * Ry * Rx * S`), `MeshComponent`, `MaterialComponent`, `CameraComponent`, `DirectionalLightComponent`, `PointLightComponent`, `EnvironmentLightComponent`.
  - **Milestone:** Implement `Entity.hpp` and the rendering/transform components in `Components.hpp`.
- [ ] **Day 54 — Scene Container & Instanced Batch Rendering**
  - **Files:** `Scene/Scene.hpp`, `Scene/Scene.mm` (Part 1: Rendering & Lifecycle)
  - **Concepts:** `CreateEntity`, `DestroyEntity`, grouping entities by `std::pair<Mesh*, Material*>` into `opaqueBatches` and `transparentBatches` inside `OnRenderRuntime` and `OnRenderEditor`.
  - **Milestone:** Render an entire `Scene` of ECS entities into the Editor Viewport!
- [ ] **Day 55 — Global Asset Manager & Texture/Material Cache**
  - **Files:** `Renderer/Asset/AssetManager.hpp`, `Renderer/Asset/AssetManager.cpp`
  - **Concepts:** Deduplicated registry for `Shader`, `Material`, `Mesh`, and `Texture2D` (`LoadTexture` with cache lookup), preventing redundant disk reads and VRAM allocations.
  - **Milestone:** Implement `AssetManager` and verify loading the same texture 50 times allocates only 1 `MTL::Texture`.
- [ ] **Day 56 — PBR Material Factory & Standardized Entity Factory**
  - **Files:** `Factories/MaterialFactory.hpp`, `Factories/MaterialFactory.cpp`, `Factories/EntityFactory.hpp`, `Factories/EntityFactory.cpp`
  - **Concepts:** Auto-scanning PBR folders (`albedo.png`, `normal.png`, `roughness.png`, `metallic.png`, `ao.png`, `height.png`), one-line `EntityFactory::SpawnMesh`.
  - **Milestone:** Implement `MaterialFactory` and `EntityFactory`.

---

### Phase 6: KanVerse Studio Editor — Hierarchy, Inspector, Content Browser, Gizmos & Picking (Days 57–70)
*Goal: Build the complete visual authoring suite so you can create, inspect, transform, and paint entities just like Blender or Unreal Engine.*

- [ ] **Day 57 — Scene Hierarchy Panel (Entity Tree & Search)**
  - **Files:** `Editor/Panels/SceneHierarchyPanel.hpp`, `Editor/Panels/SceneHierarchyPanel.cpp` (Part 1)
  - **Concepts:** Iterating `Registry` entities, rendering `ImGui::TreeNodeEx` rows with selection highlighting, live search filter bar by `TagComponent`, and tracking `m_SelectedEntity`.
  - **Milestone:** View all live scene entities in the docked `Scene Hierarchy` panel and click to select them.
- [ ] **Day 58 — Scene Hierarchy Context Menus (Create, Duplicate, Delete)**
  - **Files:** `Editor/Panels/SceneHierarchyPanel.cpp` (Part 2)
  - **Concepts:** Right-click blank space (`ImGui::BeginPopupContextWindow`) to spawn primitives (`Empty Entity`, `Chamfer Box`, `Capsule`, `Point Light`, `Directional Light`, `Camera`), right-click entity (`BeginPopupContextItem`) to `Duplicate` (`Cmd+D`) or `Delete` (`Backspace`).
  - **Milestone:** Spawn and delete 3D objects in real-time using right-click context menus!
- [ ] **Day 59 — Properties / Inspector Panel (Tag & Transform Editor)**
  - **Files:** `Editor/Panels/InspectorPanel.hpp`, `Editor/Panels/InspectorPanel.cpp` (Part 1)
  - **Concepts:** Displaying the selected entity's `TagComponent` text input, `Add Component` button popup, and `TransformComponent` (`Translation`, Euler `Rotation` in degrees, `Scale`) using `UI::DrawVec3Control`.
  - **Milestone:** Drag X/Y/Z sliders in the Inspector and watch the 3D object move/rotate/scale in the Viewport.
- [ ] **Day 60 — Inspector Panel (Camera, Light & Environment Editors)**
  - **Files:** `Editor/Panels/InspectorPanel.cpp` (Part 2)
  - **Concepts:** Component accordions (`DrawComponent<T>`) with a gear icon to remove components; color pickers (`ImGui::ColorEdit3`), intensity sliders, and radius/attenuation controls for `DirectionalLightComponent`, `PointLightComponent`, and `CameraComponent`.
  - **Milestone:** Adjust point light color and intensity live in the Inspector.
- [ ] **Day 61 — Inspector Panel (Mesh & PBR Material Editor)**
  - **Files:** `Editor/Panels/InspectorPanel.cpp` (Part 3), `Editor/Panels/MaterialEditorPanel.hpp`, `Editor/Panels/MaterialEditorPanel.cpp`
  - **Concepts:** Editing `MaterialComponent` properties (`BaseColor`, `EmissiveColor` HDR multiplier, `Roughness`, `Metallic`, `TilingFactor`), displaying 64x64 thumbnail previews of bound `Albedo`, `Normal`, `Roughness`, and `Metallic` maps.
  - **Milestone:** Tweak roughness/metallic sliders and emissive glow on any selected entity.
- [ ] **Day 62 — Content Browser Panel (Directory Tree & Grid Thumbnails)**
  - **Files:** `Editor/Panels/ContentBrowserPanel.hpp`, `Editor/Panels/ContentBrowserPanel.cpp` (Part 1)
  - **Concepts:** Navigating `KanVerse_App/Assets/` using `std::filesystem::directory_iterator`, breadcrumb navigation bar (`Assets > Textures > PBR > Wall`), Back/Forward buttons, and responsive grid layout (`ImGui::Columns`) with folder/file icons and texture thumbnails.
  - **Milestone:** Browse all folders, shaders, scenes, and PBR texture directories inside the docked `Content Browser`.
- [ ] **Day 63 — Drag-and-Drop Asset Workflow (Content Browser -> Viewport & Inspector)**
  - **Files:** `Editor/Panels/ContentBrowserPanel.cpp` (Part 2), `Editor/Panels/InspectorPanel.cpp`, `Layers/EditorLayer.mm`
  - **Concepts:** `ImGui::BeginDragDropSource` on PBR folders, textures, and `.kvscene` files in the Content Browser; `ImGui::BeginDragDropTarget` on Inspector texture slots and the 3D Viewport to drop materials directly onto meshes!
  - **Milestone:** Drag a PBR material folder from the Content Browser onto a 3D cube in the Viewport to skin it instantly.
- [ ] **Day 64 — 3D Transform Gizmos (`ImGuizmo`: Translate, Rotate, Scale)**
  - **Files:** `Editor/Gizmos/GizmoController.hpp`, `Editor/Gizmos/GizmoController.cpp`
  - **Concepts:** Integrating `ImGuizmo::Manipulate` over the 3D Viewport rect, decomposing the manipulated $4 \times 4$ matrix back into `Translation`, `Rotation`, and `Scale`, hotkeys (`W` = Translate, `E` = Rotate, `R` = Scale, `Q` = Select), Local vs World space toggle, and grid snapping (`Cmd` held).
  - **Milestone:** Grab the 3D X/Y/Z arrows and rotation rings in the Viewport to move and rotate entities like Blender!
- [ ] **Day 65 — GPU Mouse Picking & Entity Selection Outline**
  - **Files:** `KanVerse_App/Shaders/MousePicking.metal`, `Editor/Viewport/ViewportPicker.hpp`, `Editor/Viewport/ViewportPicker.mm`
  - **Concepts:** Rendering entity IDs (`R32Uint` texture attachment or fast ray-OBB intersection) under the mouse cursor on Left-Click in the 3D Viewport so clicking any 3D mesh selects it in the Hierarchy and attaches the 3D Gizmo.
  - **Milestone:** Click any 3D object in the Viewport to select it and see its transform gizmo appear.
- [ ] **Day 66 — In-Engine Console & Log Filter Panel**
  - **Files:** `Editor/Panels/ConsolePanel.hpp`, `Editor/Panels/ConsolePanel.cpp`, `Core/Diagnostics/Log.cpp`
  - **Concepts:** Adding a 4th ring-buffer sink to `Log::Dispatch` that feeds `ConsolePanel`; rendering color-coded log messages (`Trace`, `Info`, `Warn`, `Error`) with severity filter checkboxes, `Clear` button, and auto-scroll.
  - **Milestone:** View all engine logs directly inside the docked `Console` panel at the bottom of KanVerse Studio.
- [ ] **Day 67 — Real-Time Hardware Profiler & Memory Telemetry Panel**
  - **Files:** `Editor/Panels/ProfilerPanel.hpp`, `Editor/Panels/ProfilerPanel.cpp`
  - **Concepts:** Visualizing FPS, frame delta time graph (`ImGui::PlotLines`), Apple Silicon unified memory stats from `MemoryTracker::GetStats()` (`CurrentUsage`, `PeakUsage`, `AllocationCount`), and draw call / instance counts.
  - **Milestone:** Dock the `Profiler` panel and monitor real-time RAM/VRAM usage and frame pacing.
- [ ] **Day 68 — Command Pattern Undo / Redo System (`Cmd+Z` / `Cmd+Shift+Z`)**
  - **Files:** `Editor/Commands/EditorCommand.hpp`, `Editor/Commands/UndoRedoStack.hpp`
  - **Concepts:** Recording `TransformCommand`, `PropertyEditCommand`, and `SpawnDeleteCommand` onto a bounded history stack when gizmo drags or Inspector edits finish (`ImGui::IsItemDeactivatedAfterEdit()`).
  - **Milestone:** Move an object with the 3D gizmo, press `Cmd+Z`, and watch it snap back to its previous position.

---

### Phase 7: Physics Engine, Colliders, PBD Solver & Visual Debug Renderer (Days 69–76)
*Goal: Build the collision and soft-body physics engine and visualize wireframe colliders inside KanVerse Studio.*

- [ ] **Day 69 — Axis-Aligned Bounding Boxes (AABB) & Raycasting**
  - **Files:** `Physics/BoundingBox.hpp`, `Physics/BoundingBox.cpp` (Part 1)
  - **Concepts:** `AABB` (`Min`, `Max`), `simd::clamp` for `ClosestPoint`, Slab Method for `IntersectsRay` (`tMin`, `tMax`, hit normals).
  - **Milestone:** Implement `Ray`, `RaycastHit`, and all `AABB` methods.
- [ ] **Day 70 — Oriented Bounding Boxes (OBB) & 15-Axis SAT**
  - **Files:** `Physics/BoundingBox.cpp` (Part 2)
  - **Concepts:** `OBB` (`Center`, `HalfExtents`, `Axes[3]`), local-space ray transformation, 15-axis Separating Axis Theorem (SAT) intersection test.
  - **Milestone:** Implement `OBB::FromTransform`, `ClosestPoint`, `Contains`, `Intersects`, and `IntersectsRay`.
- [ ] **Day 71 — Capsule & Cylinder Narrowphase Contact Generation**
  - **Files:** `Physics/ColliderPrimitives.hpp`, `Physics/ColliderPrimitives.cpp`
  - **Concepts:** Swept-sphere `CapsuleCollider`, closest-point segment projection, iterative segment-to-OBB contact generation (`ContactManifold`), `CylinderCollider`.
  - **Milestone:** Implement `ContactManifold`, `CapsuleCollider`, and `CylinderCollider`.
- [ ] **Day 72 — Position-Based Dynamics (PBD) Distance & Cluster Solver**
  - **Files:** `Physics/PBDSolver.hpp`, `Physics/PBDSolver.cpp`
  - **Concepts:** `PBDParticle`, Gauss-Seidel `PBDDistanceConstraint` projection, volumetric shape-matching `PBDCluster` (`RestPositions`, `Stiffness`), floor collision resolution.
  - **Milestone:** Implement `PBDSolver`.
- [ ] **Day 73 — Physics & Gameplay ECS Components + Inspector Integration**
  - **Files:** `Scene/Components.hpp`, `Editor/Panels/InspectorPanel.cpp`
  - **Concepts:** Adding `ColliderComponent` (`Box`, `Capsule`, `Cylinder`), `CharacterControllerComponent`, `DebrisComponent`, `FracturableComponent`, `DoorComponent`, and `AIControllerComponent` to `Components.hpp` and exposing their fields in `InspectorPanel`.
  - **Milestone:** Add and configure a `ColliderComponent` or `FracturableComponent` on any entity using the Inspector!
- [ ] **Day 74 — Editor Wireframe Collider & Light Radius Visualizer**
  - **Files:** `Renderer/Renderer/Renderer.mm`, `Layers/EditorLayer.mm`
  - **Concepts:** Using `PipelineSpecification::Wireframe = true` to draw green wireframe boxes/capsules around entities with `ColliderComponent` and yellow wireframe spheres for `PointLightComponent` radii when `Show Colliders` is toggled in the Viewport toolbar.
  - **Milestone:** Toggle `Show Colliders` in the Editor and visually align your collision bounds with your meshes!

---

### Phase 8: Advanced PBR, Shadows, IBL, Compute Bloom, ACES Tonemapping & Atmosphere (Days 75–86)
*Goal: Upgrade the Renderer to AAA visual fidelity with PCF Shadow Mapping, HDR Skyboxes, Image-Based Lighting, Dual-Kawase Compute Bloom, and live World Settings controls.*

- [ ] **Day 75 — Shadow Depth Shader & Orthographic Shadow Pass**
  - **Files:** `KanVerse_App/Shaders/ShadowDepth.metal`, `Renderer/Renderer/Renderer.mm`, `Scene/Scene.mm`
  - **Concepts:** Instanced depth-only vertex shader (`shadow_vertex_main`), 2048x2048 `Depth32Float` shadow framebuffer, `BeginShadowPass`, and `Scene::OnRenderShadowPass` (skipping transparent & emissive bulb meshes).
  - **Milestone:** Render the scene's directional shadow map every frame and preview the depth texture in a debug ImGui window!
- [ ] **Day 76 — Equirectangular-to-Cubemap & Irradiance Convolution Shaders**
  - **Files:** `KanVerse_App/Shaders/Equirectangular.metal`, `KanVerse_App/Shaders/Irradiance.metal`, `KanVerse_App/Shaders/Skybox.metal`
  - **Concepts:** Spherical UV mapping, 6-slice `texture2d_array` compute writes, hemisphere Riemann-sum diffuse irradiance convolution, and vertex-less fullscreen skybox rendering.
  - **Milestone:** Compile `Equirectangular.metal`, `Irradiance.metal`, and `Skybox.metal`.
- [ ] **Day 77 — Cubemap Textures (Procedural HDR Sky, Equirectangular `.hdr` & Irradiance)**
  - **Files:** `Renderer/Textures/TextureCube.hpp`, `Renderer/Textures/TextureCube.mm`
  - **Concepts:** Analytical HDR sky synthesis (`__fp16`), loading `.hdr` files via `stbi_loadf`, dispatching GPU compute convolution, and generating mipmaps for roughness-based specular reflections.
  - **Milestone:** Implement `TextureCube`.
- [ ] **Day 78 — Full Cook-Torrance PBR Shader (Screen-Space TBN, PCF Shadows, 64 Point Lights & IBL)**
  - **Files:** `KanVerse_App/Shaders/TexturedMaterial.metal`
  - **Concepts:** Cotangent-frame TBN normal mapping (`dfdx`/`dfdy`), GGX/Smith/Fresnel BRDF, 3x3 PCF shadow sampling with smooth frustum fade, 64-light `GPUPointLight` loop, and horizon-stabilized diffuse + specular IBL.
  - **Milestone:** Upgrade `TexturedMaterial.metal` to full Cook-Torrance PBR + IBL + Shadows.
- [ ] **Day 79 — Dual-Kawase Multi-Mip Compute Bloom Pass**
  - **Files:** `KanVerse_App/Shaders/Bloom.metal`, `Renderer/PostProcess/BloomPass.hpp`, `Renderer/PostProcess/BloomPass.cpp`
  - **Concepts:** Soft-knee luminance threshold prefiltering, 13-tap anti-flicker downsample kernel (`bloom_downsample`), 9-tap 3x3 tent filter upsample kernel (`bloom_upsample`) across a 4-mip `RGBA16Float` pyramid.
  - **Milestone:** Implement `Bloom.metal` and `BloomPass`.
- [ ] **Day 80 — ACES Filmic Tonemapping & Post-Process Composite**
  - **Files:** `KanVerse_App/Shaders/Tonemap.metal`, `Renderer/Renderer/Renderer.mm`
  - **Concepts:** Compositing HDR scene radiance with the compute bloom texture (`(hdrColor * exposure) + (bloomColor * bloomIntensity)`) and mapping to SDR via the Narkowicz ACES Filmic curve.
  - **Milestone:** Connect `HDRFramebuffer` -> `BloomPass` -> `TonemapPipeline` -> Editor Viewport Texture!
- [ ] **Day 81 — Dynamic Atmosphere & Time-of-Day System**
  - **Files:** `World/Environment/AtmosphereSystem.hpp`, `World/Environment/AtmosphereSystem.cpp`
  - **Concepts:** Presets (`Sunrise`, `Noon`, `Sunset`, `Dusk`, `Night`), continuous day/night cycle (`EnableDynamicCycle`), and `AutoFitShadowBounds`.
  - **Milestone:** Implement `AtmosphereSystem`.
- [ ] **Day 82 — World Settings & Post-Process Editor Panel**
  - **Files:** `Editor/Panels/WorldSettingsPanel.hpp`, `Editor/Panels/WorldSettingsPanel.cpp`
  - **Concepts:** Dockable panel to switch `TimeOfDay` presets, scrub the Day/Night cycle slider, toggle `IBL` and `EnvironmentMode` (`CrispHDR`, `Irradiance`, `Procedural`), and tune `Bloom Threshold`, `SoftKnee`, `Bloom Intensity`, and `Camera Exposure` in real time.
  - **Milestone:** Scrub the sun across the sky and tune HDR bloom glow live from the `World Settings` panel!

---

### Phase 9: Scene Serialization, Prefabs, Undo/Redo & Play-In-Editor (PIE) State Machine (Days 83–90)
*Goal: Save/load levels to disk (`.kvscene`) and implement the Play / Pause / Stop toolbar so you can playtest your game directly inside the Editor!*

- [ ] **Day 83 — Scene Serializer (`yaml-cpp` / JSON Entity & Component Writer)**
  - **Files:** `Core/Serialization/SceneSerializer.hpp`, `Core/Serialization/SceneSerializer.cpp` (Part 1)
  - **Concepts:** Iterating `Registry` entities and serializing `EntityID`, `TagComponent`, `TransformComponent`, `MeshComponent`, `MaterialComponent`, lights, colliders, doors, and AI settings to human-readable `.kvscene` files.
  - **Milestone:** Press `Cmd+S` in the Editor to save your current scene to `Assets/Scenes/Level1.kvscene`.
- [ ] **Day 84 — Scene Deserializer & Drag-and-Drop Scene Loading**
  - **Files:** `Core/Serialization/SceneSerializer.cpp` (Part 2), `World/SceneManager.hpp`, `World/SceneManager.cpp`
  - **Concepts:** Parsing `.kvscene` files, reconstructing entities, re-binding cached materials/meshes via `AssetManager`, and double-clicking `.kvscene` files in the Content Browser to load them.
  - **Milestone:** Implement `SceneSerializer::Deserialize` and `SceneManager`.
- [ ] **Day 85 — Deep Scene Cloning (`Scene::Copy`) for Safe Play-In-Editor**
  - **Files:** `Scene/Scene.mm`
  - **Concepts:** Copying all entities and component pools from `m_EditorScene` into a transient `m_RuntimeScene` so when the player shatters walls, opens doors, or defeats enemies during Play mode, stopping the game restores the untouched `m_EditorScene`!
  - **Milestone:** Implement `Scene::Copy(const std::shared_ptr<Scene>& source)`.
- [ ] **Day 86 — Play / Simulate / Pause / Stop Viewport Toolbar (PIE State Machine)**
  - **Files:** `Layers/EditorLayer.hpp`, `Layers/EditorLayer.mm`
  - **Concepts:** `SceneState` enum (`Edit`, `Play`, `Simulate`, `Pause`):
    - **Edit Mode:** Uses `EditorCamera`, renders gizmos, physics paused.
    - **Play Mode:** Clones scene, possesses `PlayerRig` with `CameraRigSystem`, runs full gameplay & physics.
    - **Simulate Mode:** Runs physics & AI while keeping free-fly `EditorCamera` active so you can inspect simulations from any angle!
  - **Milestone:** Click **Play** in the top toolbar to drop into your character, shatter a wall, and click **Stop** to see the wall magically restored!

---

### Phase 10: Procedural Architectural Builders & In-Editor Level Design Tools (Days 87–96)
*Goal: Build parametric architectural generators and expose them in an interactive Level Builder Panel inside KanVerse Studio.*

- [ ] **Day 87 — Ground, Floor & Flat/Pitched Roof Builders**
  - **Files:** `World/Builders/GroundBuilder.hpp`/`.cpp`, `World/Builders/FloorBuilder.hpp`/`.cpp`, `World/Builders/RoofBuilder.hpp`/`.cpp`
  - **Concepts:** World-scale UV tiling (`tilingFactor = scale`), flat slabs vs pitched roof trigonometry (`cos(PitchAngle)`, `tan(PitchAngle)`).
  - **Milestone:** Implement `GroundBuilder`, `FloorBuilder`, and `RoofBuilder`.
- [ ] **Day 88 — Single-Flight & L-Shaped Staircase Builder**
  - **Files:** `World/Builders/StairsBuilder.hpp`, `World/Builders/StairsBuilder.cpp`
  - **Concepts:** Parametric single-flight (`Build`) and L-shaped multi-flight staircases (`BuildLShaped`) with an exact square landing connector.
  - **Milestone:** Implement `StairsBuilder`.
- [ ] **Day 89 — Parametric Wall, Door & Transparent Window Builder**
  - **Files:** `World/Builders/WallBuilder.hpp`, `World/Builders/WallBuilder.cpp`
  - **Concepts:** CSG-like wall segmentation from an `OpeningSequence` (`{Window, Door, Window}`), auto-fitting pillars, headers, sills, `DoorComponent` leaves, and transparent `WindowGlass` (`alpha = 0.35f`).
  - **Milestone:** Implement `WallBuilder`.
- [ ] **Day 90 — Indoor Sconce, Ceiling Light & Outdoor Street Lamp Builder**
  - **Files:** `World/Builders/LightBuilder.hpp`, `World/Builders/LightBuilder.cpp`
  - **Concepts:** Multi-part light fixtures (`BuildPointFixture` housing + emissive bulb + `PointLightComponent`; `BuildStreetLamp` base, pole, cantilever arm, shade, and warm point light).
  - **Milestone:** Implement `LightBuilder`.
- [ ] **Day 91 — Destructible Floor & Brick Wall Builders**
  - **Files:** `World/Builders/DestructibleFloorBuilder.hpp`/`.cpp`, `World/Builders/DestructibleWallBuilder.hpp`/`.cpp`
  - **Concepts:** Subdividing floors and running-bond brick walls into grids of `FracturableComponent` blocks (`PersistDebris`).
  - **Milestone:** Implement `DestructibleFloorBuilder` and `DestructibleWallBuilder`.
- [ ] **Day 92 — Articulated Enemy Rig Factory**
  - **Files:** `World/Builders/EnemyFactory.hpp`, `World/Builders/EnemyFactory.cpp`
  - **Concepts:** Spawning the 6-part `EnemyRig` (`Torso`, `Head`, glowing HDR `LeftEye`/`RightEye`, `LeftLeg`/`RightLeg`) with `AIControllerComponent`.
  - **Milestone:** Implement `EnemyFactory`.
- [ ] **Day 93 — Two-Story House & Master Level 1 Builders**
  - **Files:** `World/Architecture/House_1_Builder.hpp`/`.cpp`, `World/LevelBuilder.hpp`/`.cpp`
  - **Concepts:** Combining walls, floors, interior stairs, stairwell roof cutouts, 32 sconces/ceiling lights, and street lamps into `Crash_House` and Level 1 ("The Outpost").
  - **Milestone:** Implement `House_1_Builder` and `LevelBuilder`.
- [ ] **Day 94 — In-Editor Procedural Architecture Generator Panel (`LevelBuilderPanel`)**
  - **Files:** `Editor/Panels/LevelBuilderPanel.hpp`, `Editor/Panels/LevelBuilderPanel.cpp`
  - **Concepts:** A dedicated ImGui tool panel where you configure `WallSpecification`, `StairSpecification`, `LStairSpecification`, `RoofSpecification`, `DestructibleWallSpecification`, or `StreetLampSpecification` via UI sliders and click **"Spawn in Scene"** at the 3D cursor!
  - **Milestone:** Build custom rooms, staircases, street lamps, and destructible walls interactively from the `Level Builder` panel without recompiling C++!

---

### Phase 11: Gameplay Systems — Player Rig, SpringArm Camera, Doors, AI, Combat & Destruction (Days 95–105)
*Goal: Complete all character locomotion, destruction, AI, and combat mechanics, then author and ship your complete game!*

- [ ] **Day 95 — Hardware-to-Intent Input Mapping & Damped SpringArm Rig**
  - **Files:** `Systems/Player/PlayerInputIntent.hpp`, `Systems/Player/InputBridgeSystem.hpp`/`.cpp`, `Renderer/Camera/SpringArm.hpp`/`.cpp`, `Systems/Player/CameraRigSystem.hpp`/`.cpp`
  - **Concepts:** Rising-edge jump/slam detection, exponential camera lag damping, and raycast wall occlusion pull-in (`CheckOcclusion`).
  - **Milestone:** Implement `InputBridgeSystem`, `SpringArm`, and `CameraRigSystem`.
- [ ] **Day 96 — Kinematic Player Controller (Locomotion, Double-Jump, Ground-Slam & Spring Squash)**
  - **Files:** `Physics/PlayerControllerSystem.hpp`, `Physics/PlayerControllerSystem.cpp`
  - **Concepts:** View-relative movement, sequential Y-before-XZ step-climbing collision resolution, turn banking (`RollAngle`), and Hooke's Law squash-and-stretch spring physics.
  - **Milestone:** Implement `PlayerControllerSystem`.
- [ ] **Day 97 — Ground-Slam Shockwave Destruction & Floor-Aware Debris Physics**
  - **Files:** `Scene/Scene.mm` (`ShatterBlock`, `UpdateDebris`, `OnUpdate`), `KanVerse_App/Shaders/DebrisSimulation.metal`
  - **Concepts:** Spawning $2 \times 2 \times 2$ tumbling `DebrisShard`s when `TriggerSlamImpact` hits `FracturableComponent` blocks within `blastRadius`, bounce restitution, and `AudioSystem::Play(SoundEffect::Shatter)`.
  - **Milestone:** Complete destruction and debris simulation in `Scene.mm`.
- [ ] **Day 98 — Interactive Hinged & Sliding Door System**
  - **Files:** `Systems/Gameplay/DoorSystem.hpp`, `Systems/Gameplay/DoorSystem.cpp`
  - **Concepts:** Proximity-triggered doors (`InteractionRadius`), hinge pivot trigonometry (`DoorType::Angular`) and linear translation (`DoorType::Sliding`).
  - **Milestone:** Walk up to house doors in Play mode and watch them slide/swing open automatically.
- [ ] **Day 99 — Autonomous Enemy AI & Hierarchical Limb Synchronizer**
  - **Files:** `Systems/AI/EnemyControllerSystem.hpp`, `Systems/AI/EnemyControllerSystem.cpp`
  - **Concepts:** AI state machine (`Idle`, `Walk`, `Sprint`, `Attack`), platform landing, and synchronizing child `Enemy_*` head/eye/leg entities around the torso.
  - **Milestone:** Implement `EnemyControllerSystem`.
- [ ] **Day 100 — Standalone Game Layer (`KanVerseLayer`) & Procedural Hand Bobbing**
  - **Files:** `Layers/KanVerseLayer.hpp`, `Layers/KanVerseLayer.mm`, `KanVerse_Engine/KanVerse_Headers.h`
  - **Concepts:** Sine-wave procedural hand bobbing (`std::sin(m_AnimationTimer * bobFrequency)`), visor alignment, and switching between `EditorLayer` (Development build) and `KanVerseLayer` (Standalone shipping game build).
  - **Milestone:** Complete `KanVerseLayer` and test both Editor Play mode and Standalone Game mode!
- [ ] **Day 101–105 — Game Design, Custom Levels & Xcode Metal System Trace Audit**
  - **Files:** `Assets/Scenes/`, `Systems/Gameplay/Combat/`
  - **Concepts:** Using **KanVerse Studio** to design multi-floor platforming arenas, placing point lights, destructible floors, and enemy patrols, serializing the campaign scenes, and profiling GPU pass timings in Xcode Instruments.
  - **Milestone:** Ship your custom 3D game built inside your own Apple Silicon Game Engine and Studio Editor!

---

## 📁 Complete Directory Tree Hierarchy

Folders marked with `[FUTURE]` are pre-allocated for upcoming engine features (UI, Serialization, Scripting, Compute Particles, Animation).

```text
KanVerse/
├── KanVerse_Engine/
│   ├── KanVerse_Headers.h
│   ├── PrefixHeader.pch
│   ├── Audio/
│   │   ├── AudioSystem.hpp
│   │   └── AudioSystem.mm
│   ├── Core/
│   │   ├── Application/
│   │   │   ├── Application.hpp
│   │   │   └── Application.mm
│   │   ├── Diagnostics/
│   │   │   ├── Assert.hpp
│   │   │   ├── Log.hpp
│   │   │   └── Log.cpp
│   │   ├── Layers/
│   │   │   ├── Layer.hpp
│   │   │   ├── LayerStack.hpp
│   │   │   └── LayerStack.cpp
│   │   ├── Math/
│   │   │   └── Math.hpp
│   │   ├── Memory/
│   │   │   └── MemoryTracking.hpp
│   │   ├── Time/
│   │   │   ├── Clock.hpp
│   │   │   ├── Clock.cpp
│   │   │   └── TimeStep.hpp
│   │   ├── Threading/                  # [FUTURE] Job System & Task Graph
│   │   └── Serialization/              # [FUTURE] YAML/Binary Scene Archivers
│   ├── Physics/
│   │   ├── BoundingBox.hpp
│   │   ├── BoundingBox.cpp
│   │   ├── ColliderPrimitives.hpp
│   │   ├── ColliderPrimitives.cpp
│   │   ├── PBDSolver.hpp
│   │   ├── PBDSolver.cpp
│   │   ├── PlayerControllerSystem.hpp
│   │   ├── PlayerControllerSystem.cpp
│   │   └── Broadphase/                 # [FUTURE] BVH / Spatial Hashing
│   ├── Platform/
│   │   ├── Events/
│   │   │   ├── Event.hpp
│   │   │   ├── KeyEvents.hpp
│   │   │   ├── MouseEvents.hpp
│   │   │   └── WindowEvents.hpp
│   │   ├── Input/
│   │   │   ├── GameControllerBridge.hpp
│   │   │   ├── GameControllerBridge.mm
│   │   │   └── KeyCodes.hpp
│   │   └── Window/
│   │       ├── Window.hpp
│   │       ├── NativeWindow.hpp
│   │       └── NativeWindow.mm
│   ├── Renderer/
│   │   ├── Asset/
│   │   │   ├── AssetManager.hpp
│   │   │   └── AssetManager.cpp
│   │   ├── Buffers/
│   │   │   ├── BufferLayout.hpp
│   │   │   ├── IndexBuffer.hpp
│   │   │   ├── IndexBuffer.mm
│   │   │   ├── VertexBuffer.hpp
│   │   │   └── VertexBuffer.mm
│   │   ├── Camera/
│   │   │   ├── Camera.hpp
│   │   │   ├── CameraController.hpp
│   │   │   ├── CameraController.mm
│   │   │   ├── PerspectiveCamera.hpp
│   │   │   ├── PerspectiveCamera.cpp
│   │   │   ├── SpringArm.hpp
│   │   │   └── SpringArm.cpp
│   │   ├── Context/
│   │   │   ├── RenderContext.hpp
│   │   │   └── RenderContext.mm
│   │   ├── Framebuffer/
│   │   │   ├── Framebuffer.hpp
│   │   │   └── Framebuffer.mm
│   │   ├── Geometry/
│   │   │   ├── Mesh.hpp
│   │   │   ├── Mesh.cpp
│   │   │   └── SubMesh.hpp
│   │   ├── Lighting/
│   │   │   ├── DirectionalLight.hpp
│   │   │   ├── EnvironmentLighting.hpp
│   │   │   ├── Light.hpp
│   │   │   ├── PointLight.hpp
│   │   │   └── PointLight.cpp
│   │   ├── Materials/
│   │   │   ├── Material.hpp
│   │   │   └── Material.mm
│   │   ├── Pipeline/
│   │   │   ├── FrameSync.hpp
│   │   │   ├── FrameSync.mm
│   │   │   ├── Pipeline.hpp
│   │   │   ├── Pipeline.mm
│   │   │   ├── Shader.hpp
│   │   │   └── Shader.mm
│   │   ├── PostProcess/
│   │   │   ├── BloomPass.hpp
│   │   │   ├── BloomPass.cpp
│   │   │   └── SSAO/                   # [FUTURE] Screen-Space Ambient Occlusion
│   │   ├── Procedural/
│   │   │   ├── Capsule.hpp
│   │   │   ├── Capsule.cpp
│   │   │   ├── ChamferBox.hpp
│   │   │   └── ChamferBox.cpp
│   │   ├── Renderer/
│   │   │   ├── RenderCommand.hpp
│   │   │   ├── RenderCommand.mm
│   │   │   ├── Renderer.hpp
│   │   │   └── Renderer.mm
│   │   ├── Textures/
│   │   │   ├── Texture.hpp
│   │   │   ├── Texture2D.hpp
│   │   │   ├── Texture2D.mm
│   │   │   ├── TextureCube.hpp
│   │   │   └── TextureCube.mm
│   │   ├── UI/                         # [FUTURE] ImGui / Metal HUD Overlay
│   │   └── Animation/                  # [FUTURE] Skeletal Rigging & Skinning
│   └── Scene/
│       ├── Components.hpp
│       ├── Entity.hpp
│       ├── EntityID.hpp
│       ├── Registry.hpp
│       ├── Scene.hpp
│       ├── Scene.mm
│       ├── SparseSet.hpp
│       └── Prefabs/                    # [FUTURE] Reusable Entity Templates
├── KanVerse_App/
│   ├── PrefixHeader.pch
│   ├── Assets/
│   │   ├── Audio/                      # [FUTURE] .wav / .caf Sound Cues
│   │   ├── Meshes/                     # [FUTURE] .obj / .gltf 3D Models
│   │   ├── Scenes/                     # [FUTURE] Saved Level Files
│   │   └── Textures/
│   │       ├── HDR/
│   │       └── PBR/
│   │           ├── Floor/
│   │           ├── Ground/
│   │           ├── Metal/
│   │           ├── Roof/
│   │           ├── Wall/
│   │           └── Wood/
│   ├── Factories/
│   │   ├── EntityFactory.hpp
│   │   ├── EntityFactory.cpp
│   │   ├── MaterialFactory.hpp
│   │   └── MaterialFactory.cpp
│   ├── Layers/
│   │   ├── KanVerseLayer.hpp
│   │   ├── KanVerseLayer.mm
│   │   └── DebugOverlayLayer/          # [FUTURE] Telemetry & Profiler UI
│   ├── Shaders/
│   │   ├── Bloom.metal
│   │   ├── DebrisSimulation.metal
│   │   ├── Equirectangular.metal
│   │   ├── Irradiance.metal
│   │   ├── ShadowDepth.metal
│   │   ├── Skybox.metal
│   │   ├── TexturedMaterial.metal
│   │   └── Tonemap.metal
│   ├── Source/
│   │   ├── EntryPoint.mm
│   │   ├── KanVerseApp.mm
│   │   └── MetalImpl.cpp
│   ├── Systems/
│   │   ├── AI/
│   │   │   ├── EnemyControllerSystem.hpp
│   │   │   └── EnemyControllerSystem.cpp
│   │   ├── Gameplay/
│   │   │   ├── DoorSystem.hpp
│   │   │   ├── DoorSystem.cpp
│   │   │   ├── Combat/                 # [FUTURE] Health, Hitboxes & Projectiles
│   │   │   └── Inventory/              # [FUTURE] Keys, Collectibles & Triggers
│   │   └── Player/
│   │       ├── CameraRigSystem.hpp
│   │       ├── CameraRigSystem.cpp
│   │       ├── InputBridgeSystem.hpp
│   │       ├── InputBridgeSystem.cpp
│   │       └── PlayerInputIntent.hpp
│   └── World/
│       ├── LevelBuilder.hpp
│       ├── LevelBuilder.cpp
│       ├── SceneManager.hpp
│       ├── SceneManager.cpp
│       ├── Architecture/
│       │   ├── House_1_Builder.hpp
│       │   └── House_1_Builder.cpp
│       ├── Builders/
│       │   ├── DestructibleFloorBuilder.hpp
│       │   ├── DestructibleFloorBuilder.cpp
│       │   ├── DestructibleWallBuilder.hpp
│       │   ├── DestructibleWallBuilder.cpp
│       │   ├── EnemyFactory.hpp
│       │   ├── EnemyFactory.cpp
│       │   ├── FloorBuilder.hpp
│       │   ├── FloorBuilder.cpp
│       │   ├── GroundBuilder.hpp
│       │   ├── GroundBuilder.cpp
│       │   ├── LightBuilder.hpp
│       │   ├── LightBuilder.cpp
│       │   ├── RoofBuilder.hpp
│       │   ├── RoofBuilder.cpp
│       │   ├── StairsBuilder.hpp
│       │   ├── StairsBuilder.cpp
│       │   ├── WallBuilder.hpp
│       │   └── WallBuilder.cpp
│       └── Environment/
│           ├── AtmosphereSystem.hpp
│           └── AtmosphereSystem.cpp
└── Vendor/                             # [FUTURE] Third-Party Dependencies
    ├── metal-cpp/
    └── stb_image/
```

---

## ⚡ macOS Terminal Scaffolding Command

Run this command in your macOS Terminal (`zsh`) to generate all directories and empty source/header files in one shot:

```bash
# 1. Create Engine Directories (Current + Future)
mkdir -p KanVerse_Engine/{Audio,Core/{Application,Diagnostics,Layers,Math,Memory,Time,Threading,Serialization},Physics/Broadphase,Platform/{Events,Input,Window},Renderer/{Asset,Buffers,Camera,Context,Framebuffer,Geometry,Lighting,Materials,Pipeline,PostProcess/SSAO,Procedural,Renderer,Textures,UI,Animation},Scene/Prefabs}

# 2. Create App & Vendor Directories (Current + Future)
mkdir -p KanVerse_App/{Assets/{Audio,Meshes,Scenes,Textures/{HDR,PBR/{Floor,Ground,Metal,Roof,Wall,Wood}}},Factories,Layers/DebugOverlayLayer,Shaders,Source,Systems/{AI,Gameplay/{Combat,Inventory},Player},World/{Architecture,Builders,Environment}}
mkdir -p Vendor/{metal-cpp,stb_image}

# 3. Create All Empty Source & Header Files inside KanVerse_Engine
touch KanVerse_Engine/{KanVerse_Headers.h,PrefixHeader.pch} \
      KanVerse_Engine/Audio/{AudioSystem.hpp,AudioSystem.mm} \
      KanVerse_Engine/Core/Application/{Application.hpp,Application.mm} \
      KanVerse_Engine/Core/Diagnostics/{Assert.hpp,Log.hpp,Log.cpp} \
      KanVerse_Engine/Core/Layers/{Layer.hpp,LayerStack.hpp,LayerStack.cpp} \
      KanVerse_Engine/Core/Math/Math.hpp \
      KanVerse_Engine/Core/Memory/MemoryTracking.hpp \
      KanVerse_Engine/Core/Time/{Clock.hpp,Clock.cpp,TimeStep.hpp} \
      KanVerse_Engine/Physics/{BoundingBox.hpp,BoundingBox.cpp,ColliderPrimitives.hpp,ColliderPrimitives.cpp,PBDSolver.hpp,PBDSolver.cpp,PlayerControllerSystem.hpp,PlayerControllerSystem.cpp} \
      KanVerse_Engine/Platform/Events/{Event.hpp,KeyEvents.hpp,MouseEvents.hpp,WindowEvents.hpp} \
      KanVerse_Engine/Platform/Input/{GameControllerBridge.hpp,GameControllerBridge.mm,KeyCodes.hpp} \
      KanVerse_Engine/Platform/Window/{Window.hpp,NativeWindow.hpp,NativeWindow.mm} \
      KanVerse_Engine/Renderer/Asset/{AssetManager.hpp,AssetManager.cpp} \
      KanVerse_Engine/Renderer/Buffers/{BufferLayout.hpp,IndexBuffer.hpp,IndexBuffer.mm,VertexBuffer.hpp,VertexBuffer.mm} \
      KanVerse_Engine/Renderer/Camera/{Camera.hpp,CameraController.hpp,CameraController.mm,PerspectiveCamera.hpp,PerspectiveCamera.cpp,SpringArm.hpp,SpringArm.cpp} \
      KanVerse_Engine/Renderer/Context/{RenderContext.hpp,RenderContext.mm} \
      KanVerse_Engine/Renderer/Framebuffer/{Framebuffer.hpp,Framebuffer.mm} \
      KanVerse_Engine/Renderer/Geometry/{Mesh.hpp,Mesh.cpp,SubMesh.hpp} \
      KanVerse_Engine/Renderer/Lighting/{DirectionalLight.hpp,EnvironmentLighting.hpp,Light.hpp,PointLight.hpp,PointLight.cpp} \
      KanVerse_Engine/Renderer/Materials/{Material.hpp,Material.mm} \
      KanVerse_Engine/Renderer/Pipeline/{FrameSync.hpp,FrameSync.mm,Pipeline.hpp,Pipeline.mm,Shader.hpp,Shader.mm} \
      KanVerse_Engine/Renderer/PostProcess/{BloomPass.hpp,BloomPass.cpp} \
      KanVerse_Engine/Renderer/Procedural/{Capsule.hpp,Capsule.cpp,ChamferBox.hpp,ChamferBox.cpp} \
      KanVerse_Engine/Renderer/Renderer/{RenderCommand.hpp,RenderCommand.mm,Renderer.hpp,Renderer.mm} \
      KanVerse_Engine/Renderer/Textures/{Texture.hpp,Texture2D.hpp,Texture2D.mm,TextureCube.hpp,TextureCube.mm} \
      KanVerse_Engine/Scene/{Components.hpp,Entity.hpp,EntityID.hpp,Registry.hpp,Scene.hpp,Scene.mm,SparseSet.hpp}

# 4. Create All Empty Source, Header & Shader Files inside KanVerse_App
touch KanVerse_App/PrefixHeader.pch \
      KanVerse_App/Factories/{EntityFactory.hpp,EntityFactory.cpp,MaterialFactory.hpp,MaterialFactory.cpp} \
      KanVerse_App/Layers/{KanVerseLayer.hpp,KanVerseLayer.mm} \
      KanVerse_App/Shaders/{Bloom.metal,DebrisSimulation.metal,Equirectangular.metal,Irradiance.metal,ShadowDepth.metal,Skybox.metal,TexturedMaterial.metal,Tonemap.metal} \
      KanVerse_App/Source/{EntryPoint.mm,KanVerseApp.mm,MetalImpl.cpp} \
      KanVerse_App/Systems/AI/{EnemyControllerSystem.hpp,EnemyControllerSystem.cpp} \
      KanVerse_App/Systems/Gameplay/{DoorSystem.hpp,DoorSystem.cpp} \
      KanVerse_App/Systems/Player/{CameraRigSystem.hpp,CameraRigSystem.cpp,InputBridgeSystem.hpp,InputBridgeSystem.cpp,PlayerInputIntent.hpp} \
      KanVerse_App/World/{LevelBuilder.hpp,LevelBuilder.cpp,SceneManager.hpp,SceneManager.cpp} \
      KanVerse_App/World/Architecture/{House_1_Builder.hpp,House_1_Builder.cpp} \
      KanVerse_App/World/Builders/{DestructibleFloorBuilder.hpp,DestructibleFloorBuilder.cpp,DestructibleWallBuilder.hpp,DestructibleWallBuilder.cpp,EnemyFactory.hpp,EnemyFactory.cpp,FloorBuilder.hpp,FloorBuilder.cpp,GroundBuilder.hpp,GroundBuilder.cpp,LightBuilder.hpp,LightBuilder.cpp,RoofBuilder.hpp,RoofBuilder.cpp,StairsBuilder.hpp,StairsBuilder.cpp,WallBuilder.hpp,WallBuilder.cpp} \
      KanVerse_App/World/Environment/{AtmosphereSystem.hpp,AtmosphereSystem.cpp}

echo "KanVerse subdirectories and files created successfully in $(pwd)!"
```
