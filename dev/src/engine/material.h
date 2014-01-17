// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {
class ShaderEffect;

class Material : public Resource {
public:
  Ptr<ShaderEffect> effect_;

  Material();
  virtual ~Material();
  // Create a shader parameter, and add it to the material
  ShaderParameter* AddParameter(const Cstr* name, ShaderParameter::ValueType value_type);
  ShaderParameter* GetParameter(int index);
  ShaderParameter* GetParameterByName(const Cstr* name);
  // Clear all shader parameters
  void ClearParameters();
  // Set the number of shader parameters in the material
  void SetNumParameters(int num_parameters);
  int GetNumParameters() const { return parameters_.Count(); }
  // Copy the material shader parameters into the shader effect parameters
  // It clears all the previous effect parameters first
  void CopyParametersTo(ShaderEffect* effect);
  virtual void ReadFrom(Stream* stream);
  virtual void WriteTo(Stream* out_stream);
  virtual void BindResource();

private:
  Array<ShaderParameter> parameters_;

  DG_DECLARE_RESOURCE(Material, Resource);
};

namespace material_util {
  void InitSolidColorMaterial(Material* mat, const Vector4* diffuse_color = NULL, const Vector4* ambient_color = NULL, const Vector4* specular_intensity_power = NULL);
  void SetMaterialAmbient(Material* mat, const Vector4& ambient);
  void SetMaterialDiffuse(Material* mat, const Vector4& diffuse);
  // Specular_intensity_power - xyz: intensity, w: power
  void SetMaterialSpecular(Material* mat, const Vector4& specular_intensity_power);
  void SetMaterialAmbient(Material* mat, float r, float g, float b, float a = 1.0f);
  void SetMaterialDiffuse(Material* mat, float r, float g, float b, float a = 1.0f);
  void SetMaterialSpecular(Material* mat, float intensity_r, float intensity_g, float intensity_b, float power);
}

} // namespace dg
