// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "gui_first.h"

namespace dg {

Canvas::Canvas() {
  clear_color_ = DG_MAKE_COLOR_INT(255, 0, 0, 0);
}

void Canvas::ClearCanvas() {
  g_renderer.ClearScreen(clear_color_, target_->HasDepthSurface());
}

void Canvas::PreRender() {
  Check(target_);
  target_->SetCurrent();
}

void Canvas::PostRender() {
}

} // namespace dg
