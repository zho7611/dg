// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class FrameTimer {
public:
  FrameTimer();
  bool IsRunning() const {
    return timer_.IsRunning();
  }
  void Start();
  void TickFrame();
  float GetTimeInSeconds() const {
    return time_in_milli_seconds_/1000.0f;
  }
  float GetTimeInMilliSeconds() const {
    return time_in_milli_seconds_;
  }
  float GetDeltaInMilliSeconds() const {
    return delta_in_milli_seconds_;
  }
  float GetDeltaInSeconds() const {
    return delta_in_milli_seconds_/1000.0f;
  }

private:
  Timer timer_;
  float last_time_in_milli_seconds_;
  float time_in_milli_seconds_;
  float delta_in_milli_seconds_;
};

} // namespace dg
