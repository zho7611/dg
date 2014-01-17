// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

Material::Material() :
  effect_(0) {}

Material::~Material() {
  ClearParameters();
}

void Material::ReadFrom(Stream* stream) {
  StreamReadPointer(stream, effect_);
  StreamReadClass(stream, parameters_);
  OnReadDone();
}

void Material::WriteTo(Stream* out_stream) {
  StreamWritePointer(out_stream, effect_);
  StreamWriteClass(out_stream, parameters_);
  OnWriteDone();
}

void Material::CopyParametersTo(ShaderEffect* effect) {
  Check(effect_);
  if (!effect) return;
  const int num_params = parameters_.Count();
  for (int idx = 0; idx < num_params; ++idx) {
    effect->CopyParameterFrom(&parameters_.Get(idx));
  }
}

void Material::ClearParameters() {
  parameters_.Clear();
}

// This method resizes its shader parameters and content as well
// You shouldn't use pre-allocated parameters
// Call ClearParameters() for this case
void Material::SetNumParameters(int num_parameters) {
  Check(parameters_.Count() == 0);
  parameters_.Resize(num_parameters);
}

ShaderParameter* Material::GetParameter(int index) {
  return &parameters_.Get(index);
}

ShaderParameter* Material::GetParameterByName(const Cstr* name) {
  const int num_params = parameters_.Count();
  for (int idx = 0; idx < num_params; ++idx) {
    ShaderParameter* param = &parameters_.Get(idx);
    if (param->name().IsEqual(name)) {
      return param;
    }
  }
  return NULL;
}

ShaderParameter* Material::AddParameter(
    const Cstr* name,
    ShaderParameter::ValueType value_type) {
  ShaderParameter param;
  param.set_name(name);
  param.set_value_type(value_type);
  parameters_.PushBack(param);
  return &parameters_.Last();
}

void Material::BindResource() {
  effect_->BindResource();
  const int num_params = parameters_.Count();
  for (int idx = 0; idx < num_params; ++idx) {
    parameters_.Get(idx).BindResource();
  }
}

void material_util::SetMaterialAmbient(Material* mat, float r, float g, float b, float a) {
  Vector4 rgba(r, g, b, a);
  SetMaterialAmbient(mat, rgba);
}

void material_util::SetMaterialDiffuse(Material* mat, float r, float g, float b, float a) {
  Vector4 rgba(r, g, b, a);
  SetMaterialDiffuse(mat, rgba);
}

void material_util::SetMaterialSpecular(Material* mat, float intensity_r, float intensity_g, float intensity_b, float power) {
  Vector4 rgba(intensity_r, intensity_g, intensity_b, power);
  SetMaterialSpecular(mat, rgba);
}

void material_util::SetMaterialAmbient(Material* mat, const Vector4& ambient_color) {
  ShaderParameter* param = mat->GetParameterByName(TXT("GlobalMaterialAmbient"));
  if (!param) {
    param = mat->AddParameter(TXT("GlobalMaterialAmbient"), ShaderParameter::kVector4Type);
  } else {
    Check(param->value_type() == ShaderParameter::kVector4Type);
  }
  param->SetVector4(&ambient_color);
}

void material_util::SetMaterialDiffuse(Material* mat, const Vector4& diffuse_color) {
  ShaderParameter* param = mat->GetParameterByName(TXT("GlobalMaterialDiffuse"));
  if (!param) {
    param = mat->AddParameter(TXT("GlobalMaterialDiffuse"), ShaderParameter::kVector4Type);
  } else {
    Check(param->value_type() == ShaderParameter::kVector4Type);
  }
  param->SetVector4(&diffuse_color);
}

void material_util::SetMaterialSpecular(Material* mat, const Vector4& specular) {
  ShaderParameter* param = mat->GetParameterByName(TXT("GlobalMaterialSpecular"));
  if (!param) {
    param = mat->AddParameter(TXT("GlobalMaterialSpecular"), ShaderParameter::kVector4Type);
  } else {
    Check(param->value_type() == ShaderParameter::kVector4Type);
  }
  param->SetVector4(&specular);
}

void material_util::InitSolidColorMaterial(Material* mat, const Vector4* diffuse_color, const Vector4* ambient_color, const Vector4* specular_intensity_power) {
  if (!mat) return;
  ShaderParameter* param = NULL;
  param = mat->AddParameter(TXT("AmbientOcclusionTextureSampler"), ShaderParameter::kTextureType);
  param->SetTexture(g_renderer.default_textures[Renderer::DefaultTextures_Black]);
  param = mat->AddParameter(TXT("NormalTextureSampler"), ShaderParameter::kTextureType);
  param->SetTexture(g_renderer.default_textures[Renderer::DefaultTextures_NormalPurple]);
  param = mat->AddParameter(TXT("SpecularTextureSampler"), ShaderParameter::kTextureType);
  param->SetTexture(g_renderer.default_textures[Renderer::DefaultTextures_White]);
  param = mat->AddParameter(TXT("DiffuseTextureSampler"), ShaderParameter::kTextureType);
  param->SetTexture(g_renderer.default_textures[Renderer::DefaultTextures_White]);
  if (diffuse_color) material_util::SetMaterialDiffuse(mat, *diffuse_color);
  else material_util::SetMaterialDiffuse(mat, 1.f, 1.f, 1.f, 1.f);
  if (ambient_color) material_util::SetMaterialAmbient(mat, *ambient_color);
  else material_util::SetMaterialAmbient(mat, 0, 0, 0, 0);
  if (specular_intensity_power) material_util::SetMaterialSpecular(mat, *specular_intensity_power);
  else material_util::SetMaterialSpecular(mat, 0, 0, 0, 0);
  mat->effect_ = LoadResource<SimpleShaderEffect>(g_engine_config->default_effect_path_.Get());
  mat->OnReadDone();
}

} // namespace dg
