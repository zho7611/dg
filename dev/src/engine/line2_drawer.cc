// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

Line2Drawer dg::g_line2d_drawer;

Line2Drawer::Line2Drawer() {}

Line2Drawer::~Line2Drawer() {}

void Line2Drawer::Init() {
  effect_line2_ = LoadResource<Line2ShaderEffect>(g_engine_config->line2_effect_path_.Get());
}

void Line2Drawer::Clear() {
  effect_line2_ = NULL;
}

void Line2Drawer::PreRender() {
  if (!effect_line2_) {
    Init();
  }
  effect_line2_->PreRender();
}

void Line2Drawer::PostRender() {
  effect_line2_->PostRender();
}

void Line2Drawer::DrawLine2(
    const Point2& start_in_screen_coord,
    const Point2& end_in_screen_coord,
    const ColorInt& color) {
  effect_line2_->SubmitLine(start_in_screen_coord, end_in_screen_coord, color);
}

} // namespace dg
