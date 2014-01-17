// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

void AxisDrawer::Draw(Vector3& position, float length) {
  const ColorInt kXAxisColor = DG_MAKE_COLOR_INT(255, 255, 0, 0);
  const ColorInt kYAxisColor = DG_MAKE_COLOR_INT(255, 0, 255, 0);
  const ColorInt kZAxisColor = DG_MAKE_COLOR_INT(255, 0, 0, 255);
  LineDrawer& drawer = g_line_drawer;
  drawer.PreRender();
  {
    Vector3 origin(position);
    drawer.DrawLine(origin, origin + Vector3(length, 0, 0), kXAxisColor);
    drawer.DrawLine(origin, origin + Vector3(0, length, 0), kYAxisColor);
    drawer.DrawLine(origin, origin + Vector3(0, 0, length), kZAxisColor);
  }
  drawer.PostRender();
}

} // namespace dg
