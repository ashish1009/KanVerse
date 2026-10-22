# KanVerse

A stylized 3D action-platformer engine built from scratch for Apple Silicon (macOS M2) using modern C++23 and Apple metal-cpp[cite: 1]. Features a Sparse-Set ECS, Position-Based Dynamics (PBD) for procedural kinematics and rigid fractures, a 3-frame flight sync ring, and purely procedural geometry with zero external 3D assets[cite: 1].

---

## 📊 Status Tracking Legend

Use the following Markdown checkbox states to track your daily progress:
- `[ ]` **Pending** — Not started yet
- `[-]` **Ongoing** — Currently in progress / debugging
- `[x]` **Done** — Implemented, tested, and verified (0 memory leaks)

### Phase Progress Overview
- [ ] **Phase 1:** Engine Core, Diagnostics, Memory & SIMD Math (Days 1–12)
- [ ] **Phase 2:** Platform Events, Input Polling & Cocoa/Metal Windowing (Days 13–23)
- [ ] **Phase 3:** Physics Engine & Spatial Mathematics (Days 24–29)
- [ ] **Phase 4:** Generational Entity-Component-System (ECS) Architecture (Days 30–35)
- [ ] **Phase 5:** Metal RHI — Buffers, Shaders, Textures, Samplers & Pipelines (Days 36–52)
- [ ] **Phase 6:** Metal Shaders, Compute Bloom, PBR & High-Level Renderer (Days 53–62)
- [ ] **Phase 7:** Scene Graph, ECS Components, Player Physics & Destruction (Days 63–68)
- [ ] **Phase 8:** Application Systems — Factories, Input, Camera, Doors, AI & Atmosphere (Days 69–76)
- [ ] **Phase 9:** Procedural Architectural & World Builders (Days 77–85)
- [ ] **Phase 10:** Master Gameplay Loop, Polish & Next-Gen Extensions (Days 86–100)

---

## 🗺️ 100-Day Master Implementation Roadmap

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
- [-] **Day 3 — Tri-Sink Structured Logging**
  - **Files:** `Core/Diagnostics/Log.hpp`, `Core/Diagnostics/Log.cpp`
  - **Concepts:** C++23 `std::vformat` and `std::make_format_args`, variadic templates, triple-sink routing (`std::cout`, `std::ofstream`, and Apple Unified Logging `<os/log.h>`).
  - **Milestone:** Log all 5 severity levels (`Trace` to `Fatal`) and verify `KanVerse_Runtime.log` is created on disk.
- [ ] **Day 4 — Lock-Free Atomic Memory Tracker**
  - **Files:** `Core/Memory/MemoryTracking.hpp` (Part 1)
  - **Concepts:** Lock-free concurrency with `std::atomic`, `std::memory_order_relaxed`, Compare-And-Swap (`compare_exchange_weak`) loop for high-water mark (`PeakUsage`).
  - **Milestone:** Implement `MemoryStats` and `MemoryTracker` (`RecordAllocation`, `RecordDeallocation`, `GetStats`).
- [ ] **Day 5 — Automated RAII Memory Guards**
  - **Files:** `Core/Memory/MemoryTracking.hpp` (Part 2)
  - **Concepts:** Strict RAII design, deleting copy constructors/assignments, move semantics (`&& noexcept`) without double-counting bytes.
  - **Milestone:** Implement `ScopedMemoryAllocation`; verify `CurrentUsage` returns to `0` upon exiting a test scope.
- [ ] **Day 6 — High-Resolution Monotonic Frame Timer**
  - **Files:** `Core/Time/TimeStep.hpp`, `Core/Time/Clock.hpp`, `Core/Time/Clock.cpp`
  - **Concepts:** `std::chrono::steady_clock`, floating-point duration casting, `constexpr` and `[[nodiscard]]` attributes, implicit `operator float()`.
  - **Milestone:** Run a 60-iteration loop with `usleep(16666)` and log `dt.GetMilliseconds()`.
- [ ] **Day 7 — Apple Silicon SIMD Vector & Matrix Types**
  - **Files:** `Core/Math/Math.hpp` (Part 1)
  - **Concepts:** `<simd/simd.h>` register types (`simd::float2`, `float3`, `float4`, `float4x4`), 16-byte alignment rules (`sizeof(simd::float3) == 16`), column-major layout.
  - **Milestone:** Implement vector aliases, `MakeVec*` helpers, `Identity()`, `Multiply()`, `Inverse()`, and `Transpose()`.
