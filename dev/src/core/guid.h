// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

// Small sized GUID for small number of resources
struct SmallSizeGuid {
  unsigned long data;

  SmallSizeGuid() {
    Init();
  }
  SmallSizeGuid(const SmallSizeGuid& rhs) {
    CopyFrom(rhs);
  }
  const SmallSizeGuid& operator=(const SmallSizeGuid& rhs) {
    return CopyFrom(rhs);
  }
  bool operator ==(const SmallSizeGuid& rhs) const {
    return IsEqual(rhs);
  }
  bool operator !=(const SmallSizeGuid& rhs) const {
    return !IsEqual(rhs);
  }
  bool operator <(const SmallSizeGuid& rhs) const {
    return data < rhs.data;
  }
  void Init() {
    data = 0;
  }
  bool IsValid() const {
    return data > 0;
  }
  bool MakeUnique();
  bool IsEqual(const SmallSizeGuid& rhs) const {
    return data == rhs.data;
  }
  const SmallSizeGuid& CopyFrom(const SmallSizeGuid& rhs) {
    data = rhs.data;
    return *this;
  }
  void ToString(class String& out_str) const;
  void FromString(const class String& str);
  unsigned int GetHashCode() const;
};

// Standard 16 byte GUID
struct BigSizeGuid {
  static const int kNumData4Bytes = 8;
  unsigned long data1;
  unsigned short data2;
  unsigned short data3;
  unsigned char data4[kNumData4Bytes];

  BigSizeGuid() {
    Init();
  }
  BigSizeGuid(const BigSizeGuid& rhs) {
    CopyFrom(rhs);
  }
  const BigSizeGuid& operator=(const BigSizeGuid& rhs) {
    return CopyFrom(rhs);
  }
  bool operator ==(const BigSizeGuid& rhs) const {
    return IsEqual(rhs);
  }
  bool operator !=(const BigSizeGuid& rhs) const {
    return !IsEqual(rhs);
  }
  bool operator <(const BigSizeGuid& rhs) const {
    return IsLessThan(rhs);
  }
  bool IsEqual(const BigSizeGuid& rhs) const;
  bool IsLessThan(const BigSizeGuid& rhs) const;
  void Init();
  bool IsValid() const;
  bool MakeUnique();
  const BigSizeGuid& CopyFrom(const BigSizeGuid& rhs);
  void ToString(class String& out_str) const;
  void FromString(const class String& str);
  unsigned long GetHashCode() const;
};

#define DG_USE_SMALL_SIZE_GRID
#ifdef DG_USE_SMALL_SIZE_GRID
typedef SmallSizeGuid CoreGuid;
#else
typedef BigSizeGuid CoreGuid;
#endif

} // namespace dg
