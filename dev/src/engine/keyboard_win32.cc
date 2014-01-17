// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

bool IsKeyPressed(KeyCode key) {
  if (::GetAsyncKeyState(key) < 0) {
    return true;
  }
  return false;
}

} // namespace dg
