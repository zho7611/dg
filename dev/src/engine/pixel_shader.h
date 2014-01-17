// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class PixelShader : public Shader {
public:
  PixelShaderHandle device_pixel_shader_handle_;

  PixelShader();
  virtual ~PixelShader();
  virtual bool OnInitDevice();
  virtual bool OnClearDevice();
  virtual void SetCurrent();

  DG_DECLARE_RESOURCE(PixelShader, Shader);
};

} // namespace dg
