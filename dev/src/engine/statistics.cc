// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

Statistics::Statistics() :
  start_time_in_seconds_(0),
  frames_per_second_(0),
  is_started_(false),
  current_frame_(0) {
  Check(GetInstance() == this);
}

Statistics::~Statistics() {
}

Statistics* Statistics::GetInstance() {
  static Statistics* s_instance = 0;
  if (!s_instance) {
    s_instance = this;
  }
  return s_instance;
}

float Statistics::GetFramesPerSecond() {
  if (!is_started_) {
    is_started_ = true;
    start_time_in_seconds_ = g_renderer.timer.GetTimeInSeconds();
    return 0;
  }
  float time_now = g_renderer.timer.GetTimeInSeconds();
  float delta_in_seconds = time_now - start_time_in_seconds_;
  if (delta_in_seconds < 1.0f) {
    return frames_per_second_;
  }
  frames_per_second_ = float(current_frame_) / delta_in_seconds;
  start_time_in_seconds_ = time_now;
  current_frame_ = 0;
  return frames_per_second_;
}

void Statistics::IncreaseFrame() {
  ++current_frame_;
}

} // namespace dg