- [ ] **Day 8 — Metal [0, 1] Clip-Space Projection Matrices**
  - **Files:** `Core/Math/Math.hpp` (Part 2)
  - **Concepts:** Right-handed coordinate systems, Metal's `[0, 1]` NDC Z-depth, Rodrigues' rotation formula, `LookAt` view matrix derivation.
  - **Milestone:** Implement `Perspective()`, `Ortho()`, `LookAt()`, and `Rotate()`; verify `-nearZ` maps to `0.0f` and `-farZ` maps to `1.0f`.
- [ ] **Day 9 — Engine Precompiled Header**
  - **Files:** `KanVerse_Engine/PrefixHeader.pch`
  - **Concepts:** Precompiled header mechanics, accelerating `.cpp` and `.mm` compilation, suppressing clang warnings (`-Wdocumentation`, `-Wformat-security`).
  - **Milestone:** Populate `PrefixHeader.pch` and enable precompilation in Xcode.
- [ ] **Day 10 — Metal-CPP Implementation & Hardware Context**
  - **Files:** `Source/MetalImpl.cpp`, `Renderer/Context/RenderContext.hpp`, `Renderer/Context/RenderContext.mm`
  - **Concepts:** Generating `metal-cpp` symbols via `*_PRIVATE_IMPLEMENTATION` macros, `__bridge MTL::Device*`, `MTL::CommandQueue`.
  - **Milestone:** Instantiate `RenderContext::Create()` and print the Apple M-series GPU name to the console.
- [ ] **Day 11 — Low-Latency CoreAudio / AVFoundation Subsystem**
  - **Files:** `Audio/AudioSystem.hpp`, `Audio/AudioSystem.mm`
  - **Concepts:** Objective-C++ `@autoreleasepool`, `AVAudioPlayer`, rapid re-triggering (`player.currentTime = 0`), ARC memory management with STL maps.
  - **Milestone:** Implement `AudioSystem::Init`, `LoadSound`, `Play`, and `Shutdown`.
- [ ] **Day 12 — Phase 1 Review & Unit Testing**
  - **Files:** Test harness in `Source/EntryPoint.mm`
  - **Concepts:** Memory leak verification, log file inspection, math edge-case validation.
  - **Milestone:** Exercise all Phase 1 systems in a test run and verify `0 bytes leaked` at exit.

---

### Phase 2: Platform Events, Input Polling & Cocoa/Metal Windowing (Days 13–23)
*Goal: Establish OS communication, keyboard/mouse/gamepad input, and a live 120Hz Retina `MTKView` window.*

- [ ] **Day 13 — Base Event Hierarchy & Type-Safe Dispatcher**
  - **Files:** `Platform/Events/Event.hpp`
  - **Concepts:** Polymorphic events, bitflag categories (`1 << 0`), token pasting (`#type`) in `EVENT_CLASS_TYPE`, template dispatch (`EventDispatcher::Dispatch<T>`).
  - **Milestone:** Implement `EventType`, `EventCategory`, `Event`, and `EventDispatcher`.
- [ ] **Day 14 — Window, Keyboard & Mouse Event Payloads**
  - **Files:** `Platform/Events/WindowEvents.hpp`, `Platform/Events/KeyEvents.hpp`, `Platform/Events/MouseEvents.hpp`
  - **Concepts:** Concrete event classes (`WindowResizeEvent`, `WindowCloseEvent`, `KeyPressedEvent`, `MouseMovedEvent`, `MouseScrolledEvent`, `MouseButtonPressedEvent`).
  - **Milestone:** Implement all three event headers and test routing through `EventDispatcher`.
- [ ] **Day 15 — Hardware Gamepad & Keyboard Polling Bridge**
  - **Files:** `Platform/Input/KeyCodes.hpp`, `Platform/Input/GameControllerBridge.hpp`, `Platform/Input/GameControllerBridge.mm`
  - **Concepts:** macOS virtual keycodes (`Key::W = 13`, `Key::Space = 49`), `GameController.framework` (`GCController`, `GCExtendedGamepad`), `0.15f` radial thumbstick deadzone.
  - **Milestone:** Implement `KeyCodes.hpp` and `InputBridge`; test polling with a Bluetooth controller.
