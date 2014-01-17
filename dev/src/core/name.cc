// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "core_first.h"

namespace dg {

Name::Name() : index_(kInvalidIndex) {}

Name::Name(const Cstr* cstr) : index_(kInvalidIndex) {
  Set(cstr);
}

Name::Name(const Name& cstr) : index_(cstr.index_) {
}

Name::Name(const String& cstr) : index_(kInvalidIndex) {
  Set(cstr);
}

Name::~Name() {
}

void Name::Set(const Cstr* in_from) {
  if (!in_from) {
    index_ = kInvalidIndex;
    return;
  }
  int idx = g_name_pool->FindNameSlow(in_from);
  if (idx == kInvalidIndex) {
    index_ = g_name_pool->CreateName(in_from);
  } else {
    index_ = idx;
  }
}

void Name::Set(const String& in_from) {
  Set(in_from.GetConst());
}

void Name::ToString(String& outCstr) {
  outCstr.Set(GetConst());
}

void Name::FromString(const String& cstr) {
  Set(cstr.GetConst());
}

void Name::AddRight(const Name& rhs) {
  String lhsStr(GetConst());
  String rhsStr(rhs.GetConst());
  Set(lhsStr.AddRight(rhsStr));
}

void Name::AddLeft(const Name& lhs) {
  String lhsStr(lhs.GetConst());
  String rhsStr(GetConst());
  Set(lhsStr.AddRight(rhsStr));
}

Name& Name::operator=(const Name& rhs) {
  Set(rhs);
  return *this;
}

Name& Name::operator=(const String& rhs) {
  Set(rhs);
  return *this;
}

Name& Name::operator=(const Cstr* rhs) {
  Set(rhs);
  return *this;
}

void Name::ReadFrom(Stream* stream) {
  StreamReadFlat(stream, index_);
}

void Name::WriteTo(Stream* out_stream) {
  StreamWriteFlat(out_stream, index_);
}

bool Name::IsEqual(const Cstr* rhs) const {
  return string_util::IsEqual(this->GetConst(), rhs);
}

} // namespace dg
