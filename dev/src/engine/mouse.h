// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

enum MouseButtonState {
    MouseButtonState_None = 0,
    MouseButtonState_LeftButton = 1,
    MouseButtonState_MiddleButton = 1 << 1,
    MouseButtonState_RightButton = 1 << 2,
};

struct Mouse {
  float last_x;
  float last_y;
  float x;
  float y;
  float delta_x;
  float delta_y;
  enum MouseButtonState button_state;
  enum MouseButtonState last_button_state;

  Mouse() {
    last_x = last_y = x = y = delta_x = delta_y = 0;
    last_button_state = button_state = MouseButtonState_None;
  }

  void Update(int new_x, int new_y, enum MouseButtonState new_button_state);
};

} // namespace dg
