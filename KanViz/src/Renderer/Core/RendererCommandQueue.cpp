//
//  RendererCommandQueue.cpp
//  KanViz
//
//  Created by Ashish . on 29/10/25.
//

#include "RendererCommandQueue.hpp"

namespace KanViz
{
  RenderCommandQueue::RenderCommandQueue(uint32_t commandBufferSize)
  : m_commandBufferSize(commandBufferSize * 1024 * 1024) // Convert the size in bytes from MB
  {
    IK_PROFILE();
    m_commandBuffer = std::make_unique<uint8_t[]>(m_commandBufferSize);
    m_nextCommandPtr = m_commandBuffer.get();
    std::memset(m_commandBuffer.get(), 0, m_commandBufferSize);
    
    IK_LOG_INFO(LogModule::Renderer, "Creating Renderer command queue. (Size {0} MB)", m_commandBufferSize);
  }
  
  RenderCommandQueue::~RenderCommandQueue()
  {
    IK_PROFILE();
    IK_LOG_WARN(LogModule::Renderer, "Destroying Renderer command queue");
  }
  
  void* RenderCommandQueue::Allocate(RenderCommandFn function, uint32_t size)
  {
    IK_PERFORMANCE_FUNC("RenderCommandQueue::Allocate");
    
    IK_ASSERT(m_commandBuffer, "Command Buffer is not allocated");
    IK_ASSERT(size > 0, "Function size is 0");
    
    // Check memory overflow
    uint64_t memoryUsed = static_cast<uint64_t>(m_nextCommandPtr - m_commandBuffer.get());
    uint64_t memoryFree = m_commandBufferSize - memoryUsed;
    
    if (memoryFree < (sizeof(RenderCommandFn) + sizeof(uint32_t) + size))
    {
      throw std::runtime_error("Memory Overflow for Commands");
    }
    
    // 1. Stores the Render Command function pointer in the queue. 'This is lambda that calls the Render Command submited in Renderer::Submit()'
    //    |< RenderCommandFn >|------------------------------------------------------------------| // 10 MB memory
    //    ^
    // m_commandBufferPtr
    *reinterpret_cast<RenderCommandFn*>(m_nextCommandPtr) = function;
    
    // 2. Update the pointer after size of RenderCommandFn pointer
    //    |< RenderCommandFn >|------------------------------------------------------------------| // 10 MB memory
    //                        ^
    //                m_commandBufferPtr // Shift the pointer after RenderCommandFn pointer
    m_nextCommandPtr += sizeof(RenderCommandFn);
    
    // 3. Store the size of Render command function in queue (Submitted in Renderer::Submit()). Actual size of render command
    //    |< RenderCommandFn >|<  size  >|-------------------------------------------------------| // 10 MB memory
    //                        ^
    //                m_commandBufferPtr
    *reinterpret_cast<uint32_t*>(m_nextCommandPtr) = size;
    
    // 4. Update the pointer after size of uint32_t which stores the size of Render Command function
    //    |< RenderCommandFn >|<  size  >|-------------------------------------------------------| // 10 MB memory
    //                                   ^
    //                            m_commandBufferPtr // Shifter the pointer after size of uint32 which stores the size
    m_nextCommandPtr += sizeof(uint32_t);
    
    // 5. Stores the current memory address where Renderer::Submit() will store the actual render command function
    //    |< RenderCommandFn >|<  size  >|-------------------------------------------------------| // 10 MB memory
    //                                   ^
    //                            m_commandBufferPtr
    //                                memory
    void* memory = m_nextCommandPtr;
    
    // 6. Update the pointer just after the function
    //    |< RenderCommandFn >|<  size  >|< Function to be written by Renderer::Submit >|--------| // 10 MB memory
    //                                   ^                                              ^
    //                                memory                                      m_commandBufferPtr
    m_nextCommandPtr += size;
    
    // 7. Increment the Command count
    m_commandCount++;
    
    // 8. Return the memory address where actual function to be written by Renderer::Submit()
    return memory;
  }
  
  void RenderCommandQueue::Execute()
  {
    IK_PERFORMANCE_FUNC("RenderCommandQueue::Execute");
    
    // Lock the mutex
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // 1. Store the start address of command queue
    uint8_t* buffer = m_commandBuffer.get();
    
    for (uint32_t commandIdx = 0; commandIdx < m_commandCount; commandIdx++)
    {
      // 2. Get the RenderCommandFn pointer
      RenderCommandFn function = *reinterpret_cast<RenderCommandFn*>(buffer);
      
      // 3. Shift the pointer after RenderCommandFn to get size of function
      buffer += sizeof(RenderCommandFn);
      
      // 4. Get the size of function
      uint32_t size = *reinterpret_cast<uint32_t*>(buffer);
      
      // 5. Shift the pointer after the size of uint32_t to get the actual render function.
      buffer += sizeof(uint32_t);
      
      // 6. execute the function
      function(buffer);
      
      // 7. Shift the pointer after Render function
      buffer += size;
    }
    
    // Reset the command queue
    m_nextCommandPtr = m_commandBuffer.get();
    m_commandCount = 0;
  }
} // namespace KanViz
