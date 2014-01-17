// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

VertexShader::VertexShader() {
  type_ = ShaderType_VertexShader;
  profile_ = ShaderProfile_VS_30;
}

VertexShader::~VertexShader() {
}

bool VertexShader::OnInitDevice() {
  return RC_CreateVertexShader(shader_name_.Get(), blob_.data(), blob_.size(), device_vertex_shader_handle_);
}

bool VertexShader::OnClearDevice() {
  device_vertex_shader_handle_.Release();
  return true;
}

void VertexShader::SetCurrent() {
  BindResource();
  RC_SetVertexShader(shader_name_.Get(), device_vertex_shader_handle_);
}

} // namespace dg
