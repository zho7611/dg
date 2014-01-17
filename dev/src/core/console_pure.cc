// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "core_first.h"

namespace dg {

Console::Console() {}

Console::~Console() {}

bool Console::Open() {
  return true;
}

void Console::Close() {
}

Cstr Console::GetChar() {
  return TXT('\0');
}

void Console::Print(const Cstr* output) {
  DG_UNREF(output);
}

void Console::PrintFormat(const Cstr* format,...) {
  DG_UNREF(format);
}

} // namespace dg
