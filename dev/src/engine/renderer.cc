// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"
#include "../externals/imgui/imgui_render.h"

namespace dg {

Renderer dg::g_renderer;

GpuInitDesc::GpuInitDesc() :
    screen_size(1024, 768),
    buffer_size(1024, 768),
    is_full_screen(false),
    refresh_rate(0),
    is_vertical_sync(true),
    adapter(0),
    color_format(GpuFormat_A8R8G8B8),
    depth_format(GpuFormat_D24S8) {
  Check(screen_size == buffer_size);
}

void Renderer::Init() {
  frame_id_ = 0;
  for (int idx = 0; idx < DG_NUM_RENDERER_DEFAULT_TEXTURES; ++idx) {
    default_textures[idx] = NULL;
  }
  opaque_sprite.sprite_type_ = Sprite::Type_Opaque;
  translucent_sprite.sprite_type_ = Sprite::Type_Translucent;
  font_sprite.sprite_type_ = Sprite::Type_Font;
  font_sprite.Init();
  opaque_sprite.Init();
  translucent_sprite.Init();
  for (int idx = 0; idx < DG_NUM_RENDERER_DEFAULT_TEXTURES; ++idx) {
    Check(g_renderer.default_textures[idx] == NULL);
    g_renderer.default_textures[idx] = new Texture;
    g_renderer.default_textures[idx]->AddRef();
  }
  InitInternal();
}

void Renderer::Clear() {
  ClearInternal();
  for (int idx = 0; idx < DG_NUM_RENDERER_DEFAULT_TEXTURES; ++idx) {
    g_renderer.default_textures[idx]->Release();
    g_renderer.default_textures[idx] = NULL;
  }
  font_sprite.Clear();
  opaque_sprite.Clear();
  translucent_sprite.Clear();
}

const Size2& Renderer::GetScreenSize() const {
  return init_desc.screen_size;
}

bool Renderer::ClearScreen(ColorInt clear_color, bool is_depth_clear) {
  return RC_ClearScreen(clear_color, is_depth_clear);
}

bool Renderer::PresentScreen() {
  frame_id_++;
  g_engine.statistics->IncreaseFrame();
  return RC_PresentScreen();
}

static void BindDefaultTextures() {
  struct ColorPreset { int a; int r; int g; int b;};
  // Should be matched with #default-texture-orders
  ColorPreset presets[] = {
    { 255, 0, 0, 0 }, // Black
    { 255, 255, 255, 255 }, // White
    { 255, 255, 0, 0 }, // Red
    { 255, 0, 255, 0 }, // Green
    { 255, 0, 0, 255 }, // Blue
    { 255, 128, 128, 255 }, // Normal purple
  };
  const Cstr* texture_names[] = {
    TXT("black.tga"),
    TXT("white.tga"),
    TXT("red.tga"),
    TXT("green.tga"),
    TXT("blue.tga"),
    TXT("normalpurple.tga"),
  };
  for (int idx = 0; idx < DG_NUM_RENDERER_DEFAULT_TEXTURES; ++idx) {
    g_renderer.default_textures[idx]->set_resource_name(texture_names[idx]);
    texture_util::InitSolidColorTexture(g_renderer.default_textures[idx], 1, 1,
        DG_MAKE_COLOR_INT(presets[idx].a, presets[idx].r, presets[idx].g, presets[idx].b));
  }
}

static void UnbindDefaultTextures() {
  for (int idx = 0; idx < DG_NUM_RENDERER_DEFAULT_TEXTURES; ++idx) {
    g_renderer.default_textures[idx]->Clear();
  }
}

bool Renderer::InitRenderDevice() {
  if (!InitDeviceInternal()) {
    return false;
  }
  if (!OnInitDevice()) {
    return false;
  }
  return true;
}

bool Renderer::ClearRenderDevice() {
  if (!OnClearDevice()) {
    return false;
  }
  if (!ClearDeviceInternal()) {
    return false;
  }
  return true;
}

void Renderer::BindResource() {
  states.Apply();
  Check(!view_state.target);
  view_state.target = new RenderTarget;
  view_state.target->Init();
  view_state.target->SetCurrent(); // Let it be the current to make it opened-state
  mesh_builder.Init();
}

void Renderer::BindUnmanagedResource() {
  BindDefaultTextures();
  imguiRenderGLInit();
}

void Renderer::UnbindUnmanagedResource() {
  imguiRenderGLDestroy();
  UnbindDefaultTextures();
}

void Renderer::UnbindResource() {
  mesh_builder.Clear();
  g_line_drawer.Clear();
  g_line2d_drawer.Clear();
  view_state.target = 0;
  states.Reset();
}

} // namespace dg
