// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class Timer {
public:
  Timer(bool is_start_now = false);
  void Start();
  void Reset(bool is_start_now = false);
  void Pause();
  bool IsRunning() const {
    return !is_paused_;
  }
  double GetTimeInMilliSeconds() const {
    return double(GetTicksElapsed()*1000.0) / GetTicksPerSecond();
  }
  double GetTimeInSeconds() const {
    return double(GetTicksElapsed()) / GetTicksPerSecond();
  }

public:
  bool is_paused_;

#if defined(DG_TARGET_WIN32)
  // Get time ticks elapsed
  int64_t GetTicksElapsed() const;
  // Get the timer frequency. Time tick count per second
  int64_t GetTicksPerSecond() const {
    return ticks_per_second_.QuadPart;
  }
  LARGE_INTEGER ticks_per_second_;
  LARGE_INTEGER last_paused_time_;
  LARGE_INTEGER total_paused_time_;
#elif defined(DG_TARGET_ANDROID)
  long GetTicksElapsed() const;
  long GetTicksPerSecond() const;
  struct timespec timer_resolution_;
  struct timespec last_paused_time_;
  struct timespec total_paused_time_;
#elif defined(DG_TARGET_MAC)
  uint64_t GetTicksElapsed() const;
  uint64_t GetTicksPerSecond() const {
    // Resolution is in nano-seconds
    return timer_resolution_.denom * 1000 * 1000 * 1000 / timer_resolution_.numer;
  }
  mach_timebase_info_data_t timer_resolution_;
  uint64_t last_paused_time_;
  uint64_t total_paused_time_;
#endif
};

} // namespace dg
