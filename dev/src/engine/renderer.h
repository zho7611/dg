// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class View;
class Camera;
class Light;
class RenderTarget;
class MeshBuilder;

struct ViewState {
  // View and light_view are not owned by ViewState.
  // They should be defined as raw pointers rather than smart pointers.
  // With smart pointers, they won't be deallocated when the memory allocator finished.
  View* view;
  View* light_view;
  // Owns the render target so that it should be defined as a smart pointer
  Ptr<RenderTarget> target;
  Matrix world_matrix;
};

struct GpuInitDesc {
  Size2 screen_size;
  Size2 buffer_size;
  int refresh_rate;
  int adapter;
  bool is_full_screen;
  bool is_vertical_sync;
  GpuFormat color_format;
  GpuFormat depth_format;

  GpuInitDesc();
};

struct Viewport {
  int top_left_x;
  int top_left_y;
  int width;
  int height;
  float min_depth;
  float max_depth;
};

// Should be matched with #default-texture-orders
#define DG_NUM_RENDERER_DEFAULT_TEXTURES (6)

class Renderer {
public:
  // For now, the technique value is equal to the technique index in the simple_effect.json
  enum Phase {
    Phase_None = -1,
    Phase_Depth = 0,
    Phase_Shadow = 1,
    Phase_Normal = 2,
  };
  // Should be matched with #default-texture-orders
  enum DefaultTextures {
    DefaultTextures_Black = 0,
    DefaultTextures_White = 1,
    DefaultTextures_Red = 2,
    DefaultTextures_Green = 3,
    DefaultTextures_Blue = 4,
    DefaultTextures_NormalPurple = 5, // R:128, G:128, B:255
  };

  GpuInitDesc init_desc;
  ViewState view_state;
  bool is_wire_mode;
  FrameTimer timer;
  ResourceChecker resources;
  RenderStates states;
  MeshBuilder mesh_builder;
  Sprite opaque_sprite;
  Sprite translucent_sprite;
  Sprite font_sprite;
  // Default texture presets
  Texture* default_textures[DG_NUM_RENDERER_DEFAULT_TEXTURES];
  struct Viewport viewport;
  RenderTarget* render_target;

  Renderer();
  ~Renderer();
  void Init();
  void Clear();
  bool InitRenderDevice();
  bool ClearRenderDevice();
  void BindResource();
  void UnbindResource();
  void BindUnmanagedResource();
  void UnbindUnmanagedResource();
  bool BeginFrame();
  bool EndFrame();
  void BeginScene();
  void EndScene();
  bool ClearScreen(ColorInt clear_color, bool is_depth_clear);
  bool PresentScreen();
  const Size2& GetScreenSize() const;
  bool ResetRendererDevice();
  bool GetIsInBeginScene() const { return is_in_begin_scene_; }
  Phase phase() const { return phase_; }
  void set_phase(Phase phase) { phase_ = phase; }
  unsigned int frame_id() const { return frame_id_; }

private:
  unsigned int frame_id_;
  bool is_device_lost_;
  bool is_in_begin_scene_;
  Phase phase_;

  void InitInternal();
  void ClearInternal();
  bool OnInitDevice();
  bool OnClearDevice();
  bool InitDeviceInternal();
  bool ClearDeviceInternal();

#if defined(DG_RENDERER_DX9)
public:
  IDirect3DDevice9* device_dx9_;

private:
  D3DPRESENT_PARAMETERS present_params_dx9_;
  IDirect3D9* d3d9_;

  void InitPresentParamsDx9();
  void InitFormatsDx9();
  bool CreateDeviceDx9();
  void DestroyDeviceDx9();
#elif defined(DG_RENDERER_GL2)
public:
#if defined(DG_TARGET_ANDROID)
  EGLDisplay  display_handle_gl2_;
  EGLContext  render_context_gl2_;
  EGLSurface  window_surface_gl2_;
#else
  HGLRC render_context_gl2_;
#endif // DG_TARGET_ANDROID
#endif // #if defined(DG_RENDERER_DX9)
};

extern Renderer g_renderer;

} // namespace dg
