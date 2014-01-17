// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

Shader::Shader() :
    profile_(ShaderProfile_Unknown),
    type_(ShaderType_Unknown) {
}

Shader::~Shader() {
}

ShaderParameter* Shader::GetParameter(int idx) {
  if (idx >= parameters_.Count()) {
    return NULL;
  }
  return &(parameters_.Get(idx));
}

ShaderParameter* Shader::FindParameterByName(const Cstr* name) {
  for (int idx = 0; idx < parameters_.Count(); ++idx) {
    ShaderParameter* param = &parameters_.Get(idx);
    if (param->name().IsEqual(name)) {
      return param;
    }
  }
  return NULL;
}

void Shader::ReadFrom(Stream* stream) {
  StreamReadAuto(stream, shader_name_);
  StreamReadFlat(stream, profile_);
  StreamReadAuto(stream, blob_);
  StreamReadAuto(stream, parameters_);
  OnReadDone();
}

void Shader::WriteTo(Stream* out_stream) {
  StreamWriteAuto(out_stream, shader_name_);
  StreamWriteFlat(out_stream, profile_);
  StreamWriteAuto(out_stream, blob_);
  StreamWriteAuto(out_stream, parameters_);
  OnWriteDone();
}

} // namespace dg
