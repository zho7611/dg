// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

// Excerpted from <zmouse.h>
#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL (WM_MOUSELAST+1) // Message that will be supported by the OS
#endif

void Mouse::Update(int new_x, int new_y, MouseButtonState new_button_state) {
  last_button_state = button_state;
  button_state = new_button_state;
  x = (float)new_x;
  y = (float)new_y;
  delta_x = x - last_x;
  delta_y = y - last_y;
  last_x = x;
  last_y = y;
}

} // namespace dg
