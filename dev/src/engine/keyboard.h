// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

#ifdef DG_TARGET_WIN32
enum KeyCode {
    KeyCode_None = 0,
    KeyCode_Escape = VK_ESCAPE,
    KeyCode_Space = VK_SPACE,
    KeyCode_Up = VK_UP,
    KeyCode_Down = VK_DOWN,
    KeyCode_Left = VK_LEFT,
    KeyCode_Right = VK_RIGHT,
    KeyCode_LeftShift = VK_LSHIFT,
    KeyCode_RightShift = VK_RSHIFT,
    KeyCode_LeftControl = VK_LCONTROL,
    KeyCode_RightControl = VK_RCONTROL,
};
#else
enum KeyCode {
};
#endif // #ifdef DG_TARGET_WIN32

bool IsKeyPressed(KeyCode key);

} // namespace dg
