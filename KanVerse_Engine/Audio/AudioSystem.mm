/*!
 * @file AudioSystem.mm
 * @brief Implementation of AudioSystem using AVFoundation for low-latency playback.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#include "Audio/AudioSystem.hpp"
#include "Core/Diagnostics/Log.hpp"
#include "Core/Diagnostics/Assert.hpp"

#import <AVFoundation/AVFoundation.h>
#import <Foundation/Foundation.h>

namespace KanVerse {
  
  static std::unordered_map<SoundEffect, AVAudioPlayer*> s_AudioPlayers;
  
  void AudioSystem::Init() {
    KV_CORE_INFO("AudioSystem initialized.");
  }
  
  void AudioSystem::Shutdown() {
    for (auto& [effect, player] : s_AudioPlayers) {
      if (player) {
        [player stop];
        // Note: Removed [player release] because ARC manages memory automatically here
      }
    }
    s_AudioPlayers.clear();
    KV_CORE_INFO("AudioSystem shutdown complete.");
  }
  
  void AudioSystem::LoadSound(SoundEffect effect, std::string_view filepath) {
    @autoreleasepool {
      NSString* pathStr = [NSString stringWithUTF8String:filepath.data()];
      NSURL* url = [NSURL fileURLWithPath:pathStr];
      
      NSError* error = nil;
      AVAudioPlayer* player = [[AVAudioPlayer alloc] initWithContentsOfURL:url error:&error];
      
      if (error || !player) {
        KV_CORE_WARN("AudioSystem: Failed to load sound file from '{}'", filepath);
        return;
      }
      
      [player prepareToPlay];
      s_AudioPlayers[effect] = player;
      KV_CORE_INFO("AudioSystem: Loaded sound successfully: '{}'", filepath);
    }
  }
  
  void AudioSystem::Play(SoundEffect effect) {
    auto it = s_AudioPlayers.find(effect);
    if (it != s_AudioPlayers.end() && it->second) {
      AVAudioPlayer* player = it->second;
      if (player.isPlaying) {
        player.currentTime = 0; // Rewind for rapid triggers
      }
      [player play];
    }
  }
  
} // namespace KanVerse

