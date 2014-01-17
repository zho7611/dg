// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

RenderTarget::RenderTarget() :
  color_format_(GpuFormat_Unknown),
  depth_format_(GpuFormat_Unknown) {
}

RenderTarget::~RenderTarget() {
}

void RenderTarget::SetCurrent() {
  BindResource();
  if (!size_.IsZero()) {
    Viewport viewport;
    viewport.top_left_x = viewport.top_left_y = 0;
    viewport.width = this->size_.x;
    viewport.height = this->size_.y;
    viewport.min_depth = 0.0f;
    viewport.max_depth = 1.0f;
    g_renderer.viewport = viewport;
    RC_SetViewport(viewport);
    g_renderer.render_target = this;
    RC_SetRenderTargetWithDepth(0, device_render_target_handle_, device_depth_surface_handle_);
  }
}

bool RenderTarget::HasDepthSurface() const {
  return device_depth_surface_handle_;
}

bool RenderTarget::IsForRenderer() const {
  return (g_renderer.view_state.target.ptr() == this);
}

void RenderTarget::Init() {
  if (!IsForRenderer()) {
    texture_ = new Texture;
    int mip_levels = 1;
#if defined(DG_RENDERER_GL2)
    // #fixme: gl needs full mipmaps for render target texture?
    mip_levels = 0;
#endif
    texture_->Init(GpuUsage_RenderTarget, size_, color_format_, mip_levels);
  } else {
    Check(!texture_);
  }
  OnReadDone();
}

void RenderTarget::Init(
    const Size2& size_in_pixel,
    GpuFormat color_format,
    GpuFormat depth_format) {
  size_ = size_in_pixel;
  color_format_ = color_format;
  depth_format_ = depth_format;
  Init();
}

void RenderTarget::Clear() {
  UnbindResource();
  resource_state_.SetLoaded(false);
  texture_ = 0;
}

void RenderTarget::set_size(const Size2& size) {
  Clear();
  size_ = size;
  Init();
}

bool RenderTarget::OnInitDevice() {
  if (device_render_target_handle_) {
    return true;
  }
  if (IsForRenderer()) {
    return CreateSurfacesForRenderer();
  }
  if (!CreateSurfaces()) {
    return false;
  }
  return true;
}

bool RenderTarget::OnClearDevice() {
  if (!device_render_target_handle_) {
    return false;
  }
  if (texture_) {
    // Texture_ in RenderTarget is not controlled by g_renderer.resources
    texture_->OnClearDevice();
    texture_->resource_state().SetBound(false);
  }
  device_render_target_handle_.Release();
  device_depth_surface_handle_.Release();
  return true;
}

bool RenderTarget::CreateSurfaces() {
  Check(!device_render_target_handle_);
  Check(!device_depth_surface_handle_);
  Check(!size_.IsZero());
  // Create a color surface from the texture
  {
    // The texture depends on the render target
    // Its resource state is controlled by the render target itself
    texture_->OnInitDevice();
    texture_->resource_state().SetBound(true);
    const int level = 0;
    RC_CreateRenderTargetFromTexture(texture_->device_texture_handle(), level, device_render_target_handle_);
  }
  // Create a depth buffer
  {
    Check(!device_depth_surface_handle_);
    const int kMultiSampleType = 0;
    const int kMultiSampleQuality = 0;
    RC_CreateDepthStencilSurface(size_.x, size_.y, depth_format_, kMultiSampleType, kMultiSampleQuality, device_depth_surface_handle_);
  }
  return true;
}

bool RenderTarget::CreateSurfacesForRenderer() {
  Check(!device_render_target_handle_);
  Check(!device_depth_surface_handle_);
  // Set the color surface
  color_format_ = g_renderer.init_desc.color_format;
  size_ = g_renderer.init_desc.screen_size;
  RC_GetRenderTarget(0, device_render_target_handle_);
  depth_format_ = g_renderer.init_desc.depth_format;
  RC_GetDepthStencilSurface(device_depth_surface_handle_);
  Check(!texture_);
  return true;
}

// Write the render target in a raw format
void RenderTarget::WriteTo(Stream* out_stream) {
  const Size2& kSize = size();
  // Create the intermediate lockable system-mem surface
  SurfaceHandle lockable_surface;
  if (!RC_CreateOffscreenPlainSurface(lockable_surface, kSize.x, kSize.y, color_format_)) {
    return;
  }
  // Get the render target data into the lockable surface
  if (!RC_GetRenderTargetData(device_render_target_handle_, lockable_surface)) {
    return;
  }
  uint8_t* buffer = NULL;
  int pitch = 0;
  if (!RC_LockSurfarce(lockable_surface, buffer, pitch)) {
    return;
  }
  const int kBufferSize = pitch * kSize.y;
  // Copy the surface in raw format
  out_stream->WriteBytes(buffer, kBufferSize);
  // Unlock and release
  RC_UnlockSurface(lockable_surface);
  lockable_surface.Release();
}

} // namespace dg
