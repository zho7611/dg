// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "core_first.h"
#include <time.h>

namespace dg {

// Reference: about linux high-performance timing
// Http://tdistler.com/2010/06/27/high-performance-timing-on-linux-windows
// Http://www.guyrutenberg.com/2007/09/22/profiling-code-using-clock_gettime/

static inline timespec TimeSpecSubtract(timespec lhs, timespec rhs) {
  timespec temp;
  Check(lhs.tv_sec >= rhs.tv_sec);
  temp.tv_sec = lhs.tv_sec - rhs.tv_sec;
  temp.tv_nsec = lhs.tv_nsec - rhs.tv_nsec;
  return temp;
}

static timespec TimeSpecAdd(timespec lhs, timespec rhs) {
  timespec temp;
  temp.tv_sec = lhs.tv_sec + rhs.tv_sec;
  temp.tv_nsec = lhs.tv_nsec + rhs.tv_nsec;
  return temp;
}

Timer::Timer(bool is_start_now) {
  clock_getres(CLOCK_MONOTONIC, &timer_resolution_);
  Reset(is_start_now);
}

void Timer::Reset(bool is_start_now) {
  is_paused_ = true;
  MyMemSet(&total_paused_time_, 0, sizeof(total_paused_time_));
  MyMemSet(&last_paused_time_, 0, sizeof(last_paused_time_));
  if (is_start_now) {
    Start();
  }
}

void Timer::Start() {
  if (is_paused_) {
    timespec timeNow;
    clock_gettime(CLOCK_MONOTONIC, &timeNow);
    timespec diff = TimeSpecSubtract(timeNow, last_paused_time_);
    total_paused_time_ = TimeSpecAdd(total_paused_time_, diff);
    is_paused_ = false;
  }
}

void Timer::Pause() {
  if (!is_paused_) {
    is_paused_ = true;
    clock_gettime(CLOCK_MONOTONIC, &last_paused_time_);
  }
}

long Timer::GetTicksElapsed() const {
  struct timespec timeNow;
  if (is_paused_) {
    timeNow = last_paused_time_;
  } else {
    clock_gettime(CLOCK_MONOTONIC, &timeNow);
  }
  timespec elapsed = TimeSpecSubtract(timeNow, total_paused_time_);
  long elapsedNanoTicks = elapsed.tv_nsec / timer_resolution_.tv_nsec;
  long elapsedSecondTicks = elapsed.tv_sec * 1000000000 / timer_resolution_.tv_nsec;
  return elapsedNanoTicks + elapsedSecondTicks;
}

long Timer::GetTicksPerSecond() const {
  if (timer_resolution_.tv_nsec == 0) {
    Check(timer_resolution_.tv_sec == 1);
    return 1;
  }
  Check(timer_resolution_.tv_sec == 0);
  return 1000000000 / timer_resolution_.tv_nsec;
}

} // namespace dg
