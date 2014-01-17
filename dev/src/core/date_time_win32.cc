// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "core_first.h"

namespace dg {

DateTime::DateTime() {
  MyMemSet(this, 0, sizeof(DateTime));
}

int DateTime::Year() {
  return system_time_struct_.wYear;
}

int DateTime::Month() {
  return system_time_struct_.wMonth;
}

int DateTime::Day() {
  return system_time_struct_.wDay;
}

int DateTime::Hour() {
  return system_time_struct_.wHour;
}

int DateTime::Minute() {
  return system_time_struct_.wMinute;
}

int DateTime::Second() {
  return system_time_struct_.wSecond;
}

int DateTime::MilliSeconds() {
  return system_time_struct_.wMilliseconds;
}

DateTime DateTime::Now() {
  DateTime now;
  ::GetLocalTime(&now.system_time_struct_);
  return now;
}

} // namespace dg