- [ ] **Day 16 — Abstract Window Interface**
  - **Files:** `Platform/Window/Window.hpp`, `Platform/Window/NativeWindow.hpp`
  - **Concepts:** Platform-independent `Window` contract, `WindowProps`, `EventCallbackFn`, opaque `void*` handles (`m_WindowHandle`, `m_ViewHandle`, `m_DelegateHandle`).
  - **Milestone:** Write `Window.hpp` and `NativeWindow.hpp`.
- [ ] **Day 17 — Custom `KanVerseMetalView` Subclass**
  - **Files:** `Platform/Window/NativeWindow.mm` (Part 1)
  - **Concepts:** Subclassing `MTKView`, `NSTrackingArea` for mouse movement, overriding `keyDown:`, `keyUp:`, `flagsChanged:`, `mouseDown:`, `mouseMoved:`, `scrollWheel:`.
  - **Milestone:** Implement `@interface KanVerseMetalView : MTKView` and connect OS input callbacks.
- [ ] **Day 18 — Cocoa Window & Application Lifecycle Delegate**
  - **Files:** `Platform/Window/NativeWindow.mm` (Part 2)
  - **Concepts:** `NSWindowDelegate` & `NSApplicationDelegate`, intercepting `applicationShouldTerminate:` and returning `NSTerminateCancel` so C++ destructors run, Retina backing conversion (`convertRectToBacking:`).
  - **Milestone:** Implement `@interface KanVerseWindowDelegate`.
- [ ] **Day 19 — Programmatic Cocoa Window Creation & Event Pump**
  - **Files:** `Platform/Window/NativeWindow.mm` (Part 3)
  - **Concepts:** Storyboard-less `NSApp` setup, P-Core QoS pinning (`pthread_set_qos_class_self_np(QOS_CLASS_USER_INTERACTIVE, 0)`), `__bridge_retained` vs `__bridge_transfer` ARC lifetimes, draining `nextEventMatchingMask:`.
  - **Milestone:** Complete `NativeWindow::Init`, `Shutdown`, `OnUpdate`, `SetVSync`, and `Maximize`.
- [ ] **Day 20 — Dual-Partitioned Layer Stack**
  - **Files:** `Core/Layers/Layer.hpp`, `Core/Layers/LayerStack.hpp`, `Core/Layers/LayerStack.cpp`
  - **Concepts:** Partitioning standard layers vs overlays using `m_LayerInsertIndex`, reverse event iteration (`rbegin()`) so overlays consume events first.
  - **Milestone:** Implement `Layer` and `LayerStack`.
- [ ] **Day 21 — Master Application Singleton & Run Loop**
  - **Files:** `Core/Application/Application.hpp`, `Core/Application/Application.mm`
  - **Concepts:** Singleton orchestrator (`Application::Get()`), coordinating `RenderContext`, `Window`, `LayerStack`, and `Clock` inside `Application::Run()`.
  - **Milestone:** Implement `Application`.
- [ ] **Day 22 — Client Application Bootstrap & Entry Point**
  - **Files:** `Source/EntryPoint.mm`, `Source/KanVerseApp.mm`
  - **Concepts:** Engine `main()` vs Client `CreateApplication()`, scoped lifetime block for post-teardown `MemoryTracker::GetStats()` verification.
  - **Milestone:** Implement `EntryPoint.mm` and `KanVerseApp.mm`.
- [ ] **Day 23 — Phase 2 Integration Milestone**
  - **Files:** Full Phase 1 + Phase 2 Build
  - **Concepts:** End-to-end event loop, Cmd+Q clean shutdown, P-Core scheduling, Retina resize events.
  - **Milestone:** Launch the 120Hz macOS window, test input/resize logs, quit via `Cmd+Q`, and verify `0 bytes leaked`.

---

### Phase 3: Physics Engine & Spatial Mathematics (Days 24–29)
*Goal: Build the geometric intersection primitives and Position-Based Dynamics (PBD) solver.*

- [ ] **Day 24 — Axis-Aligned Bounding Boxes (AABB) & Raycasting**
  - **Files:** `Physics/BoundingBox.hpp`, `Physics/BoundingBox.cpp` (Part 1)
  - **Concepts:** `AABB` (`Min`, `Max`), `simd::clamp` for `ClosestPoint`, Slab Method for `IntersectsRay` (`tMin`, `tMax`, hit normals).
  - **Milestone:** Implement `Ray`, `RaycastHit`, and all `AABB` methods.
