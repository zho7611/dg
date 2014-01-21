// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

ShaderProgram::~ShaderProgram() {
}

void ShaderProgram::SubmitParameters() {
  SetCurrent();
  // Bind resources first
  for (int idx = 0; idx < parameters_.Count(); ++idx) {
    ShaderParameter& param = parameters_.Get(idx);
    param.BindResource();
  }
  for (int idx = 0; idx < parameters_.Count(); ++idx) {
    ShaderParameter& param = parameters_.Get(idx);
    param.SubmitParameter();
  }
}

void ShaderProgram::SetCurrent() {
  Check(vertex_shader_);
  Check(pixel_shader_);
  vertex_shader_->SetCurrent();
  pixel_shader_->SetCurrent();
  // TODO: creating device resource should be somewhere else
  if (!shader_program_handle_) {
    BindResource();
  }
  String debug_name = vertex_shader_->shader_name_;
  debug_name.Format(TXT("%s-%s"), vertex_shader_->shader_name_.Get(), pixel_shader_->shader_name_.Get());
  RC_SetShaderProgram(debug_name.Get(), shader_program_handle_,
      vertex_shader_->device_vertex_shader_handle_,
      pixel_shader_->device_pixel_shader_handle_);
}

void ShaderProgram::SetShaders(VertexShader* vertex_shader, PixelShader* pixel_shader) {
  if (shader_program_handle_) {
    shader_program_handle_.Release();
  }
  vertex_shader_ = vertex_shader;
  pixel_shader_ = pixel_shader;
  parameters_.Clear();
  for (int idx = 0; idx < vertex_shader_->parameters_.Count(); ++idx) {
    parameters_.PushBack(vertex_shader_->parameters_.Get(idx));
  }
  for (int idx = 0; idx < pixel_shader_->parameters_.Count(); ++idx) {
    parameters_.PushBack(pixel_shader_->parameters_.Get(idx));
  }
}

bool ShaderProgram::OnInitDevice() {
  if (shader_program_handle_) {
    return false;
  }
  RC_CreateShaderProgram(shader_program_handle_,
      vertex_shader_->device_vertex_shader_handle_,
      pixel_shader_->device_pixel_shader_handle_);
  return shader_program_handle_;
}

bool ShaderProgram::OnClearDevice() {
  if (!shader_program_handle_) {
    return false;
  }
  shader_program_handle_.Release();
  return true;
}

void ShaderProgram::FindShaderParametersByName(const Cstr* name, List<ShaderParameter*>* out_parameters) {
  for (int idx = 0; idx < parameters_.Count(); ++idx) {
    ShaderParameter* param = &parameters_.Get(idx);
    if (param->name().IsEqual(name)) {
      out_parameters->PushBack(param);
    }
  }
}

void ShaderProgram::ReadFrom(Stream* stream) {
  StreamReadPointer(stream, vertex_shader_);
  StreamReadPointer(stream, pixel_shader_);
  SetShaders(vertex_shader_.ptr(), pixel_shader_.ptr());
  OnReadDone();
}

void ShaderProgram::WriteTo(Stream* out_stream) {
  StreamWritePointer(out_stream, vertex_shader_);
  StreamWritePointer(out_stream, pixel_shader_);
  OnWriteDone();
}

ShaderPass::~ShaderPass() {
  shader_program_.UnbindResource();
}

void ShaderPass::ReadFrom(Stream* stream) {
  StreamReadAuto(stream, name_);
  StreamReadAuto(stream, render_states_);
  StreamReadAuto(stream, sampler_states_);
  StreamReadAuto(stream, shader_program_);
}

void ShaderPass::WriteTo(Stream* out_stream) {
  StreamWriteAuto(out_stream, name_);
  StreamWriteAuto(out_stream, render_states_);
  StreamWriteAuto(out_stream, sampler_states_);
  StreamWriteAuto(out_stream, shader_program_);
}

void ShaderPass::BeginPass() {
  render_states_.Apply();
  sampler_states_.Apply();
  shader_program_.SetCurrent();
}

void ShaderPass::EndPass() {
}

void ShaderTechnique::ReadFrom(Stream* stream) {
  StreamReadAuto(stream, name_);
  StreamReadAuto(stream, passes_);
}

void ShaderTechnique::WriteTo(Stream* out_stream) {
  StreamWriteAuto(out_stream, name_);
  StreamWriteAuto(out_stream, passes_);
}

ShaderEffect::ShaderEffect() :
  active_technique_index_(-1),
  active_pass_index_(-1) {
}

ShaderEffect::~ShaderEffect() {
}

void ShaderEffect::ReadFrom(Stream* stream) {
  StreamReadAuto(stream, effect_name_);
  StreamReadAuto(stream, techniques_);
  OnReadDone();
}

