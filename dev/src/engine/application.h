// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class View;
class Scene;
class Camera;

#if defined(DG_TARGET_WIN32)
typedef HWND WindowHandle;
#else
typedef int WindowHandle;
#endif

class Application : public Resource {
public:
  Application();
  virtual ~Application();
  virtual bool Init(const Cstr* name, const Cstr* config_file);
  virtual void Clear();
  void ResetRenderer();
  void ResizeWindowSize(const Size2& size);
  void ResizeScreenSize(const Size2& size);
  Size2 GetClientRectSize() const;
  Size2 GetDesiredScreenSize() const;
  virtual const Cstr* GetApplicationName() const {
    return TXT("MyApplication");
  }
  virtual void ReadFrom(Stream* stream);
  virtual void WriteTo(Stream* out_stream);
  virtual void Run();
  virtual void PreRender();
  virtual void Render();
  virtual void PostRender();
  virtual void Update();
  virtual bool PresentScreen();
  virtual void OnMouseUpdate(int x, int y, MouseButtonState button_state) {}
  virtual void OnMouseWheel(int deltaZ) {}
  virtual void OnKeyDown(KeyCode code) {}
  virtual void OnSize(int width, int height);
  virtual void SetupDefaultView(Scene* scene, View* view, Camera* camera, Camera* shadow_camera);
  virtual void SetupDefaultLightView(Scene* scene, View* shadow_view, Camera* shadow_camera);
  // Get system dependent window handle
  static WindowHandle GetWindowHandle();

  Ptr<View> view_;
  Ptr<View> light_view_;
  Size2 screen_size_;
  String title_;
  bool is_full_screen_;
  bool is_activated_;
  bool is_quit_;
  bool is_show_;

private:
  void FillInitDesc() const;
  virtual bool Dispatch();
  void RenderNormalTechnique();
  void RenderShadowTechnique();
  void RenderDepthTechnique();
  void SetDepthTexture();
  void SetShadowTexture();

  DG_DECLARE_RESOURCE(Application, Resource);
};

extern Application* g_app;

int ApplicationMain();

} // namespace dg