- [ ] **Day 25 — Oriented Bounding Boxes (OBB) & 15-Axis SAT**
  - **Files:** `Physics/BoundingBox.cpp` (Part 2)
  - **Concepts:** `OBB` (`Center`, `HalfExtents`, `Axes[3]`), local-space ray transformation, 15-axis Separating Axis Theorem (SAT) intersection test.
  - **Milestone:** Implement `OBB::FromTransform`, `ClosestPoint`, `Contains`, `Intersects`, and `IntersectsRay`.
- [ ] **Day 26 — Capsule & Cylinder Narrowphase Contact Generation**
  - **Files:** `Physics/ColliderPrimitives.hpp`, `Physics/ColliderPrimitives.cpp`
  - **Concepts:** Swept-sphere `CapsuleCollider`, closest-point segment projection, iterative segment-to-OBB contact generation (`ContactManifold`), `CylinderCollider`.
  - **Milestone:** Implement `ContactManifold`, `CapsuleCollider`, and `CylinderCollider`.
- [ ] **Day 27 — Position-Based Dynamics (PBD) Distance Solver**
  - **Files:** `Physics/PBDSolver.hpp`, `Physics/PBDSolver.cpp` (Part 1)
  - **Concepts:** `PBDParticle` (`Position`, `PredictedPosition`, `Velocity`, `InverseMass`), semi-implicit Euler prediction, Gauss-Seidel `PBDDistanceConstraint` projection.
  - **Milestone:** Implement `AddParticle`, `AddDistanceConstraint`, and steps 1 & 2 of `PBDSolver::Update`.
- [ ] **Day 28 — Volumetric Shape-Matching Soft-Body Clusters**
  - **Files:** `Physics/PBDSolver.cpp` (Part 2)
  - **Concepts:** `PBDCluster`, mass-weighted center of mass, local `RestPositions`, stiffness blending, velocity derivation `(PredictedPosition - Position) / dt`.
  - **Milestone:** Complete `AddCluster`, `Clear`, and steps 3 & 4 of `PBDSolver::Update`.
- [ ] **Day 29 — Physics Unit Testing**
  - **Files:** Physics test harness
  - **Concepts:** SAT edge cases, capsule-box penetration depths, PBD stability.
  - **Milestone:** Simulate a 4-particle tetrahedron falling onto `floorY` and verify stability.

---

### Phase 4: Generational Entity-Component-System (ECS) Architecture (Days 30–35)
*Goal: Build a cache-friendly, $O(1)$ paged Sparse-Set ECS from scratch.*

- [ ] **Day 30 — 64-Bit Generational Entity Handles**
  - **Files:** `Scene/EntityID.hpp`
  - **Concepts:** Bit-packing 32-bit slot index (`GetIndex`) and 32-bit generation counter (`GetGeneration`) into `uint64_t`.
  - **Milestone:** Implement `EntityID`, `NullEntity`, `GetIndex`, `GetGeneration`, and `CreateID`.
- [ ] **Day 31 — Paged Sparse Set Container (Part 1: Allocation & Emplace)**
  - **Files:** `Scene/SparseSet.hpp` (Part 1)
  - **Concepts:** Paged Sparse Sets (`PageSize = 4096`) allocating `uint32_t*` pages on demand (`AssurePage`), $O(1)$ `EntityID -> Dense Index` mapping.
  - **Milestone:** Implement `ISparseSet`, `SparseSet` constructors, `AssurePage`, `Contains`, and `Emplace`.
- [ ] **Day 32 — Paged Sparse Set Container (Part 2: Swap-and-Pop & Memory Guard)**
  - **Files:** `Scene/SparseSet.hpp` (Part 2)
  - **Concepts:** $O(1)$ Swap-and-Pop removal in `Remove()` to keep `m_DenseEntities` and `m_DenseComponents` 100% contiguous, `UpdateMemoryTracking()`.
  - **Milestone:** Implement `Get`, `Remove`, `Clear`, and `UpdateMemoryTracking`.
- [ ] **Day 33 — Generational Registry & Component Pools**
  - **Files:** `Scene/Registry.hpp` (Part 1)
  - **Concepts:** Type-erased pool map (`std::unordered_map<std::type_index, std::unique_ptr<ISparseSet>>`), slot recycling (`m_FreeList`), generation increments on `Destroy()`.
  - **Milestone:** Implement `Create`, `Destroy`, `IsValid`, `Emplace`, `Get`, `Has`, `Remove`, `Clear`, and `GetPool<T>()`.
