// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "core_first.h"

namespace dg {

StringStream::StringStream(const Cstr * value) :
  buffer_(const_cast<Cstr*>(value)),
  position_(0) {}

const Cstr* StringStream::Separators() const {
  return TXT(" ,\t\n");
}

bool StringStream::ReadFloat(float& out_value) {
  Cstr temp[kMaxShortText];
  int token_size = 0;
  string_util::GetToken(&buffer_[position_], Separators(), token_size);
  if (token_size <= 0) {
    DG_UNREACHABLE();
    return false;
  }
  MyMemCpy(temp, buffer_ + position_, token_size*sizeof(Cstr));
  temp[token_size] = TXT('\0');
  out_value = (float)MyAtof(temp); // Doing atof() with bare buffer_ takes long
  position_ += token_size+1;
  return true;
}

bool StringStream::ReadInt(int& out_value) {
  Cstr temp[kMaxShortText];
  int token_size = 0;
  string_util::GetToken(&buffer_[position_], Separators(), token_size);
  if (token_size <= 0) {
    DG_UNREACHABLE();
    return false;
  }
  MyMemCpy(temp, buffer_ + position_, token_size*sizeof(Cstr));
  temp[token_size] = TXT('\0');
  out_value = MyAtoi(buffer_ + position_); // Doing atoi() with bare buffer_ takes long
  position_ += token_size+1;
  return true;
}

bool StringStream::ReadClassTo(String& out_value) {
  int token_size = 0;
  string_util::GetToken(&buffer_[position_], Separators(), token_size);
  if (token_size <= 0) {
    DG_UNREACHABLE();
    return false;
  }
  String token;
  token.CopyFromRange(buffer_, position_, token_size);
  out_value.Set(token);
  position_ += token_size+1;
  return true;
}

} // namespace dg
