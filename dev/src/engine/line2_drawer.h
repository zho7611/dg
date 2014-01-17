// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class Line2ShaderEffect;

// Line drawer in 2d coordinates
class Line2Drawer {
public:
  Line2Drawer();
  ~Line2Drawer();
  void Init();
  void Clear();
  void PreRender();
  void PostRender();
  void DrawLine2(
      const Point2& start_in_screen_coord,
      const Point2& end_in_screen_coord,
      const ColorInt& color);

private:
  Ptr<Line2ShaderEffect> effect_line2_;
};

extern Line2Drawer g_line2d_drawer;

} // namespace dg
