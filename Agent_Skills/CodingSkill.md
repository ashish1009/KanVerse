advanced-cpp-metal-coder
Write, modify, refactor, and fix code using advanced, high-performance C++ and Apple Metal (Objective-C++ .mm and Metal Shading Language). Use whenever the user asks to write, modify, optimize, or fix code, enforcing SOLID principles, architectural design patterns, professional API design, and strict separation of .hpp declarations (with full API documentation) and .cpp/.mm implementations (with detailed block comments).

Instructions
Advanced C++ and Metal Coder
Author production-grade, ultra-high-performance C++, Objective-C++ (.mm), and Apple Metal code that strictly adheres to SOLID principles, advanced architectural design patterns, and clean file separation.

When to Use
The user asks to write new code, implement a feature, or design a module or system.
The user asks to modify, refactor, optimize, or extend existing code.
The user asks to fix bugs, memory issues, race conditions, or performance bottlenecks in code.
The task involves C++, Objective-C++, Apple Metal graphics/compute shaders, or general coding tasks where C++ or Metal is the target stack.
Steps
1. Analyze Context and Architecture
Evaluate the problem requirements, performance constraints, memory model, and concurrency needs before writing code. Select appropriate architectural and design patterns that satisfy SOLID principles:

Single Responsibility (SRP): Isolate distinct responsibilities (such as resource allocation, command encoding, and pipeline state management) into dedicated classes.
Open/Closed (OCP): Enable extension without modification using templates, CRTP (Curiously Recurring Template Pattern), concepts, or polymorphic interfaces.
Liskov Substitution (LSP): Ensure derived types and concept implementations honor all behavioral and performance contracts of their base interfaces.
Interface Segregation (ISP): Design narrow, role-specific interfaces rather than monolithic classes.
Dependency Inversion (DIP): Depend on abstractions (concepts or pure virtual interfaces) and inject dependencies cleanly.
2. Design a Professional API in .hpp Files
Place all public and protected type definitions, class declarations, concepts, and inline/template signatures in dedicated .hpp header files.

Use #pragma once and clean, hierarchical namespaces.
Design ergonomic, self-documenting, type-safe interfaces using modern C++ (C++20/C++23) features: [[nodiscard]], noexcept, constexpr, std::span, std::string_view, std::expected (or structured result types), and smart pointers (std::unique_ptr, std::shared_ptr) to make ownership semantics explicit.
Apply RAII strictly so resources (memory, file descriptors, MTL::Buffer, MTL::Texture, MTL::CommandQueue) are tied to object lifetimes.
Write comprehensive Doxygen-style API documentation exclusively in the .hpp file for every namespace, class, struct, enum, method, and template parameter (@brief, @tparam, @param, @return, @note, thread-safety guarantees, and computational/memory complexity).
Maintain clean, consistent 4-space indentation and logical grouping of member declarations.
3. Implement High-Performance Logic in .cpp or .mm Files
Place all out-of-line method definitions, internal helpers, and platform-specific or Metal bridge code in separate .cpp or .mm implementation files (and .metal files when writing GPU shaders).

Write detailed block comments (/* ... */) inside .cpp and .mm files before each function body and complex algorithmic block to explain the implementation strategy, memory layout, synchronization invariants, hardware interaction, and optimization rationale.
Optimize for hardware efficiency:
Prefer contiguous, cache-aligned data structures (alignas, Structure ofArrays where appropriate) to maximize cache locality and prevent false sharing.
Eliminate unnecessary heap allocations, copies, and virtual dispatch overhead in hot paths using move semantics, perfect forwarding, small-buffer optimizations, and compile-time polymorphism.
For Apple Metal (.mm / metal-cpp / .metal), manage autorelease pools (@autoreleasepool) explicitly, minimize CPU-GPU synchronization stalls using double/triple buffering with dispatch_semaphore_t or MTLSharedEvent, batch state changes in MTLRenderCommandEncoder / MTLComputeCommandEncoder, and optimize threadgroup memory and SIMD-group operations.
Provide complete, compilable implementations rather than truncated snippets or placeholder comments.
4. Verify Code Quality
Check the generated or modified code against this checklist before presenting it:

