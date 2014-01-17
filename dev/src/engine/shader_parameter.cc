// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

ShaderParameter::ShaderParameter() :
    shader_type_(ShaderType_Unknown),
    value_type_(kUnknownType),
    register_count_(1),
    register_index_(-1),
    is_disabled_(false) {
}

ShaderParameter::~ShaderParameter() {
  if (value_type_ == kTextureType) {
    variant_.texture_value = 0;
  }
}

void ShaderParameter::ReadFrom(Stream* stream) {
  StreamReadClass(stream, name_);
  StreamReadFlat(stream, register_index_);
  StreamReadFlat(stream, shader_type_);
  StreamReadFlat(stream, value_type_);
  StreamReadFlat(stream, register_count_);
  switch (value_type_) {
  case ShaderParameter::kVector4Type:
  case ShaderParameter::kMatrixType:
    StreamReadAuto(stream, variant_.float_array_value);
    Check(!variant_.float_array_value.IsEmpty());
    break;
  case ShaderParameter::kTextureType:
    StreamReadPointer(stream, variant_.texture_value);
    break;
  default:
    DG_UNIMPLEMENTED();
  }
}

void ShaderParameter::WriteTo(Stream* out_stream) {
  StreamWriteClass(out_stream, name_);
  StreamWriteFlat(out_stream, register_index_);
  Check(value_type_ != kUnknownType);
  StreamWriteFlat(out_stream, shader_type_);
  StreamWriteFlat(out_stream, value_type_);
  StreamWriteFlat(out_stream, register_count_);
  switch (value_type_) {
  case ShaderParameter::kVector4Type:
  case ShaderParameter::kMatrixType:
    Check(!variant_.float_array_value.IsEmpty());
    StreamWriteAuto(out_stream, variant_.float_array_value);
    break;
  case ShaderParameter::kTextureType:
    StreamWritePointer(out_stream, variant_.texture_value);
    break;
  default:
    DG_UNIMPLEMENTED();
  }
}

void ShaderParameter::SetDefaultValueByType(ValueType type) {
  switch (type) {
  case ShaderParameter::kVector4Type:
    SetVector4(&Vector4::kZero);
    break;
  case ShaderParameter::kMatrixType:
    SetMatrix(&Matrix::kIdentity);
    break;
  case ShaderParameter::kTextureType:
    SetTexture(NULL);
    break;
  }
}

void ShaderParameter::SetVector4(const Vector4* value) {
  Check((value_type_ == kUnknownType) || (value_type_ == ShaderParameter::kVector4Type));
  Check(register_count_ == 1);
  value_type_ = kVector4Type;
  variant_.float_array_value.Resize(4);
  MyMemCpy(variant_.float_array_value.ptr(), value->GetFloatsConst(), sizeof(*value));
}

void ShaderParameter::SetVector4Array(const float* values, int num_vector4) {
  Check((value_type_ == kUnknownType) || (value_type_ == ShaderParameter::kVector4Type));
  value_type_ = kVector4Type;
  register_count_ = num_vector4;
  variant_.float_array_value.Resize(4*num_vector4);
  if (values == NULL) {
    MyMemSet(variant_.float_array_value.ptr(), 0, sizeof(float)*4*num_vector4);
  } else {
    MyMemCpy(variant_.float_array_value.ptr(), values, sizeof(float)*4*num_vector4);
  }
}

void ShaderParameter::SetMatrix(const Matrix* matrix) {
  Check((value_type_ == kUnknownType) || (value_type_ == ShaderParameter::kMatrixType));
  value_type_ = kMatrixType;
  variant_.float_array_value.Resize(16);
  MyMemCpy(variant_.float_array_value.ptr(), matrix->GetFloatsConst(), sizeof(*matrix));
}

void ShaderParameter::SetTexture(Texture* texture) {
  Check((value_type_ == kUnknownType) || (value_type_ == ShaderParameter::kTextureType));
  value_type_ = kTextureType;
  variant_.texture_value = texture;
}

Vector4 ShaderParameter::GetVector4() const {
  Check(value_type_ == ShaderParameter::kVector4Type);
  return Vector4(variant_.float_array_value.GetConstPtr());
}

