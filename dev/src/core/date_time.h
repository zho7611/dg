// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class DateTime {
public:
  DateTime();
  int Year();
  int Month();
  int Day();
  int Hour();
  int Minute();
  int Second();
  int MilliSeconds();

  static DateTime Now();

private:
#if defined(DG_TARGET_WIN32)
  SYSTEMTIME system_time_struct_;
#else
  struct tm system_time_struct_;
  int MilliSeconds_;
#endif
};

} // namespace dg
