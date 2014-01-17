// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "core_first.h"

namespace dg {

Timer::Timer(bool is_start_now) {
  mach_timebase_info(&timer_info_);
  Reset(is_start_now);
}

void Timer::Reset(bool is_start_now) {
  is_paused_ = true;
  total_paused_time_ = last_paused_time_ = 0;
  if (is_start_now) {
    Start();
  }
}

void Timer::Start() {
  if (is_paused_) {
    uint64_t time_now = mach_absolute_time();
    total_paused_time_ += time_now - last_paused_time_;
    is_paused_ = false;
  }
}

void Timer::Pause() {
  if (!is_paused_) {
    is_paused_ = true;
    last_paused_time_ = mach_absolute_time();
  }
}

uint64_t Timer::GetTicksElapsed() const {
  uint64_t time_now;
  if (is_paused_) {
    time_now = last_paused_time_;
  } else {
    time_now = mach_absolute_time();
  }
  uint64_t diff_nano_seconds = time_now - total_paused_time_;
  return diff_nano_seconds;
}

} // namespace dg