Matrix ShaderParameter::GetMatrix() const {
  Check(value_type_ == ShaderParameter::kMatrixType);
  return Matrix(variant_.float_array_value.GetConstPtr());
}

const Texture& ShaderParameter::GetTexture() const {
  Check(value_type_ == ShaderParameter::kTextureType);
  return *(variant_.texture_value);
}

void ShaderParameter::SetShaderConstantVector4(
    const Cstr* name, ShaderType in_shader_type, int in_register_index, const float* data, int in_register_count) {
  switch (in_shader_type) {
  case ShaderType_VertexShader:
    RC_SetVertexShaderConstantFloat(name, in_register_index, data, in_register_count);
    break;
  case ShaderType_PixelShader:
    RC_SetPixelShaderConstantFloat(name, in_register_index, data, in_register_count);
    break;
  case ShaderType_Unknown:
    RC_SetVertexShaderConstantFloat(name, in_register_index, data, in_register_count);
    RC_SetPixelShaderConstantFloat(name, in_register_index, data, in_register_count);
    break;
  default:
    DG_UNREACHABLE();
  }
}

// Submit the shader parameter value into the GPU
void ShaderParameter::SubmitParameter() {
  if (is_disabled_) return;
  if (register_index_ < 0) {
    // This is only for GL
    RC_GetShaderConstantStartRegister(name_.Get(), &register_index_);
    if (register_index_ < 0) {
      is_disabled_ = true;
      return;
    }
  }
  switch (value_type_) {
  case ShaderParameter::kVector4Type:
    SetShaderConstantVector4(
        name_.Get(), shader_type_, register_index_,
        variant_.float_array_value.GetConstPtr(),
        register_count_);
    break;
  case ShaderParameter::kMatrixType:
    SetShaderConstantMatrix(
        name_.Get(), shader_type_,
        register_index_,
        reinterpret_cast<const Matrix*>(
            variant_.float_array_value.GetConstPtr()));
    break;
  case ShaderParameter::kTextureType:
    if (variant_.texture_value) {
      Check(variant_.texture_value->resource_state().IsBound());
      //DG_LOG_LINE(TXT("tex#%d = %s as %s"), register_index_, variant_.texture_value->resource_name(), name_.Get());
      RC_SetTexture(name_.Get(), register_index_, variant_.texture_value->device_texture_handle());
    } else {
      TextureHandle null_ptr;
      RC_SetTexture(name_.Get(), register_index_, null_ptr);
    }
    break;
  }
}

ShaderParameter& ShaderParameter::CopyFrom(const ShaderParameter& rhs) {
  if (this == &rhs) {
    return *this;
  }
  set_name(rhs.name().GetConst());
  shader_type_ = rhs.shader_type_;
  value_type_ = rhs.value_type_;
  register_index_ = rhs.register_index_;
  register_count_ = rhs.register_count_;
  // Texture type is the special case
  // Just copying the data structure is not enough for the texture type
  if (rhs.value_type_ == kTextureType) {
    // Pointer reference counting is done here
    variant_.texture_value = rhs.variant_.texture_value;
  } else {
    variant_ = rhs.variant_;
  }
  return *this;
}

void ShaderParameter::CopyValueFrom(const ShaderParameter& rhs) {
  Check(rhs.value_type_ == this->value_type_);
  variant_ = rhs.variant_;
}

void ShaderParameter::SetShaderConstantMatrix(
    const Cstr* name, ShaderType in_shader_type,
    int in_register_index, const Matrix* data) {
  switch (in_shader_type) {
  case ShaderType_VertexShader:
    RC_SetVertexShaderConstantMatrix(name, in_register_index, data);
    break;
  case ShaderType_PixelShader:
    RC_SetPixelShaderConstantMatrix(name, in_register_index, data);
    break;
  case ShaderType_Unknown:
    // If not sure vertex or pixel, then just set in vertex
    RC_SetVertexShaderConstantMatrix(name, in_register_index, data);
    break;
  default:
    DG_UNREACHABLE();
  }
}

void ShaderParameter::BindResource() {
  if (value_type_ == kTextureType) {
    if (variant_.texture_value) {
      variant_.texture_value->BindResource();
    }
  }
}

} // namespace dg
