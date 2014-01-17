// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class Statistics {
public:
  Statistics();
  ~Statistics();

  float GetFramesPerSecond();
  void IncreaseFrame();

private:
  bool is_started_;
  float frames_per_second_;
  int current_frame_;
  float start_time_in_seconds_;

  Statistics* GetInstance();
};

} // namespace dg