void ShaderEffect::WriteTo(Stream* out_stream) {
  StreamWriteAuto(out_stream, effect_name_);
  StreamWriteAuto(out_stream, techniques_);
  OnWriteDone();
}

void ShaderEffect::SetShadowsParams() {
  Check(kNumShadowCascades == 4);
  SetVector4ArrayByName(TXT("GlobalCascadeDepths"), g_shadow_manager.depths_, ((kNumShadowCascades-1+3)/4));
  float cascade_scales[kNumShadowCascades*4];
  for (int idx = 0; idx < kNumShadowCascades; idx++) {
    cascade_scales[idx*4] = g_shadow_manager.scales_[idx].x;
    cascade_scales[idx*4+1] = g_shadow_manager.scales_[idx].y;
    cascade_scales[idx*4+2] = g_shadow_manager.scales_[idx].z;
    cascade_scales[idx*4+3] = 0;
  }
  SetVector4ArrayByName(TXT("GlobalCascadeScales"), cascade_scales, kNumShadowCascades);
  float cascade_offsets[kNumShadowCascades*4];
  for (int idx = 0; idx < kNumShadowCascades; idx++) {
    cascade_offsets[idx*4] = g_shadow_manager.offsets_[idx].x;
    cascade_offsets[idx*4+1] = g_shadow_manager.offsets_[idx].y;
    cascade_offsets[idx*4+2] = g_shadow_manager.offsets_[idx].z;
    cascade_offsets[idx*4+3] = 0;
  }
  SetVector4ArrayByName(TXT("GlobalCascadeOffsets"), cascade_offsets, kNumShadowCascades);
}

void ShaderEffect::SetViewParams() {
  View* view = g_renderer.view_state.view;
  if (!view) {
    // Set default matrices
    SetMatrixByName(TXT("GlobalViewProjectionMatrix"), Matrix::kIdentity);
    return;
  }
  Camera* camera = view->camera();
  if (!camera) {
    // Set default matrices
    SetMatrixByName(TXT("GlobalViewProjectionMatrix"), Matrix::kIdentity);
    return;
  }
  // If it is the light view,
  if (camera->is_light_camera) {
    SetMatrixByName(TXT("GlobalViewProjectionMatrix"),
        *camera->GetLightViewProjMatrix(camera->cascade_index));
  } else {// It is the normal view with light_camera
    const Matrix* view_proj = camera->view_proj_matrix();
    SetMatrixByName(TXT("GlobalViewProjectionMatrix"), *view_proj);
    Vector4 camera_origin_4(camera->GetOrigin(), 1);
    SetVector4ByName(TXT("GlobalCameraPosition"), camera_origin_4);
    // Set the light camera projection matrix
    if (view->light_camera()) {
      SetMatrixByName(TXT("GlobalLightViewProjectionMatrix"),
          *view->light_camera()->GetLightViewProjMatrix(0));
    }
  }
}

void ShaderEffect::SetLight(Light* light) {
  Check(light);
  Vector4 light_ambient;
  Vector4 light_diffuse;
  Vector4 light_direction;
  {
    light_ambient.Set(light->ambient_color_);
    light_diffuse.Set(light->diffuse_color_);
    light_direction.Set(light->direction_.Normalize(), 1.0f);
  }
  SetVector4ByName(TXT("GlobalLightAmbient"), light_ambient);
  SetVector4ByName(TXT("GlobalLightDiffuse"), light_diffuse);
  SetVector4ByName(TXT("GlobalLightDirection"), light_direction);
  Vector4 global_light_brightness_vec4(g_engine_config->global_light_brightness_);
  SetVector4ArrayByName(TXT("GlobalLightBrightness"), global_light_brightness_vec4.GetFloatsConst(), 1);
}

void ShaderEffect::SetupEffect() {
}

void ShaderEffect::GetRenderTargetSize(Size2& outSize) {
  outSize = g_renderer.GetScreenSize();
}

void ShaderEffect::SubmitMesh(Mesh* mesh, Material* material) {
  // For now, the renderer phase index determines which shader technique will be used
  if (g_renderer.phase() != Renderer::Phase_None) {
    SetTechnique((int)g_renderer.phase());
  }
  // Set up for this shader effect
  SetupEffect();
  // Copy the material shader parameters into the shader effect
  if (material) {
    material->CopyParametersTo(this);
  }
  const int kNumPasses = BeginEffect();
  for (int idx = 0; idx < kNumPasses; ++idx) {
    BeginPass(idx);
    CommitChanges();
    if (mesh) mesh->SubmitMesh();
    EndPass();
  }
  EndEffect();
}

int ShaderEffect::BeginEffect() {
  if (!techniques_.IsEmpty()) {
    return techniques_.Get(active_technique_index_).passes_.Count();
  }
  return 0;
}

