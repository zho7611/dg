// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "core_first.h"

namespace dg {

DateTime::DateTime() {
  MyMemSet(this, 0, sizeof(DateTime));
}

int DateTime::Year() {
  return system_time_struct_.tm_year;
}

int DateTime::Month() {
  return system_time_struct_.tm_mon;
}

int DateTime::Day() {
  return system_time_struct_.tm_mday;
}

int DateTime::Hour() {
  return system_time_struct_.tm_hour;
}

int DateTime::Minute() {
  return system_time_struct_.tm_min;
}

int DateTime::Second() {
  return system_time_struct_.tm_sec;
}

int DateTime::MilliSeconds() {
  return MilliSeconds_;
}

DateTime DateTime::Now() {
  DateTime now;
  struct timeval tv;
  struct timezone tz;
  struct tm *tm;
  gettimeofday(&tv, &tz);
  struct tm* timeInfo = ::localtime(&tv.tv_sec);
  now.system_time_struct_ = *timeInfo;
  now.MilliSeconds_ = tv.tv_usec / 1000;
  return now;
}

} // namespace dg
