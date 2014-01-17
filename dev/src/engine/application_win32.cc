// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

#if defined(DG_TARGET_WIN32)
struct ApplicationContextWin32 {
  HINSTANCE hinstance;
  HICON hicon;
  HCURSOR hcursor;
  HWND hwnd;
  UINT window_style;
  HMENU hmenu;
  RECT window_rect;

  ApplicationContextWin32();
  virtual LRESULT WindowProcedure(
      HWND hwnd, UINT message,
      WPARAM wparam, LPARAM lparam);
  void SetupWindow(const Cstr* class_name, Size2 screen_size, bool is_full_screen, bool is_show);
  Size2 GetWindowSizeFitToClientRectSize(Size2 screen_size);
  Size2 GetClientRectSize();
  void ResizeWindow(int width, int height, bool is_full_screen);
};
#endif // #if defined(DG_TARGET_WIN32)

ApplicationContextWin32 g_app_context_win32;

WindowHandle Application::GetWindowHandle() {
  return g_app_context_win32.hwnd;
}

LRESULT CALLBACK WindowProc(
    HWND hwnd, UINT message,
    WPARAM wparam, LPARAM lparam) {
  return dg::g_app_context_win32.WindowProcedure(hwnd, message, wparam, lparam);
}

ApplicationContextWin32::ApplicationContextWin32() :
    hwnd(0),
    hicon(0),
    hcursor(0),
    hinstance(0),
    hmenu(0),
    window_style(WS_OVERLAPPEDWINDOW) {}

LRESULT ApplicationContextWin32::WindowProcedure(
    HWND hwnd,
    UINT message,
    WPARAM wparam,
    LPARAM lparam) {
  Check(!hwnd || (hwnd == hwnd));
  Check(g_app);
  static int s_width = 0;
  static int s_height = 0;
  switch (message) {
  case WM_ACTIVATEAPP:
    g_app->is_activated_ = (wparam == TRUE) ? true : false;
    break;
  case WM_MOUSEMOVE:
  case WM_LBUTTONDOWN:
  case WM_LBUTTONUP:
  case WM_MBUTTONDOWN:
  case WM_MBUTTONUP:
  case WM_RBUTTONDOWN:
  case WM_RBUTTONUP: {
    MouseButtonState button_state = MouseButtonState_None;
    if (wparam & MK_LBUTTON) EnumWrite(button_state, MouseButtonState_LeftButton);
    if (wparam & MK_MBUTTON) EnumWrite(button_state, MouseButtonState_MiddleButton);
    if (wparam & MK_RBUTTON) EnumWrite(button_state, MouseButtonState_RightButton);
    int x = (int)LOWORD(lparam);
    int y = (int)HIWORD(lparam);
    g_engine.mouse.Update(x, y, button_state);
    g_app->OnMouseUpdate(x, y, button_state);
    break;
  }
  case WM_MOUSEWHEEL:
    g_app->OnMouseWheel((short)HIWORD(wparam));
    break;
  case WM_SIZE:
    s_width = LOWORD(lparam);
    s_height = HIWORD(lparam);
    g_app->OnSize(s_width, s_height);
    break;
  case WM_KEYDOWN:
    g_app->OnKeyDown((KeyCode)wparam);
    break;
  case WM_DESTROY:
    ::PostQuitMessage(0);
    break;
  }
  return ::DefWindowProc(hwnd, message, wparam, lparam);
}

Application::Application() :
    screen_size_(320, 240),
    is_quit_(true),
    is_activated_(false),
    view_(0),
    light_view_(0),
    is_show_(true),
    is_full_screen_(false) {
  g_app = this;
}

Application::~Application() {
  Check(is_quit_);
  g_app = NULL;
}

bool Application::Init(
    const Cstr* name,
    const Cstr* config_file) {
  title_ = name;
  if (!g_engine.Init(Engine::Usage_All, config_file)) {
    DG_UNREACHABLE();
    return false;
  }
  is_full_screen_ = g_engine_config->is_full_screen_;
  screen_size_ = GetDesiredScreenSize();
  dg::g_app_context_win32.SetupWindow(
      title_.Get(),
      screen_size_,
      is_full_screen_,
      is_show_);
  FillInitDesc();
  g_renderer.InitRenderDevice();
  g_renderer.BindResource();
  g_renderer.BindUnmanagedResource();
  return true;
}

