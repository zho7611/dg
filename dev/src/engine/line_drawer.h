// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class LineShaderEffect;

// Line drawer in 3d world coordinates
class LineDrawer {
public:
  LineDrawer();
  ~LineDrawer();
  void Init();
  void Clear();
  void PreRender();
  void PostRender();
  void DrawLine(
      const Vector3& start_in_world_coord,
      const Vector3& end_in_world_coord,
      const ColorInt& color);

private:
  Ptr<LineShaderEffect> effect_line_;
};

extern LineDrawer g_line_drawer;

} // namespace dg
