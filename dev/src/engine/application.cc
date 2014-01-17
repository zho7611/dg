// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

Application* dg::g_app = NULL;

void Application::ReadFrom(Stream* stream) {
  StreamReadClass(stream, view_);
  StreamReadClass(stream, light_view_);
  StreamReadClass(stream, screen_size_);
  StreamReadClass(stream, title_);
  StreamReadFlat(stream, is_full_screen_);
  StreamReadFlat(stream, is_show_);
  OnReadDone();
}

void Application::WriteTo(Stream* out_stream) {
  StreamWriteClass(out_stream, view_);
  StreamWriteClass(out_stream, light_view_);
  StreamWriteClass(out_stream, screen_size_);
  StreamWriteClass(out_stream, title_);
  StreamWriteFlat(out_stream, is_full_screen_);
  StreamWriteFlat(out_stream, is_show_);
  OnWriteDone();
}

void Application::Clear() {
  Check(is_quit_);
  light_view_ = 0;
  view_ = 0;
  g_renderer.UnbindResource();
  g_renderer.UnbindUnmanagedResource();
  g_renderer.ClearRenderDevice();
  g_engine.Clear();
}

void Application::Update() {
  if (view_) {
    view_->Update();
  }
  if (light_view_) {
    light_view_->Update();
  }
}

void Application::RenderShadowTechnique() {
  if (light_view_) {
    ScopedShadowTechnique technique;
    // #csm
    // Get properly divided viewports
    light_view_->PreRender(&technique);
    for (int idx = 0; idx < kNumShadowCascades; idx++) {
      light_view_->SetCascadeIndex(idx);
      Camera::UpdateAsLightCamera(
          view_->camera(),
          view_->light_camera(),
          light_view_->view_size);
      light_view_->Render(&technique);
    }
    light_view_->PostRender(&technique);
  }
}

void Application::RenderDepthTechnique() {
  if (view_) {
    RenderTarget* depth_target = view_->depth_target();
    if (depth_target && !depth_target->size().IsZero()) {
      ScopedDepthTechnique technique;
      depth_target->SetCurrent();
      view_->PreRender(&technique);
      view_->Render(&technique);
      view_->PostRender(&technique);
    }
  }
}

void Application::RenderNormalTechnique() {
  Check(view_);
  if (view_) {
    g_renderer.view_state.light_view = light_view_.ptr();
    ScopedNormalTechnique technique;
    view_->PreRender(&technique);
    view_->Render(&technique);
    view_->PostRender(&technique);
  }
}

void Application::PreRender() {}

void Application::Render() {
  if (this->screen_size_.IsZero()) return;
  {
    DG_SCOPED_PROFILER(TXT("app.render.depth"), DG_MAKE_COLOR_ARGB(0xfff0ad44));
    RenderDepthTechnique();
  }
  {
    DG_SCOPED_PROFILER(TXT("app.render.shadow"), DG_MAKE_COLOR_ARGB(0xff128085));
    RenderShadowTechnique();
  }
  {
    DG_SCOPED_PROFILER(TXT("app.render.normal"), DG_MAKE_COLOR_ARGB(0xffc74029));
    RenderNormalTechnique();
  }
  {
    DG_SCOPED_PROFILER(TXT("app.render.present"), DG_MAKE_COLOR_ARGB(0xff8ac2b0));
    PresentScreen();
  }
}

void Application::PostRender() {}

void Application::Run() {
  is_quit_ = false;
  g_renderer.timer.Start();
  while (!is_quit_) {
    if (!Dispatch()) {
      Check(!is_quit_);
      g_renderer.timer.TickFrame();
      if (is_activated_) {
        PreRender();
        Render();
        PostRender();
      }
      Os::SleepMilliSeconds(0);
    }
  }
}

void Application::FillInitDesc() const {
  GpuInitDesc& desc = g_renderer.init_desc;
  desc.is_full_screen = is_full_screen_;
  desc.screen_size = desc.buffer_size = screen_size_;
  desc.refresh_rate = 0;
}

bool Application::PresentScreen() {
  if (!g_renderer.PresentScreen()) {
    if (!g_renderer.ResetRendererDevice()) {
      DG_LOG_LINE(TXT("error: application.presentscene.restorefailed:"));
      return false;
    }
    DG_LOG_LINE(TXT("application.presentscene.restored:"));
    return true;
  }
  return true;
}

Size2 Application::GetDesiredScreenSize() const {
  if (is_full_screen_) {
    return g_engine_config->full_screen_screen_size_;
  }
  return g_engine_config->windowed_screen_size_;
}

void Application::ResizeScreenSize(const Size2& size) {
  screen_size_ = size;
  if (view_) {
    view_->ResetSize(size);
    ResetRenderer();
  }
}

// This is called after the application window is resized
void Application::OnSize(int width, int height) {
  if ((width == 0) || (height == 0)) return;
  Size2 client_rect_size = g_app->GetClientRectSize();
  g_app->ResizeScreenSize(client_rect_size);
}

void Application::SetupDefaultView(Scene* scene, View* in_view, Camera* camera, Camera* light_camera) {
  Check(!this->view_);
  this->view_ = in_view;
  this->view_->clear_color.FromColorInt(g_engine_config->background_color_);
  this->view_->set_scene(scene);
  this->view_->set_camera(camera);
  this->view_->set_light_camera(light_camera);
  this->view_->view_size = screen_size_;
  this->view_->OnReadDone();
  this->view_->Init();
  // Setup post effect blur
  {
    PostEffectBlur* post_effect_blur = new PostEffectBlur;
    post_effect_blur->Init(screen_size_);
    this->view_->set_post_effect(post_effect_blur);
  }
}

void Application::SetupDefaultLightView(Scene* scene, View* in_light_view, Camera* light_camera) {
  Check(!this->light_view_);
  Check(light_camera);
  Check(light_camera->is_light_camera);
  Check(light_camera->is_orthogonal);
  this->light_view_ = in_light_view;
  this->light_view_->is_enabled = g_engine_config->is_shadow_enabled_;
  this->light_view_->set_scene(scene);
  this->light_view_->set_camera(light_camera);
  this->light_view_->set_light_camera(NULL);
  this->light_view_->clear_color.FromColorInt(DG_MAKE_COLOR_INT(255, 255, 255, 255));
  this->light_view_->view_size = g_engine_config->shadow_map_size_;
  this->light_view_->OnReadDone();
  this->light_view_->Init();
  PostEffectShadow* post_effect_shadow = new PostEffectShadow;
  post_effect_shadow->Init(this->light_view_->view_size);
  this->light_view_->set_post_effect(post_effect_shadow);
}

} // namespace dg