- [ ] **Day 34 — Variadic Multi-Component Query Views**
  - **Files:** `Scene/Registry.hpp` (Part 2)
  - **Concepts:** C++20 fold expressions to find the smallest component pool (`FindSmallestPool`) as iteration leader, `requires(!std::is_invocable_v<...>)` overloads for `View::Each`.
  - **Milestone:** Implement `Registry::View<Components...>` and `GetView()`.
- [ ] **Day 35 — High-Level Entity Handle Wrapper**
  - **Files:** `Scene/Entity.hpp`
  - **Concepts:** Ergonomic facade pattern wrapping `(EntityID, Scene*)`, operator overloads (`operator bool()`, `operator EntityID()`, `operator==`).
  - **Milestone:** Implement `Entity.hpp`.

---

### Phase 5: Metal RHI — Buffers, Shaders, Textures, Samplers & Pipelines (Days 36–52)
*Goal: Build the Render Hardware Interface (RHI) wrapping Apple's Metal API.*

- [ ] **Day 36 — Vertex Attribute Layout & SIMD Stride Reflection**
  - **Files:** `Renderer/Buffers/BufferLayout.hpp`
  - **Concepts:** `ShaderDataType`, Metal 16-byte alignment (`ShaderDataType::Float3 == 16`), automatic offset and stride calculation.
  - **Milestone:** Implement `ShaderDataType`, `ShaderDataTypeSize`, `BufferElement`, and `BufferLayout`.
- [ ] **Day 37 — Unified Memory Vertex & Index Buffers**
  - **Files:** `Renderer/Buffers/VertexBuffer.hpp`, `Renderer/Buffers/VertexBuffer.mm`, `Renderer/Buffers/IndexBuffer.hpp`, `Renderer/Buffers/IndexBuffer.mm`
  - **Concepts:** `MTL::ResourceStorageModeShared` zero-copy unified memory buffers, 16-bit vs 32-bit index buffers, `ScopedMemoryAllocation`.
  - **Milestone:** Implement `MetalVertexBuffer` and `MetalIndexBuffer`.
- [ ] **Day 38 — Triple-Buffered GCD Semaphore Synchronizer**
  - **Files:** `Renderer/Pipeline/FrameSync.hpp`, `Renderer/Pipeline/FrameSync.mm`
  - **Concepts:** Triple buffering (`kMaxFramesInFlight = 3`), `dispatch_semaphore_create(3)`, `dispatch_semaphore_wait`, `cmdBuffer->addCompletedHandler` signaling.
  - **Milestone:** Implement `FrameSynchronizer`.
- [ ] **Day 39 — Metal Shading Language (MSL) Compiler & Library Loader**
  - **Files:** `Renderer/Pipeline/Shader.hpp`, `Renderer/Pipeline/Shader.mm`
  - **Concepts:** Runtime MSL compilation (`newLibrary` with `setFastMathEnabled(true)`) vs precompiled `newDefaultLibrary()`, vertex-only shadow shaders (`CreateVertexOnly`).
  - **Milestone:** Implement `MetalShader` and all factory methods.
- [ ] **Day 40 — Graphics Pipeline State & Depth/Blend Configuration**
  - **Files:** `Renderer/Pipeline/Pipeline.hpp`, `Renderer/Pipeline/Pipeline.mm`
  - **Concepts:** `MTL::VertexDescriptor` (16-byte aligned stride), `MTL::RenderPipelineDescriptor` (HDR `RGBA16Float` vs SDR `BGRA8Unorm_sRGB`, alpha blending), `MTL::DepthStencilState`, shadow depth bias.
  - **Milestone:** Implement `MetalPipeline`.
- [ ] **Day 41 — Offscreen HDR & Shadow Depth Framebuffers**
  - **Files:** `Renderer/Framebuffer/Framebuffer.hpp`, `Renderer/Framebuffer/Framebuffer.mm`
  - **Concepts:** `MTL::StorageModePrivate` render targets, `RGBA16Float` color and `Depth32Float` depth attachments, reusable `MTL::RenderPassDescriptor`.
  - **Milestone:** Implement `MetalFramebuffer` including `Resize()` and `Invalidate()`.
