// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

PixelShader::PixelShader() {
  type_ = ShaderType_PixelShader;
  profile_ = ShaderProfile_PS_30;
}

PixelShader::~PixelShader() {
}

bool PixelShader::OnInitDevice() {
  return RC_CreatePixelShader(shader_name_.Get(), blob_.data(), blob_.size(), device_pixel_shader_handle_);
}

bool PixelShader::OnClearDevice() {
  device_pixel_shader_handle_.Release();
  return true;
}

void PixelShader::SetCurrent() {
  BindResource();
  RC_SetPixelShader(shader_name_.Get(), device_pixel_shader_handle_);
}

} // namespace dg
