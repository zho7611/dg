// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

typedef List<ShaderParameter*> ParameterList;

// A pair of vertex shader and pixel shader
// This is for OpenGL which requires the shader program resource
class ShaderProgram : public RenderResource {
public:
  ~ShaderProgram();
  void SetShaders(VertexShader* vertex_shader, PixelShader* pixel_shader);
  VertexShader* vertex_shader() { return vertex_shader_.ptr(); }
  PixelShader* pixel_shader() { return pixel_shader_.ptr(); }
  // Bind the current shader program resource
  void SetCurrent();
  void ReadFrom(Stream* stream);
  void WriteTo(Stream* out_stream);
  void SubmitParameters();
  bool OnInitDevice();
  bool OnClearDevice();
  void FindShaderParametersByName(const Cstr* name, ParameterList* out_parameters);
  Array<ShaderParameter>& parameters() { return parameters_; }

private:
  Ptr<VertexShader> vertex_shader_;
  Ptr<PixelShader> pixel_shader_;
  ShaderProgramHandle shader_program_handle_;
  Array<ShaderParameter> parameters_;
};

// A set of render states, sampler states and shader program
class ShaderPass {
public:
  ~ShaderPass();
  void ReadFrom(Stream* stream);
  void WriteTo(Stream* out_stream);
  void BeginPass();
  void EndPass();

  String name_;
  RenderStateSet render_states_;
  SamplerStateSet sampler_states_;
  ShaderProgram shader_program_;
};

// A list of shader passes
class ShaderTechnique {
public:
  void ReadFrom(Stream* stream);
  void WriteTo(Stream* out_stream);

  String name_;
  Array<ShaderPass> passes_;
};

// A collection of shader techniques
class ShaderEffect : public Resource {
public:
  String effect_name_;
  Array<ShaderTechnique> techniques_;

  ShaderEffect();
  virtual ~ShaderEffect();
  void SubmitMesh(Mesh* mesh, Material* material);
  int BeginEffect();
  void EndEffect();
  void BeginPass(int pass);
  void EndPass();
  void CommitChanges();
  void CopyParameterFrom(ShaderParameter* material_parameter);
  void SetIntegerByName(const Cstr* name, int value);
  void SetFloatByName(const Cstr* name, float value);
  void SetFloatArrayByName(const Cstr* name, const float* value, int num_floats);
  void SetVector2ByName(const Cstr* name, Vector2& vector);
  void SetVector3ByName(const Cstr* name, Vector3& vector);
  void SetVector4ByName(const Cstr* name, Vector4& vector);
  void SetVector2ArrayByName(const Cstr* name, const float* values, int num_vector2);
  void SetVector3ArrayByName(const Cstr* name, const float* values, int num_vector3);
  void SetVector4ArrayByName(const Cstr* name, const float* values, int num_vector4);
  void SetMatrixByName(const Cstr* name, const Matrix& matrix);
  void SetTextureByName(const Cstr* name, Texture* texture);
  void SetTechniqueByName(const Cstr* name);
  void SetTechnique(int index);
  void SetViewParams();
  void SetLight(Light* light);
  void SetShadowsParams();
  virtual void ReadFrom(Stream* stream);
  virtual void WriteTo(Stream* out_stream);

protected:
  virtual void SetupEffect();
  void GetRenderTargetSize(Size2& outSize);
  // Find the matching parameters in the shader effect.
  // The found parameters could be from vertex shader, pixel shader, or the shader effect itself.
  // Returns true if parameters are found.
  bool FindParametersByName(const Cstr* name, ParameterList* out_parameters);
  // Find matching parameters in the vertex/pixel shaders.
  // The shader parameters from shader effect are not included.
  // Returns true if parameters are found.
  bool FindShaderParametersByName(const Cstr* name, ParameterList* out_parameters);

private:
  // -1 is the invalid index value
  int active_technique_index_;
  // -1 is the invalid index value
  int active_pass_index_;

  DG_DECLARE_RESOURCE(ShaderEffect, Resource);
};

} // namespace dg
