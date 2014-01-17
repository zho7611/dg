// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class String;

class Name {
public:
  static const int kMaxNameLength = NamePool::kMaxNameLength;
  static const int kInvalidIndex = NamePool::kInvalidIndex;

  Name();
  Name(const Cstr* cstr);
  Name(const Name& cstr);
  Name(const String& cstr);
  ~Name();
  void Clear() {
    index_ = kInvalidIndex;
  }
  const Cstr* GetConst() const;
  void Set(const Cstr* in_from);
  void Set(const String& in_from);
  void Set(const Name& in_from) {
    index_ = in_from.index_;
  }
  bool IsEqual(const Name& rhs) const {
    return (index_ == rhs.index_);
  }
  bool IsEqual(const Cstr* rhs) const;
  void AddRight(const Name& rhs);
  void AddLeft(const Name& rhs);
  bool IsEmpty() const {
    return index_ == kInvalidIndex;
  }
  void ToString(String& outCstr);
  void FromString(const String& cstr);
  Name& operator=(const Name& rhs);
  Name& operator=(const String& rhs);
  Name& operator=(const Cstr* rhs);
  void ReadFrom(Stream* stream);
  void WriteTo(Stream* out_stream);

private:
  int index_; // Idx to the g_name_pool
};

extern NamePool* g_name_pool;

inline const Cstr* Name::GetConst() const {
  return (index_ != kInvalidIndex) ? g_name_pool->GetData(index_) : NULL;
}

} // namespace dg
