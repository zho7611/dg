// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class Profiler {
public:
  Profiler(const Cstr* profile_name, ColorInt color, bool is_start = true);
  ~Profiler();
  static bool is_profile_enabled;
  void Start();
  void Stop();

private:

  ColorInt color_;
  const Cstr* profile_name_;
};

#if !defined(DG_SHIP)
#define DG_SCOPED_PROFILER(ProfilerName, ColorValue) Profiler dg_profiler_##LINE##(ProfilerName, ColorValue);
#else
#define DG_SCOPED_PROFILER(ProfilerName, ColorValue)
#endif

void ProfilerPush(const Cstr* message, ColorInt color);
void ProfilerPop();

} // namespace dg