- [ ] **Day 42 — 2D Texture Loader (ImageIO / CoreGraphics & sRGB vs Linear)**
  - **Files:** `Renderer/Textures/Texture.hpp`, `Renderer/Textures/Texture2D.hpp`, `Renderer/Textures/Texture2D.mm`
  - **Concepts:** Decoding images via `CGImageSourceCreateWithURL` and `CGBitmapContextCreate`, `RGBA8Unorm_sRGB` (Albedo) vs `RGBA8Unorm` (Normal/Roughness/Metallic/AO), checkerboard generator.
  - **Milestone:** Implement `Texture.hpp`, `Texture2D.hpp`, and `MetalTexture2D`.
- [ ] **Day 43 — Equirectangular-to-Cubemap & Irradiance Convolution Shaders**
  - **Files:** `KanVerse_App/Shaders/Equirectangular.metal`, `KanVerse_App/Shaders/Irradiance.metal`
  - **Concepts:** Spherical UV mapping (`atan2`, `asin`), writing to a 6-slice `texture2d_array`, hemisphere Riemann-sum irradiance convolution with Reinhard tone compression.
  - **Milestone:** Write `Equirectangular.metal` and `Irradiance.metal`.
- [ ] **Day 44 — Cubemap Textures (Part 1: 6-Face & Procedural HDR Sky)**
  - **Files:** `Renderer/Textures/TextureCube.hpp`, `Renderer/Textures/TextureCube.mm` (Part 1)
  - **Concepts:** Cubemap face directions, analytical HDR sky synthesis (`ZenithColor`, `HorizonColor`, `GroundColor`, solar corona, and sun disc) in 16-bit half-floats (`__fp16`).
  - **Milestone:** Implement the 6-image constructor and `ProceduralSkyParams` constructor of `MetalTextureCube`.
- [ ] **Day 45 — Cubemap Textures (Part 2: GPU Compute HDR & Irradiance)**
  - **Files:** `Renderer/Textures/TextureCube.mm` (Part 2)
  - **Concepts:** `stbi_loadf`, `MTL::TextureType2DArray` views (`newTextureView`), dispatching `equirectangular_to_cubemap` and `irradiance_convolution`, `generateMipmaps`.
  - **Milestone:** Complete `TextureCube.mm`.
- [ ] **Day 46 — PBR Material System & Triple-Buffered Uniforms**
  - **Files:** `Renderer/Materials/Material.hpp`, `Renderer/Materials/Material.mm`
  - **Concepts:** 256-byte aligned `MaterialUniforms`, 6 PBR texture bindings, automatic transparency flagging (`color.w < 0.99f`), `Sync(frameIndex)`.
  - **Milestone:** Implement `Material`.
- [ ] **Day 47 — Procedural Chamfered Box Generator**
  - **Files:** `Renderer/Procedural/ChamferBox.hpp`, `Renderer/Procedural/ChamferBox.cpp`
  - **Concepts:** 64-byte SIMD-aligned `Vertex` struct, 6 inner faces + 12 curved bevel edges with smooth normal interpolation.
  - **Milestone:** Implement `ChamferBoxGenerator::Generate`.
- [ ] **Day 48 — Procedural UV Capsule Generator**
  - **Files:** `Renderer/Procedural/Capsule.hpp`, `Renderer/Procedural/Capsule.cpp`
  - **Concepts:** Spherical-to-Cartesian UV mesh generation (`phi`, `theta`), splitting top and bottom hemispheres around a cylindrical equator, CCW winding.
  - **Milestone:** Implement `Capsule::Create`.
- [ ] **Day 49 — High-Level Mesh Container & Global Asset Manager**
  - **Files:** `Renderer/Geometry/SubMesh.hpp`, `Renderer/Geometry/Mesh.hpp`, `Renderer/Geometry/Mesh.cpp`, `Renderer/Asset/AssetManager.hpp`, `Renderer/Asset/AssetManager.cpp`
  - **Concepts:** `Submesh` partitioning and AABB bounds, binding vertex/index buffers in `Mesh`, deduplicated texture caching in `AssetManager`.
  - **Milestone:** Implement `SubMesh.hpp`, `Mesh`, and `AssetManager`.
- [ ] **Day 50 — Perspective Camera & View-Projection Matrices**
  - **Files:** `Renderer/Camera/Camera.hpp`, `Renderer/Camera/PerspectiveCamera.hpp`, `Renderer/Camera/PerspectiveCamera.cpp`
  - **Concepts:** Base `Camera` contract, caching `m_ViewMatrix` and `m_ViewProjectionMatrix`, viewport resize handling.
  - **Milestone:** Implement `Camera` and `PerspectiveCamera`.
