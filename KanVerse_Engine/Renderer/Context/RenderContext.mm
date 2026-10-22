/*!
 * @file RenderContext.mm
 * @brief Implementation of the Metal render context using metal-cpp and Foundation.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#include "Core/Diagnostics/Log.hpp"
#include "Core/Diagnostics/Assert.hpp"
#include "Renderer/Context/RenderContext.hpp"

// metal-cpp headers in canonical order
#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>

// Apple Native Frameworks
#import <Metal/Metal.h>
#import <Foundation/Foundation.h>
#include <cassert>

namespace KanVerse {
  
  std::unique_ptr<RenderContext> RenderContext::Create() {
    return std::make_unique<RenderContext>();
  }
  
  RenderContext::RenderContext() {
    // Retrieve system default device via native runtime and bridge to metal-cpp wrapper
    id<MTLDevice> sysDevice = MTLCreateSystemDefaultDevice();
    if (!sysDevice) {
      KV_CORE_ERROR("Metal is not supported on this host.");
      KV_CORE_ASSERT(false, "Metal is not supported on this host.");
      return;
    }
    
    m_Device = (__bridge MTL::Device*)sysDevice;
    
    m_CommandQueue = m_Device->newCommandQueue();
    if (!m_CommandQueue) {
      KV_CORE_ERROR("Failed to create primary MTL::CommandQueue.");
      KV_CORE_ASSERT(false, "Failed to create primary MTL::CommandQueue.");
      return;
    }
    
    KV_CORE_INFO("RenderContext initialized successfully on device: %s",
                 m_Device->name()->utf8String());
  }
  
  RenderContext::~RenderContext() {
    if (m_CommandQueue) {
      m_CommandQueue->release();
      m_CommandQueue = nullptr;
    }
    if (m_Device) {
      m_Device->release();
      m_Device = nullptr;
    }
  }
  
  RenderContext::RenderContext(RenderContext&& other) noexcept
  : m_Device(other.m_Device), m_CommandQueue(other.m_CommandQueue) {
    other.m_Device = nullptr;
    other.m_CommandQueue = nullptr;
  }
  
  RenderContext& RenderContext::operator=(RenderContext&& other) noexcept {
    if (this != &other) {
      if (m_CommandQueue) m_CommandQueue->release();
      if (m_Device) m_Device->release();
      
      m_Device = other.m_Device;
      m_CommandQueue = other.m_CommandQueue;
      
      other.m_Device = nullptr;
      other.m_CommandQueue = nullptr;
    }
    return *this;
  }
  
} // namespace KanVerse

