// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class VertexShader : public Shader {
public:
  VertexShader();
  virtual ~VertexShader();
  virtual bool OnInitDevice();
  virtual bool OnClearDevice();
  virtual void SetCurrent();

  VertexShaderHandle device_vertex_shader_handle_;

  DG_DECLARE_RESOURCE(VertexShader, Shader);
};

} // namespace dg
