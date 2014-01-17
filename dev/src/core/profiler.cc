// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "core_first.h"

namespace dg {

bool Profiler::is_profile_enabled = true;

Profiler::Profiler(const Cstr* profile_name, ColorInt color, bool is_start) {
  if (!is_profile_enabled) {
    return;
  }
  profile_name_ = profile_name;
  color_ = color;
  if (is_start) Start();
}

Profiler::~Profiler() {
  if (!is_profile_enabled) {
    return;
  }
  Stop();
}

void Profiler::Start() {
  ProfilerPush(profile_name_, color_);
}

void Profiler::Stop() {
  ProfilerPop();
}

#if !defined(DG_TARGET_WIN32)
void ProfilerPush(const Cstr* message, ColorInt color) {}
void ProfilerPop() {}
#endif

} // namespace dg
