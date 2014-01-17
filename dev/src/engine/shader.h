// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

enum ShaderType {
  ShaderType_Unknown = 0,
  ShaderType_VertexShader = 1,
  ShaderType_PixelShader = 2,
};

enum ShaderProfile {
  ShaderProfile_Unknown = 0,
  ShaderProfile_VS_11,
  ShaderProfile_PS_20,
  ShaderProfile_VS_30,
  ShaderProfile_PS_30,
};

class ShaderParameter;

class Shader : public RenderResource {
public:
  Shader();
  virtual ~Shader();
  virtual void ReadFrom(Stream* stream);
  virtual void WriteTo(Stream* out_stream);
  virtual void SetCurrent() DG_PURE;
  ShaderParameter* GetParameter(int idx);
  ShaderParameter* FindParameterByName(const Cstr* name);

  String shader_name_;
  AutoDeleteBlob blob_;
  ShaderProfile profile_;
  ShaderType type_;
  Array<ShaderParameter> parameters_;
};

} // namespace dg
