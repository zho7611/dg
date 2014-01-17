// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "core_first.h"

namespace dg {

#if !defined(DG_TARGET_WIN32)
CommandLine::CommandLine() {}
#endif

CommandLine::CommandLine(int argc, Cstr** argv) {
  for (int idx = 0; idx < argc; ++idx) {
    options_.PushBack(String(argv[idx]));
  }
}

int CommandLine::GetOptionIndex(const Cstr* option) const {
  for (int idx = 0; idx < options_.Count(); ++idx) {
    if (options_.GetConst(idx).IsEqual(option)) {
      return idx;
    }
  }
  return -1;
}

const Cstr* CommandLine::GetOption(int index) const {
  if (index < options_.Count()) {
    return options_.GetConst(index).GetConst();
  }
  return 0;
}

} // namespace dg
