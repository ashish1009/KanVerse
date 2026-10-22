/*!
 * @file FrameSync.mm
 * @brief Implementation of the Grand Central Dispatch frame synchronizer.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#include "Renderer/Pipeline/FrameSync.hpp"

#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>

#import <Metal/Metal.h>
#import <dispatch/dispatch.h>

namespace KanVerse {
  
  FrameSynchronizer::FrameSynchronizer(uint32_t maxFramesInFlight)
  : m_MaxFramesInFlight(maxFramesInFlight),
  m_CurrentFrameIndex(0) {
    dispatch_semaphore_t sema = dispatch_semaphore_create(maxFramesInFlight);
    m_Semaphore = (__bridge_retained void*)sema;
  }
  
  FrameSynchronizer::~FrameSynchronizer() {
    if (m_Semaphore) {
      dispatch_semaphore_t sema = (__bridge_transfer dispatch_semaphore_t)m_Semaphore;
      sema = nil;
      m_Semaphore = nullptr;
    }
  }
  
  FrameSynchronizer::FrameSynchronizer(FrameSynchronizer&& other) noexcept
  : m_Semaphore(other.m_Semaphore),
  m_MaxFramesInFlight(other.m_MaxFramesInFlight),
  m_CurrentFrameIndex(other.m_CurrentFrameIndex) {
    other.m_Semaphore = nullptr;
  }
  
  FrameSynchronizer& FrameSynchronizer::operator=(FrameSynchronizer&& other) noexcept {
    if (this != &other) {
      if (m_Semaphore) {
        dispatch_semaphore_t sema = (__bridge_transfer dispatch_semaphore_t)m_Semaphore;
        sema = nil;
      }
      m_Semaphore = other.m_Semaphore;
      m_MaxFramesInFlight = other.m_MaxFramesInFlight;
      m_CurrentFrameIndex = other.m_CurrentFrameIndex;
      other.m_Semaphore = nullptr;
    }
    return *this;
  }
  
  uint32_t FrameSynchronizer::BeginFrame() {
    dispatch_semaphore_t sema = (__bridge dispatch_semaphore_t)m_Semaphore;
    dispatch_semaphore_wait(sema, DISPATCH_TIME_FOREVER);
    
    m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % m_MaxFramesInFlight;
    return m_CurrentFrameIndex;
  }
  
  void FrameSynchronizer::AttachToCommandBuffer(MTL::CommandBuffer* cmdBuffer) {
    dispatch_semaphore_t sema = (__bridge dispatch_semaphore_t)m_Semaphore;
    __block dispatch_semaphore_t blockSema = sema;
    
    cmdBuffer->addCompletedHandler(^void(MTL::CommandBuffer*) {
      dispatch_semaphore_signal(blockSema);
    });
  }
  
} // namespace KanVerse