void ApplicationContextWin32::SetupWindow(
    const Cstr* class_name,
    Size2 screen_size,
    bool is_full_screen,
    bool is_show) {
  WNDCLASS wcl;
  Check(hinstance);
  wcl.hInstance = hinstance;
  wcl.lpszClassName = class_name;
  wcl.lpfnWndProc = WindowProc;
  wcl.style = 0;
  wcl.hIcon = (hicon) ? hicon : ::LoadIcon(hinstance, IDC_ICON);
  wcl.hCursor = (hcursor) ? hcursor : ::LoadCursor(NULL, IDC_ARROW);
  wcl.lpszMenuName = NULL;
  wcl.cbClsExtra = 0;
  wcl.cbWndExtra = 0;
  wcl.hbrBackground = (HBRUSH)::GetStockObject(BLACK_BRUSH);
  ::RegisterClass(&wcl);
  Check(!hwnd);
  hwnd = ::CreateWindowEx(
      0, // WS_EXTOPMOST,
      wcl.lpszClassName,
      wcl.lpszClassName,
      window_style, //WS_POPUP,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      screen_size.x,
      screen_size.y,
      NULL,
      NULL,
      hinstance,
      NULL);
  Check(hwnd);
  ::GetWindowRect(hwnd, &window_rect);
  if (is_show) {
    if (!is_full_screen) {
      Size2 window_size;
      window_size = GetWindowSizeFitToClientRectSize(screen_size);
      dg::g_app_context_win32.ResizeWindow(window_size.x, window_size.y, is_full_screen);
    }
    ::ShowWindow(hwnd, SW_SHOW);
  }
  ::UpdateWindow(hwnd);
}

void ApplicationContextWin32::ResizeWindow(int width, int height, bool is_full_screen) {
  HWND insert_after = HWND_TOPMOST;
  UINT show_flag = SWP_SHOWWINDOW;
  UINT style = 0;
  int x = 0;
  int y = 0;
  if (is_full_screen) {
    style = 0;
    insert_after = HWND_TOPMOST;
    x = y = 0;
  } else {
    style = this->window_style;
    insert_after = HWND_TOP;
    x = this->window_rect.left;
    y = this->window_rect.top;
  }
  ::SetWindowLongPtr(hwnd, GWL_STYLE, style);
  ::SetWindowPos(hwnd, insert_after, 0, 0, width, height, SWP_SHOWWINDOW);
  ::GetWindowRect(hwnd, &window_rect);
}

Size2 ApplicationContextWin32::GetClientRectSize() {
  ::RECT client_rect;
  ::GetClientRect(hwnd, &client_rect);
  return Size2(client_rect.right - client_rect.left, client_rect.bottom - client_rect.top);
}

// Get the the window to make the client_rect be fit to the screen
Size2 ApplicationContextWin32::GetWindowSizeFitToClientRectSize(Size2 screen_size) {
  int x = 0;
  int y = 0;
  int width = 0;
  int height = 0;
  ::RECT desired_rect; // Input: client rect, output: window rect
  desired_rect.left = window_rect.left;
  desired_rect.top = window_rect.top;
  desired_rect.right = desired_rect.left +screen_size.x;
  desired_rect.bottom = desired_rect.top + screen_size.y;
  ::AdjustWindowRect(&desired_rect, g_app_context_win32.window_style, (hmenu)? TRUE : FALSE);
  width = desired_rect.right - desired_rect.left;
  height = desired_rect.bottom - desired_rect.top;
  return Size2(width, height);
}

Size2 Application::GetClientRectSize() const {
  return dg::g_app_context_win32.GetClientRectSize();
}

void Application::ResizeWindowSize(const Size2& size) {
  dg::g_app_context_win32.ResizeWindow(size.x, size.y, is_full_screen_);
}

void Application::ResetRenderer() {
  FillInitDesc();
  g_renderer.ResetRendererDevice();
}

bool Application::Dispatch() {
  MSG msg;
  if (::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
    if (::GetMessage(&msg, NULL, 0, 0)) {
      ::TranslateMessage(&msg);
      ::DispatchMessage(&msg);
      return true;
    }
    is_quit_ = true;
    return true;
  }
  return false;
}

} // namespace dg

int APIENTRY WinMain(
    HINSTANCE hinstance,
    HINSTANCE hprev_instance,
    LPSTR command_line_cstr,
    int command_show) {
  dg::g_app_context_win32.hinstance = hinstance;
  return ApplicationMain();
}
