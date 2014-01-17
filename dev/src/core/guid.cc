// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "core_first.h"

namespace dg {

struct SmallSizeGuidStruct {
  bool Initialized_;
  unsigned int UserId_;
  unsigned int LastData_;

  SmallSizeGuidStruct() :
    Initialized_(false),
    UserId_(0),
    LastData_(0) {}
};

static SmallSizeGuidStruct SmallSizeGuidStruct_;

bool SmallSizeGuid::MakeUnique() {
  if (!SmallSizeGuidStruct_.Initialized_) {
    String username;
    Os::GetUsername(username);
    // Username is required for the case that multiple developers generate guids at the same time
    // Not more than 255 users can generate unique id at the same time(msec) in best-case if
    // The hash-code does not conflict
    SmallSizeGuidStruct_.UserId_ = GetStringHashCode(username.Get());
    SmallSizeGuidStruct_.UserId_ <<= 24;
    SmallSizeGuidStruct_.Initialized_ = true;
  }
  do {
    DateTime now_date_time = DateTime::Now();
    const unsigned int kStartYear = 2010;
    // Time is msec since the start year: 2010-01-01
    const unsigned int now_time_in_msec =
        (now_date_time.Year() - kStartYear) * 1000 * 60 * 60 * 24 * 31 * 12 +
        now_date_time.Month() * 1000 * 60 * 60 * 24 * 31 +
        now_date_time.Day() * 1000 * 60 * 60 * 24 +
        now_date_time.Hour() * 1000 * 60 * 60 +
        now_date_time.Minute() * 1000 * 60 +
        now_date_time.Second() * 1000 +
        now_date_time.MilliSeconds();
    // Data will be [userid-hash] xor [time-in-milli-second]
    // The high 8-bit of the msec will be altered by the user id
    data = SmallSizeGuidStruct_.UserId_ ^ now_time_in_msec;
    // Not to generate more than one guid in a msec
    Os::SleepMilliSeconds(1);
    // Try until we get the different id data
  } while (data == SmallSizeGuidStruct_.LastData_);
  // Update the last data to compare with next time
  SmallSizeGuidStruct_.LastData_ = data;
  return true;
}

void SmallSizeGuid::ToString(class String& out_str) const {
  out_str.Format(TXT("%08X"), data);
}

void SmallSizeGuid::FromString(const class String& str) {
  MySnscanf(str.GetConst(), str.size(), TXT("%08X"), &data);
}

inline unsigned int SmallSizeGuid::GetHashCode() const {
  return data;
}

bool BigSizeGuid::IsEqual(const BigSizeGuid& rhs) const {
  if (data1 != rhs.data1)
    return false;
  if (data2 != rhs.data2)
    return false;
  if (data3 != rhs.data3)
    return false;
  for (int idx = 0; idx < kNumData4Bytes; ++idx) {
    if (data4[idx] != rhs.data4[idx]) {
      return false;
    }
  }
  return true;
}

void BigSizeGuid::Init() {
  data1 = 0;
  data2 = 0;
  data3 = 0;
  MyMemSet(data4, 0, sizeof(unsigned char)*kNumData4Bytes);
}

bool BigSizeGuid::IsValid() const {
  if (data1 != 0)
    return true;
  if (data2 != 0)
    return true;
  if (data3 != 0)
    return true;
  for (int idx = 0; idx < kNumData4Bytes; ++idx) {
    if (data4[idx] != 0) {
      return true;
    }
  }
  return false;
}

const BigSizeGuid& BigSizeGuid::CopyFrom(const BigSizeGuid& rhs) {
  data1 = rhs.data1;
  data2 = rhs.data2;
  data3 = rhs.data3;
  for (int idx = 0; idx < kNumData4Bytes; ++idx) {
    data4[idx] = rhs.data4[idx];
  }
  return *this;
}

bool BigSizeGuid::IsLessThan(const BigSizeGuid& rhs) const {
  if (data1 != rhs.data1)
    return (data1 < rhs.data1);
  if (data2 != rhs.data2)
    return (data2 < rhs.data2);
  if (data3 != rhs.data3)
    return (data3 < rhs.data3);
  for (int idx = 0; idx < kNumData4Bytes; ++idx) {
    if (data4[idx] != rhs.data4[idx]) {
      return (data4[idx] < rhs.data4[idx]);
    }
  }
  return false;
}

void BigSizeGuid::ToString(class String& out_str) const {
  out_str.Format(TXT("%08X-%04X-%04X-%08X%08X"),
      data1, data2, data3,
      *((unsigned long*)&data4[0]),
      *((unsigned long*)&data4[4]));
}

void BigSizeGuid::FromString(const class String& str) {
  MySnscanf(str.GetConst(), str.size(), TXT("%08X-%04X-%04X-%08X%08X"),
      &data1, &data2, &data3, (unsigned long*)(&data4[0]), (unsigned long*)(&data4[4]));
}

inline unsigned long BigSizeGuid::GetHashCode() const {
  return *(unsigned long*)(&data4[4]);
}

} // namespace dg
