// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "core_first.h"

namespace dg {

void Interlocked::Increment() {
  ::InterlockedIncrement(&count_);
}

void Interlocked::Decrement() {
  ::InterlockedDecrement(&count_);
}

} // namespace dg