void ShaderEffect::EndEffect() {
}

void ShaderEffect::BeginPass(int pass) {
  active_pass_index_ = pass;
  if (!techniques_.IsEmpty()) {
    techniques_.Get(active_technique_index_).passes_.Get(active_pass_index_).BeginPass();
  }
}

void ShaderEffect::EndPass() {
  if (!techniques_.IsEmpty()) {
    techniques_.Get(active_technique_index_).passes_.Get(active_pass_index_).EndPass();
  }
  active_pass_index_ = -1;
}

bool ShaderEffect::FindParametersByName(const Cstr* name, ParameterList* out_parameters) {
  Check(out_parameters);
  bool found = false;
  if (FindShaderParametersByName(name, out_parameters)) {
    return true;
  }
  return found;
}

bool ShaderEffect::FindShaderParametersByName(const Cstr* name, ParameterList* out_parameters) {
  Check(out_parameters);
  for (int tech_idx = 0; tech_idx < techniques_.Count(); ++tech_idx) {
    ShaderTechnique& tech = techniques_.Get(tech_idx);
    for (int pass_idx = 0; pass_idx < tech.passes_.Count(); ++pass_idx) {
      ShaderPass& pass = tech.passes_.Get(pass_idx);
      pass.shader_program_.FindShaderParametersByName(name, out_parameters);
    }
  }
  return (!out_parameters->IsEmpty());
}

// Set effect level shader parameters over vertex/pixel shader constant
void ShaderEffect::CommitChanges() {
  if ((active_technique_index_ >= 0) && (active_pass_index_ >= 0) && (techniques_.Count() > 0)) {
    ShaderPass& pass = techniques_.Get(active_technique_index_).passes_.Get(active_pass_index_);
    pass.shader_program_.SubmitParameters();
  }
}

void ShaderEffect::SetTechniqueByName(const Cstr* name) {
  for (int idx = 0; idx < techniques_.Count(); ++idx) {
    ShaderTechnique* tech = &techniques_.Get(idx);
    if (tech->name_.IsEqual(name)) {
      SetTechnique(idx);
      return;
    }
  }
}

void ShaderEffect::SetTechnique(int index) {
  active_technique_index_ = index;
}

void ShaderEffect::SetVector4ByName(const Cstr* name, Vector4& vector) {
  ParameterList params;
  if (FindParametersByName(name, &params)) {
    ParameterList::Iterator it = params.Begin();
    ParameterList::Iterator ie = params.End();
    for (; it != ie; ++it) {
      (*it)->SetVector4(&vector);
    }
  } else {
    DG_LOG_LINE_ONE_TIME(TXT("shadereffect.setvector4byname: not-found-name:%s"), name);
  }
}

void ShaderEffect::SetVector4ArrayByName(const Cstr* name, const float* values, int num_vector4) {
  ParameterList params;
  if (FindParametersByName(name, &params)) {
    ParameterList::Iterator it = params.Begin();
    ParameterList::Iterator ie = params.End();
    for (; it != ie; ++it) {
      (*it)->SetVector4Array(values, num_vector4);
    }
  } else {
    DG_LOG_LINE_ONE_TIME(TXT("shadereffect.setvector4arraybyname: not-found-name:%s"), name);
  }
}

void ShaderEffect::SetMatrixByName(const Cstr* name, const Matrix& matrix) {
  ParameterList params;
  if (FindParametersByName(name, &params)) {
    ParameterList::Iterator it = params.Begin();
    ParameterList::Iterator ie = params.End();
    for (; it != ie; ++it) {
      (*it)->SetMatrix(&matrix);
    }
  } else {
    DG_LOG_LINE_ONE_TIME(TXT("shadereffect.setmatrixbyname: not-found-name:%s"), name);
  }
}

void ShaderEffect::SetTextureByName(const Cstr* name, Texture* texture) {
  ParameterList params;
  if (FindParametersByName(name, &params)) {
    ParameterList::Iterator it = params.Begin();
    ParameterList::Iterator ie = params.End();
    for (; it != ie; ++it) {
      (*it)->SetTexture(texture);
    }
  } else {
    DG_LOG_LINE_ONE_TIME(TXT("shadereffect.settexturebyname: not-found-name:%s"), name);
  }
}

// Copies the shader parameters from the material into the shader effect
// Every shader parameters that match the name of the material parameter will be affected
void ShaderEffect::CopyParameterFrom(ShaderParameter* material_parameter) {
  ParameterList params;
  if (FindParametersByName(material_parameter->name().GetConst(), &params)) {
    ParameterList::Iterator it = params.Begin();
    ParameterList::Iterator ie = params.End();
    for (; it != ie; ++it) {
      (*it)->CopyValueFrom(*material_parameter);
    }
  }
}

} // namespace dg
