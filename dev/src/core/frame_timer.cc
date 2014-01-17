// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "core_first.h"

namespace dg {

FrameTimer::FrameTimer() :
  last_time_in_milli_seconds_(0),
  time_in_milli_seconds_(0),
  delta_in_milli_seconds_(0) {}

void FrameTimer::Start() {
  timer_.Start();
  last_time_in_milli_seconds_ = time_in_milli_seconds_ = (float)timer_.GetTimeInMilliSeconds();
}

void FrameTimer::TickFrame() {
  Check(timer_.IsRunning());
  last_time_in_milli_seconds_ = time_in_milli_seconds_;
  time_in_milli_seconds_ = (float)timer_.GetTimeInMilliSeconds();
  delta_in_milli_seconds_ = time_in_milli_seconds_ - last_time_in_milli_seconds_;
}

} // namespace dg
