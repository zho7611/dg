// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

LineDrawer dg::g_line_drawer;

LineDrawer::LineDrawer() {
  Check(!effect_line_);
}

LineDrawer::~LineDrawer() {
  Clear();
}

void LineDrawer::Init() {
  effect_line_ = LoadResource<LineShaderEffect>(g_engine_config->line_effect_path_.Get());
}

void LineDrawer::Clear() {
  effect_line_ = NULL;
}

void LineDrawer::PreRender() {
  if (!effect_line_) {
    Init();
  }
  effect_line_->PreRender();
}

void LineDrawer::PostRender() {
  effect_line_->PostRender();
}

void LineDrawer::DrawLine(
    const Vector3& start_in_world_coord,
    const Vector3& end_in_world_coord,
    const ColorInt& color) {
  effect_line_->SubmitLine(start_in_world_coord, end_in_world_coord, color);
}

} // namespace dg
