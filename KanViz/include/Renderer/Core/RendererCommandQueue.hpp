//
//  RendererCommandQueue.hpp
//  KanViz
//
//  Created by Ashish . on 29/10/25.
//

#pragma once

namespace KanViz
{
  /// This class stores render commands in a queue and executes them in one go.
  class RenderCommandQueue
  {
  public:
    // Define the render function pointer type
    typedef void(*RenderCommandFn)(void*);
    
    /// Constructs the render command queue instance and allocates memory.
    /// - Parameter commandBufferSize: Buffer memory size in mega bytes.
    RenderCommandQueue(uint32_t commandBufferSize);
    /// Destroys the render command queue and releases allocated memory.
    ~RenderCommandQueue();
    
    /// Allocates a function in the buffer/queue.
    /// - Parameters:
    ///   - function: Function pointer to the render command.
    ///   - size: Size of the command in bytes.
    /// - Returns: A pointer to the allocated memory for the command.
    void* Allocate(RenderCommandFn function, uint32_t size);
    
    /// Executes the functions in the queue.
    void Execute();
    
    // Delete copy and move constructors.
    DELETE_COPY_MOVE_CONSTRUCTORS(RenderCommandQueue);
    
  private:
    Scope<uint8_t[]> m_commandBuffer {nullptr}; //< Pointer to the command buffer.
    uint8_t* m_nextCommandPtr {nullptr};        //< Pointer to the next available position in the command buffer.
    size_t m_commandBufferSize {0};             //< Total size of the command buffer in bytes.
    std::atomic<uint32_t> m_commandCount;       //< Atomic for thread safety
    std::mutex m_mutex;                         //< Mutex for locking
  };
} // namespace KanViz
