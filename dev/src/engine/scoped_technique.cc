// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

void ScopedDepthTechnique::PreRender(View* view, bool is_clear_scene) {
  DG_UNREF(view);
  if (is_clear_scene) {
    bool is_clear_depth = true;
    g_renderer.ClearScreen(DG_MAKE_COLOR_INT(255, 0, 0, 0), is_clear_depth);
  }
}

void ScopedDepthTechnique::PostRender(View* view) {
  DG_UNREF(view);
}

void ScopedNormalTechnique::PreRender(View* view, bool is_clear_scene) {
  bool is_clear_depth = true;
  if (g_engine_config->is_post_process_blur_ && view->post_effect() && view->post_effect()->is_enabled_) {
    view->post_effect()->PreRender();
    is_clear_depth = view->post_effect()->HasDepthSurface();
  } else {
    g_renderer.view_state.target->SetCurrent();
  }
  if (is_clear_scene) {
    g_renderer.ClearScreen(view->clear_color.ToColorInt(), is_clear_depth);
  }
}

void ScopedNormalTechnique::PostRender(View* view) {
  if (g_engine_config->is_post_process_blur_ && view->post_effect() && view->post_effect()->is_enabled_) {
    view->post_effect()->PostRender();
  }
  view->DrawPostRender();
}

void ScopedShadowTechnique::PreRender(View* view, bool is_clear_scene) {
  bool is_clear_depth = true;
  if (view->post_effect() && view->post_effect()->is_enabled_) {
    view->post_effect()->PreRender();
    is_clear_depth = view->post_effect()->HasDepthSurface();
  }
  if (is_clear_scene) {
    g_renderer.ClearScreen(view->clear_color.ToColorInt(), is_clear_depth);
  }
}

void ScopedShadowTechnique::PostRender(View* view) {
  if (view->post_effect() && view->post_effect()->is_enabled_) {
    view->post_effect()->PostRender();
  }
}

} // namespace dg
