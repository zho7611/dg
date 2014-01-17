// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

Renderer::Renderer() :
    is_device_lost_(false),
    phase_(Phase_Normal),
    is_wire_mode(0),
    render_target(NULL),
    is_in_begin_scene_(false) {
}

Renderer::~Renderer() {}

void Renderer::InitInternal() {}

void Renderer::ClearInternal() {}

#if defined(DG_TARGET_ANDROID)
bool Renderer::InitDeviceInternal() {
  EGLint kAttributes[] = {
    EGL_RED_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    EGL_BLUE_SIZE, 8,
    EGL_DEPTH_SIZE, 16,
    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
    EGL_NONE
  };
  EGLint kContextAttributes[] = {
    EGL_CONTEXT_CLIENT_VERSION, 2,
    EGL_NONE
  };
  EGLConfig configurations[1];
  configurations[0] = 0;
  EGLint configs;
  display_handle_gl2_ = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  if (eglGetError() != EGL_SUCCESS) {
    return false;
  }
  eglInitialize(display_handle_gl2_, NULL, NULL);
  if (eglGetError() != EGL_SUCCESS) {
    return false;
  }
  eglChooseConfig(display_handle_gl2_, kAttributes, configurations, 1, &configs);
  if (configs == 0) {
    return false;
  }
  HWND hwnd = Application::GetWindowHandle();
  window_surface_gl2_ = eglCreateWindowSurface(display_handle_gl2_, configurations[0], (EGLNativeWindowType)hwnd, NULL);
  if (window_surface_gl2_ == EGL_NO_SURFACE) {
    return false;
  }
  render_context_gl2_ = eglCreateContext(display_handle_gl2_, configurations[0], EGL_NO_CONTEXT, kContextAttributes);
  if (render_context_gl2_ == EGL_NO_CONTEXT) {
    return false;
  }
  eglMakeCurrent(display_handle_gl2_, window_surface_gl2_, window_surface_gl2_, render_context_gl2_);
  if (eglGetError() != EGL_SUCCESS) {
    return false;
  }
  const GLubyte* version = glGetString(GL_VERSION);
  const GLubyte* extensions = glGetString(GL_EXTENSIONS);
  return true;
}
#elif defined(DG_TARGET_WIN32) // DG_TARGET_ANDROID
static bool CheckExtensionGl2() {
  String version((const char*)glGetString(GL_VERSION));
  String extension((const char*)glGetString(GL_EXTENSIONS));
  DG_LOG_LINE(TXT("checkextensiongl2: version(%s)"), version.Get());
  DG_LOG(extension.Get());
  DG_LOG(String(string_util::kLineEndStr).Get());
  return true;
}

bool Renderer::InitDeviceInternal() {
  static PIXELFORMATDESCRIPTOR pfd = {
    sizeof(PIXELFORMATDESCRIPTOR),  // Size Of This Pixel Format Descriptor
    1,                              // Version Number
    PFD_DRAW_TO_WINDOW |            // Format Must Support Window
    PFD_SUPPORT_OPENGL |            // Format Must Support OpenGL
    PFD_DOUBLEBUFFER,               // Must Support Double Buffering
    PFD_TYPE_RGBA,                  // Request An RGBA Format
    32,                             // Select Our Color Depth
    0, 0, 0, 0, 0, 0,               // Color Bits Ignored
    0,                              // No Alpha Buffer
    0,                              // Shift Bit Ignored
    0,                              // No Accumulation Buffer
    0, 0, 0, 0,                     // Accumulation Bits Ignored
    16,                             // 16Bit Z-Buffer (Depth Buffer)
    0,                              // No Stencil Buffer
    0,                              // No Auxiliary Buffer
    PFD_MAIN_PLANE,                 // Main Drawing Layer
    0,                              // Reserved
    0, 0, 0                         // Layer Masks Ignored
  };
  HDC hdc = ::GetDC(Application::GetWindowHandle());
  GLuint pixelFormat = ::ChoosePixelFormat(hdc, &pfd);
  if (pixelFormat == 0) {
    DG_LOG_LINE(TXT("error: renderer.opendeviceinternal.failed: choosepixelformat.failed"));
    return false;
  }
  if (!::SetPixelFormat(hdc, pixelFormat, &pfd)) {
    DG_LOG_LINE(TXT("error: renderer.opendeviceinternal.failed: setpixelformat.failed"));
    return false;
  }
  render_context_gl2_ = wglCreateContext(hdc);
  if (render_context_gl2_ == NULL) {
    DG_LOG_LINE(TXT("error: renderer.opendeviceinternal.failed: wglcreatecontext.failed"));
    return false;
  }
  if (wglMakeCurrent(hdc, render_context_gl2_) == false) {
    DG_LOG_LINE(TXT("error: renderer.opendeviceinternal.failed: wglmakecurrent.failed"));
    return false;
  }
  int loadgen_result = ogl_LoadFunctions();
  if (loadgen_result == ogl_LOAD_FAILED) {
    DG_LOG_LINE(TXT("error: renderer.opendeviceinternal.failed: glloadfunction.failed"));
    return false;
  }
  if (loadgen_result > ogl_LOAD_SUCCEEDED) {
    DG_LOG_LINE(TXT("renderer.opendeviceinternal.warning: glloadfunction.num_failed:%d"), loadgen_result-ogl_LOAD_SUCCEEDED);
  }
  loadgen_result = wgl_LoadFunctions(hdc);
  if (loadgen_result == ogl_LOAD_FAILED) {
    DG_LOG_LINE(TXT("error: renderer.opendeviceinternal.failed: wglloadfunction.failed"));
    return false;
  }
  if (loadgen_result > ogl_LOAD_SUCCEEDED) {
    DG_LOG_LINE(TXT("renderer.opendeviceinternal.warning: wglloadfunction.num_failed:%d"), loadgen_result-ogl_LOAD_SUCCEEDED);
  }
  if (!CheckExtensionGl2()) {
    DG_LOG_LINE(TXT("error: renderer.opendeviceinternal.failed: checkextension.failed"));
    return false;
  }
  DG_LOG_LINE(TXT("renderer.opendeviceinternal: ok"));
  return true;
}
#endif // DG_TARGET_WIN32

bool Renderer::ClearDeviceInternal() {
  DG_LOG_LINE(TXT("renderer.closedeviceinternal: ok"));
  return true;
}

bool Renderer::OnInitDevice() {
  DG_LOG_LINE(TXT("renderer.initdevice: ok"));
  return true;
}

bool Renderer::OnClearDevice() {
  DG_LOG_LINE(TXT("renderer.cleardevice: ok"));
  return true;
}

void Renderer::BeginScene() {
  Check(!is_in_begin_scene_);
  if (!is_in_begin_scene_) {
    is_in_begin_scene_ = true;
  }
}

void Renderer::EndScene() {
  Check(is_in_begin_scene_);
  if (is_in_begin_scene_) {
    is_in_begin_scene_ = false;
  }
}

bool Renderer::ResetRendererDevice() {
  UnbindUnmanagedResource();
  resources.ClearDeviceAll();
  resources.InitDeviceAll();
  BindUnmanagedResource();
  DG_LOG_LINE(TXT("renderer.resetrendererdevice: ok"));
  return true;
}

} // namespace dg
