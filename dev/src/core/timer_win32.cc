// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "core_first.h"

namespace dg {

Timer::Timer(bool is_start_now) {
  ::QueryPerformanceFrequency(&ticks_per_second_);
  Reset(is_start_now);
}

void Timer::Reset(bool is_start_now) {
  is_paused_ = true;
  total_paused_time_.QuadPart = 0;
  last_paused_time_.QuadPart = 0;
  if (is_start_now) {
    Start();
  }
}

void Timer::Start() {
  if (is_paused_) {
    LARGE_INTEGER timeNow;
    ::QueryPerformanceCounter(&timeNow);
    total_paused_time_.QuadPart += timeNow.QuadPart - last_paused_time_.QuadPart;
    is_paused_ = false;
  }
}

void Timer::Pause() {
  if (!is_paused_) {
    is_paused_ = true;
    ::QueryPerformanceCounter(&last_paused_time_);
  }
}

int64_t Timer::GetTicksElapsed() const {
  LARGE_INTEGER timeNow;
  if (is_paused_) {
    timeNow = last_paused_time_;
  } else {
    ::QueryPerformanceCounter(&timeNow);
  }
  return (timeNow.QuadPart - total_paused_time_.QuadPart);
}

} // namespace dg
