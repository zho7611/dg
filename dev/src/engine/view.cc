// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

View::View() :
  camera_(0),
  light_camera_(0),
  scene_(0),
  post_effect_(0),
  clear_color(0, 0, 0, 0),
  is_enabled(true),
  view_size(0, 0) {
}

View::~View() {
  Clear();
}

void View::ReadFrom(Stream* stream) {
  DG_UNIMPLEMENTED();
  /*
  StreamReadClass(stream, camera_);
  StreamReadClass(stream, ShadowCamera_);
  StreamReadClass(stream, post_effect_);
  StreamReadClass(stream, scene_);
  StreamReadFlat(stream, clear_color_);
  */
  OnReadDone();
}

void View::WriteTo(Stream* out_stream) {
  DG_UNIMPLEMENTED();
  /*
  StreamWriteClass(out_stream, camera_);
  StreamWriteClass(out_stream, ShadowCamera_);
  StreamWriteClass(out_stream, post_effect_);
  StreamWriteClass(out_stream, scene_);
  StreamWriteFlat(out_stream, clear_color_);
  */
  OnWriteDone();
}

void View::Init() {
  if (!view_size.IsZero()) {
    depth_target_.Init(view_size, GpuFormat_A32B32G32R32F, GpuFormat_D24S8);
  }
}

void View::Clear() {
  light_camera_ = NULL;
  camera_ = NULL;
  post_effect_ = NULL;
  scene_ = NULL;
  depth_target_.UnbindResource();
}

void View::DrawPostRender() {
}

void View::Update() {
  DG_SCOPED_PROFILER(TXT("view.update"), DG_MAKE_COLOR_ARGB(0xff128085));
  if (camera_) {
    camera_->UpdateTransform();
  }
  if (light_camera_) {
    light_camera()->UpdateTransform();
  }
  if (scene_) {
    scene_->UpdateTransform();
  }
}

void View::PreRender(ScopedTechnique* technique) {
  DG_SCOPED_PROFILER(TXT("view.prerender"), DG_MAKE_COLOR_ARGB(0xffffddb4));
  BindResource();
  struct Viewport viewport;
  Check(!view_size.IsZero());
  viewport.top_left_x = 0;
  viewport.top_left_y = 0;
  viewport.width = view_size.x;
  viewport.height = view_size.y;
  viewport.min_depth = 0;
  viewport.max_depth = 1.0f;
  RC_SetViewport(viewport);
  g_renderer.view_state.view = this;
  g_renderer.BeginScene();
  technique->PreRender(this, true);
  if (scene_ && is_enabled) {
    scene_->BindResource();
  }
}

void View::PostRender(ScopedTechnique* technique) {
  DG_SCOPED_PROFILER(TXT("view.postrender"), DG_MAKE_COLOR_ARGB(0xff8ac2b0));
  technique->PostRender(this);
  g_renderer.EndScene();
  g_renderer.view_state.view = 0;
}

void View::Render(ScopedTechnique* technique) {
  DG_SCOPED_PROFILER(TXT("view.render"), DG_MAKE_COLOR_ARGB(0xffc94029));
  g_renderer.view_state.view = this;
  if (scene_ && is_enabled) {
    scene_->Submit();
  }
}

RenderTarget* View::render_target() {
  return (post_effect_) ? post_effect_->render_target() : 0;
}

void View::ResetSize(const Size2& in_view_size) {
  if (view_size != in_view_size) {
    view_size = in_view_size;
    if (camera_) {
      camera_->proj_desc.aspect_ratio =
          float(view_size.x) / float(view_size.y);
    }
    if (post_effect_) {
      post_effect_->SetSize(view_size);
    }
    depth_target_.set_size(view_size);
  }
}

void View::SetCascadeIndex(int cascade_index) {
  camera_->cascade_index = cascade_index;
}

} // namespace dg