- [ ] **Day 51 — Orbit Camera Controller & Damped SpringArm Rig**
  - **Files:** `Renderer/Camera/CameraController.hpp`, `Renderer/Camera/CameraController.mm`, `Renderer/Camera/SpringArm.hpp`, `Renderer/Camera/SpringArm.cpp`
  - **Concepts:** Spherical orbit angles, quadratic screen-shake trauma (`m_Trauma * m_Trauma`), exponential lag damping (`1.0f - std::exp(-LagSpeed * dt)`), raycast occlusion pull-in.
  - **Milestone:** Implement `CameraController` and `SpringArm`.
- [ ] **Day 52 — Photometric Directional, Point & Environment Lights**
  - **Files:** `Renderer/Lighting/Light.hpp`, `Renderer/Lighting/DirectionalLight.hpp`, `Renderer/Lighting/PointLight.hpp`, `Renderer/Lighting/PointLight.cpp`, `Renderer/Lighting/EnvironmentLighting.hpp`
  - **Concepts:** Orthographic shadow frustum matrix (`DirectionalLight::RecalculateMatrices`), constant-linear-quadratic point light attenuation, ambient scaling.
  - **Milestone:** Implement all files in `Renderer/Lighting/`.

---

### Phase 6: Metal Shaders, Compute Bloom, PBR & High-Level Renderer (Days 53–62)
*Goal: Write the Metal Shading Language (MSL) shaders and connect the multi-pass GPU pipeline.*

- [ ] **Day 53 — Shadow Depth & Procedural Skybox Shaders**
  - **Files:** `KanVerse_App/Shaders/ShadowDepth.metal`, `KanVerse_App/Shaders/Skybox.metal`
  - **Concepts:** Instanced depth-only vertex transform (`[[instance_id]]`), vertex-less fullscreen triangle generation (`float2((vertexID << 1) & 2, vertexID & 2)`) with inverse view-projection unprojection.
  - **Milestone:** Write `ShadowDepth.metal` and `Skybox.metal`.
- [ ] **Day 54 — Dual-Kawase Compute Bloom Pass**
  - **Files:** `KanVerse_App/Shaders/Bloom.metal`, `Renderer/PostProcess/BloomPass.hpp`, `Renderer/PostProcess/BloomPass.cpp`
  - **Concepts:** Soft-knee luminance prefiltering, 13-tap anti-flicker downsample kernel (`bloom_downsample`), 9-tap 3x3 tent filter upsample kernel (`bloom_upsample`) across a 4-mip pyramid. *(Note: ensure `- 1` is intact in threadgroup size math).*
  - **Milestone:** Implement `Bloom.metal` and `BloomPass`.
- [ ] **Day 55 — ACES Filmic Tonemapping & GPU Debris Compute Shaders**
  - **Files:** `KanVerse_App/Shaders/Tonemap.metal`, `KanVerse_App/Shaders/DebrisSimulation.metal`
  - **Concepts:** HDR + Bloom composite (`(hdrColor * exposure) + (bloomColor * bloomIntensity)`), Narkowicz ACES Filmic curve, parallel GPU debris kinematics (`simulate_debris`).
  - **Milestone:** Write `Tonemap.metal` and `DebrisSimulation.metal`.
- [ ] **Day 56 — PBR Shader (Part 1: Vertex Stage, Triplanar UVs & Screen-Space TBN)**
  - **Files:** `KanVerse_App/Shaders/TexturedMaterial.metal` (Part 1)
  - **Concepts:** Normal matrix transformation, normal-offset shadow acne prevention, box-aligned UV tiling, cotangent-frame TBN normal mapping via `dfdx` and `dfdy`.
  - **Milestone:** Implement structs, BRDF helpers (`DistributionGGX`, `GeometrySmith`, `FresnelSchlick`), `vertex_main`, and normal mapping in `TexturedMaterial.metal`.
- [ ] **Day 57 — PBR Shader (Part 2: Cook-Torrance Direct Lighting, PCF Shadows & Point Lights)**
  - **Files:** `KanVerse_App/Shaders/TexturedMaterial.metal` (Part 2)
  - **Concepts:** Cook-Torrance microfacet BRDF, 3x3 Percentage-Closer Filtering (PCF) shadow sampling with smooth border fade, 64-light `GPUPointLight` loop with windowed attenuation.
  - **Milestone:** Implement sections 1 & 2 of `fragment_main` in `TexturedMaterial.metal`.
