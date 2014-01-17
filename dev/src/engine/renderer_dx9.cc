// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

D3DFORMAT FormatToDx9(GpuFormat format);

Renderer::Renderer() :
    is_device_lost_(false),
    phase_(Phase_Normal),
    is_wire_mode(0),
    device_dx9_(NULL),
    render_target(NULL),
    is_in_begin_scene_(false) {
}

Renderer::~Renderer() {
  Check(!device_dx9_);
  Check(!d3d9_);
}

void Renderer::InitPresentParamsDx9() {
  const D3DFORMAT kFormatDepthStencilDx9 = D3DFMT_D24S8;
  const D3DFORMAT kFormatBackBufferDx9 = D3DFMT_A8R8G8B8;
  ::ZeroMemory(&present_params_dx9_, sizeof(present_params_dx9_));
  present_params_dx9_.BackBufferWidth = init_desc.buffer_size.x;
  present_params_dx9_.BackBufferHeight = init_desc.buffer_size.y;
  if (init_desc.is_full_screen) {
    present_params_dx9_.BackBufferCount = 2;
    present_params_dx9_.FullScreen_RefreshRateInHz =
        init_desc.refresh_rate;
    if (init_desc.is_vertical_sync) {
      present_params_dx9_.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
    } else {
      present_params_dx9_.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
    }
  } else {
    // Windowed mode
    present_params_dx9_.BackBufferCount = 1;
    present_params_dx9_.Windowed = TRUE;
    present_params_dx9_.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
  }
  present_params_dx9_.hDeviceWindow = Application::GetWindowHandle();
  present_params_dx9_.BackBufferFormat = kFormatBackBufferDx9;
  present_params_dx9_.EnableAutoDepthStencil = TRUE;
  present_params_dx9_.AutoDepthStencilFormat = kFormatDepthStencilDx9;
  present_params_dx9_.SwapEffect = D3DSWAPEFFECT_DISCARD;
}

void Renderer::InitInternal() {
  Check(!d3d9_);
  d3d9_ = ::Direct3DCreate9(D3D_SDK_VERSION);
}

void Renderer::ClearInternal() {
  Check(d3d9_);
  DG_SAFE_RELEASE(d3d9_);
}

void Renderer::DestroyDeviceDx9() {
  Check(device_dx9_);
  DG_SAFE_RELEASE(device_dx9_);
}

bool Renderer::CreateDeviceDx9() {
  Check(!device_dx9_);
  D3DDEVTYPE device_type = D3DDEVTYPE_HAL;
  D3DCAPS9 caps;
  d3d9_->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);
  DWORD behavior_flags = (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) ?
      D3DCREATE_HARDWARE_VERTEXPROCESSING :
      D3DCREATE_SOFTWARE_VERTEXPROCESSING;
  int adapter_idx = init_desc.adapter;
#if !defined(DG_SHIP)
  for (unsigned int idx = 0; idx < d3d9_->GetAdapterCount(); idx++) {
    D3DADAPTER_IDENTIFIER9 adapterIdentifier;
    HRESULT hr = d3d9_->GetAdapterIdentifier(idx, 0, &adapterIdentifier);
    if (MyStrStrAnsi(adapterIdentifier.Description, "PerfHUD") != 0) {
      adapter_idx = idx;
      device_type = D3DDEVTYPE_REF;
      break;
    }
  }
#endif // #if !defined(DG_SHIP)
  if (FAILED(d3d9_->CreateDevice(
      adapter_idx,
      device_type,
      Application::GetWindowHandle(),
      behavior_flags,
      &present_params_dx9_,
      &device_dx9_))) {
    DG_LOG_LINE(TXT("renderer.createdevicedx9: size:%dx%d isfullscreen:%d"),
        present_params_dx9_.BackBufferWidth,
        present_params_dx9_.BackBufferHeight,
        !present_params_dx9_.Windowed);
    LogErrorCode(ErrorCode_CreateDeviceFailed);
    return false;
  }
  return true;
}

bool Renderer::InitDeviceInternal() {
  Check(d3d9_);
  InitPresentParamsDx9();
  if (!CreateDeviceDx9()) {
    return false;
  }
  return true;
}

bool Renderer::ClearDeviceInternal() {
  DestroyDeviceDx9();
  return true;
}

bool Renderer::OnInitDevice() {
  Check(device_dx9_);
  return true;
}

bool Renderer::OnClearDevice() {
  Check(device_dx9_);
  return true;
}

void Renderer::BeginScene() {
  device_dx9_->SetRenderState(D3DRS_FILLMODE, (is_wire_mode) ? D3DFILL_WIREFRAME : D3DFILL_SOLID);
  Check(!is_in_begin_scene_);
  if (!is_in_begin_scene_) {
    device_dx9_->BeginScene();
    is_in_begin_scene_ = true;
  }
}

void Renderer::EndScene() {
  Check(is_in_begin_scene_);
  if (is_in_begin_scene_) {
    device_dx9_->EndScene();
    is_in_begin_scene_ = false;
  }
}

bool Renderer::ResetRendererDevice() {
  UnbindUnmanagedResource();
  resources.ClearDeviceAll();
  Sleep(100);
  InitPresentParamsDx9();
  is_device_lost_ = true;
  if (device_dx9_->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) {
    if (device_dx9_->Reset(&present_params_dx9_) != D3D_OK) {
      DG_LOG_LINE(TXT("error: renderer.resetrendererdevice.reset.failed:"));
      return false;
    }
    DG_LOG_LINE(TXT("renderer.resetrendererdevice.devicereset:"));
  } else {
    DestroyDeviceDx9();
    while (!CreateDeviceDx9()) {
      Sleep(100);
    }
    DG_LOG_LINE(TXT("renderer.resetrendererdevice.recreated:"));
  }
  resources.InitDeviceAll();
  BindUnmanagedResource();
  is_device_lost_ = false;
  return true;
}

} // namespace dg
