// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "core_first.h"

namespace dg {

void Interlocked::Increment() {
  __sync_fetch_and_add(&count_, 1);
}

void Interlocked::Decrement() {
  __sync_fetch_and_sub(&count_, 1);
}

} // namespace dg