- [ ] **Day 58 — PBR Shader (Part 3: Horizon-Stabilized Image-Based Lighting)**
  - **Files:** `KanVerse_App/Shaders/TexturedMaterial.metal` (Part 3)
  - **Concepts:** Diffuse `irradianceMap` + roughness-mipmapped specular `envMap` sampling, reflection horizon stabilization (`sampleR.y = -abs(R.y) - 0.05f`), grazing Fresnel attenuation.
  - **Milestone:** Complete `TexturedMaterial.metal`.
- [ ] **Day 59 — Low-Level Render Command Dispatcher**
  - **Files:** `Renderer/Renderer/RenderCommand.hpp`, `Renderer/Renderer/RenderCommand.mm`
  - **Concepts:** Encoding indexed and hardware-instanced draw calls (`drawIndexedPrimitives` with `instanceCount`).
  - **Milestone:** Implement `RenderCommand`.
- [ ] **Day 60 — High-Level Renderer (Part 1: Initialization, Buffers, Samplers & Pipelines)**
  - **Files:** `Renderer/Renderer/Renderer.hpp`, `Renderer/Renderer/Renderer.mm` (Part 1)
  - **Concepts:** Triple-buffered `GPUUniforms`, `ShadowUniforms`, and `InstanceBuffers` (16,384 instances/frame), hardware shadow comparison sampler, compiling all 5 pipelines.
  - **Milestone:** Implement `Renderer::Init`, `Shutdown`, and state getters/setters; uncomment `Renderer::Init()` in `Application.mm`.
- [ ] **Day 61 — High-Level Renderer (Part 2: Shadow Pass, Forward Pass, Instancing & Tonemap)**
  - **Files:** `Renderer/Renderer/Renderer.mm` (Part 2)
  - **Concepts:** Multi-pass command buffer recording (`BeginShadowPass`, `BeginScene`, sub-allocating instance data in `SubmitInstanced`, executing `BloomPass` and `TonemapPipeline` in `EndScene`).
  - **Milestone:** Complete `Renderer.mm`.
- [ ] **Day 62 — Phase 6 Visual Milestone: First 3D Render**
  - **Files:** Test layer in `KanVerse_App`
  - **Concepts:** Verifying the complete Shadow -> Skybox -> HDR PBR -> Compute Bloom -> ACES Tonemap pipeline.
  - **Milestone:** Render a lit, shadowed, bevel-edged 3D cube against the procedural HDR sky!

---

### Phase 7: Scene Graph, ECS Components, Player Physics & Destruction (Days 63–68)
*Goal: Connect the ECS Registry to the Renderer and Physics engine, enabling character locomotion, ground-slam shockwaves, and shattering debris.*

- [ ] **Day 63 — Core ECS Components**
  - **Files:** `Scene/Components.hpp`
  - **Concepts:** `TagComponent`, `TransformComponent`, `MeshComponent`, `MaterialComponent`, `CameraComponent`, `DirectionalLightComponent`, `PointLightComponent`, `EnvironmentLightComponent`, `ColliderComponent`, `CharacterControllerComponent`, `DebrisComponent`, `FracturableComponent`, `DoorComponent`, `AIControllerComponent`.
  - **Milestone:** Implement `Components.hpp`.
- [ ] **Day 64 — Kinematic Player Controller (Part 1: Locomotion, Jumping & Sequential Collisions)**
  - **Files:** `Physics/PlayerControllerSystem.hpp`, `Physics/PlayerControllerSystem.cpp` (Part 1)
  - **Concepts:** View-relative movement vectors, double-jump impulse, airborne ground-slam dive, **Sequential Collision Resolution** (resolving Y-axis steps/floors *before* X and Z walls).
  - **Milestone:** Implement sections 1–4 of `PlayerControllerSystem::OnUpdate`.
- [ ] **Day 65 — Kinematic Player Controller (Part 2: Banking, Pitching & Squash-and-Stretch Spring)**
  - **Files:** `Physics/PlayerControllerSystem.cpp` (Part 2)
  - **Concepts:** Shortest-arc yaw interpolation, turn banking (`RollAngle`), sprint leaning (`PitchAngle`), Hooke's Law damped spring (`-k * x - c * v`) with volume-preserving scale. *(Note: ensure `*` is present in `controller.RollAngle

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