Separate .hpp and .cpp/.mm files are provided.
Full API documentation resides on .hpp declarations, and explanatory block comments reside in .cpp/.mm implementations.
SOLID principles and appropriate design patterns (such as RAII, Factory, Strategy, Builder, Command, or Pimpl) are cleanly applied.
Indentation, naming conventions, and error handling are consistent and production-ready.
Example Structure
Header (renderer_pipeline.hpp)
#pragma once

#include <cstdint>
#include <expected>
#include <memory>
#include <span>

namespace engine::gfx {

/**
 * @brief Error codes returned by graphics pipeline operations.
 */
enum class PipelineError : std::uint8_t {
    DeviceUnavailable,
    ShaderCompilationFailed,
    InvalidBufferAlignment
};

/**
 * @brief Abstract interface for a high-performance compute dispatch pipeline.
 *
 * Encapsulates GPU state and dispatches compute workloads with zero per-frame
 * heap allocations.
 *
 * @note Thread-safety: Instances are safe for concurrent dispatch from multiple
 *       threads only when recording into separate command buffers.
 */
class IComputePipeline {
public:
    virtual ~IComputePipeline() noexcept = default;

    /**
     * @brief Encodes and dispatches a 1D compute workload.
     *
     * @param input_data Contiguous read-only view of aligned float elements.
     * @param element_count Number of active elements to process on the GPU.
     * @return std::expected<void, PipelineError> Success or specific failure code.
     */
    [[nodiscard]] virtual auto dispatch(
        std::span<const float> input_data,
        std::size_t element_count
    ) noexcept -> std::expected<void, PipelineError> = 0;
};

} // namespace engine::gfx
Implementation (renderer_pipeline.mm)
#include "renderer_pipeline.hpp"
#import <Metal/Metal.h>

namespace engine::gfx {

/*
 * MetalComputePipeline Implementation Notes:
 * ------------------------------------------
 * Uses pre-allocated MTLBuffer ring buffers aligned to page boundaries (4096 bytes)
 * to enable zero-copy unified memory access on Apple Silicon. Command encoding is
 * wrapped in an explicit @autoreleasepool block to deterministicly reclaim transient
 * Objective-C objects at the end of each dispatch call.
 */
class MetalComputePipeline final : public IComputePipeline {
public:
    explicit MetalComputePipeline(id<MTLDevice> device, id<MTLComputePipelineState> pso) noexcept
        : device_(device), pso_(pso), queue_([device newCommandQueue]) {}

    [[nodiscard]] auto dispatch(
        std::span<const float> input_data,
        std::size_t element_count
    ) noexcept -> std::expected<void, PipelineError> override {
        @autoreleasepool {
            /*
             * Validate input alignment and state before acquiring a command buffer
             * to avoid unnecessary driver overhead on invalid arguments.
             */
            if (device_ == nil || pso_ == nil || queue_ == nil) {
                return std::unexpected(PipelineError::DeviceUnavailable);
            }

            id<MTLCommandBuffer> cmd_buffer = [queue_ commandBuffer];
            id<MTLComputeCommandEncoder> encoder = [cmd_buffer computeCommandEncoder];

            [encoder setComputePipelineState:pso_];
            [encoder endEncoding];
            [cmd_buffer commit];

            return {};
        }
    }

private:
    id<MTLDevice> device_{nil};
    id<MTLComputePipelineState> pso_{nil};
    id<MTLCommandQueue> queue_{nil};
};

} // namespace engine::gfx
Gotchas
Mixing documentation styles across files: Keep Doxygen API contracts in .hpp files so headers remain clean reference specifications, and put detailed implementation block comments (/* ... */) inside .cpp or .mm files so algorithmic details live next to the code.
Incomplete edits: When modifying or fixing existing code, output the complete updated functions or classes in both .hpp and .cpp/.mm rather than leaving ellipses (// ... existing code ...) that break compilation.
Hidden Objective-C memory leaks in .mm files: Always wrap Metal command buffer creation and transient Foundation objects in @autoreleasepool blocks inside tight loops or worker threads.
