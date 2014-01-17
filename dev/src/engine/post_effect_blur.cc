// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

PostEffectBlur::PostEffectBlur() : is_last_frame_updated_(false) {}

PostEffectBlur::~PostEffectBlur() {
  Clear();
  Check(!mesh_quad_);
}

void PostEffectBlur::Init(const Size2& size) {
  PostEffect::Init(size);
  OnReadDone();
  const Size2 kDownSampleSize = size_/GetInvScale();
  // Initialize render targets
  {
    const bool kIsNeedFloatPrecision = false;
    GpuFormat format = GpuFormat_A8R8G8B8;
    if (kIsNeedFloatPrecision) {
      format = GpuFormat_A16B16G16R16F;
    }
    GpuFormat depth_format = GpuFormat_D24S8;
    original_.Init(size_, format, depth_format);
    original_.BindResource();
    down_sample_.Init(kDownSampleSize, format, depth_format);
    down_sample_.BindResource();
    horizontal_blur_.Init(kDownSampleSize, format, depth_format);
    horizontal_blur_.BindResource();
    vertical_blur_.Init(kDownSampleSize, format, depth_format);
    vertical_blur_.BindResource();
    last_frame_blur_.Init(kDownSampleSize, format, depth_format);
    last_frame_blur_.BindResource();
    blend_original_.Init(size_, format, depth_format);
    blend_original_.BindResource();
    save_current_frame_.Init(kDownSampleSize, format, depth_format);
    save_current_frame_.BindResource();
  }
  effect_blur_ = LoadResource<BlurShaderEffect>(g_engine_config->blur_effect_path_.Get());
  Check(!mesh_quad_);
  mesh_quad_ = new Mesh;
  MeshBuilder::BuildScreenQuad(*mesh_quad_);
}

void PostEffectBlur::SetSize(const Size2& size) {
  Clear();
  Init(size);
}

void PostEffectBlur::Clear() {
  // Clear render targets
  {
    original_.Clear();
    down_sample_.Clear();
    horizontal_blur_.Clear();
    vertical_blur_.Clear();
    last_frame_blur_.Clear();
    blend_original_.Clear();
    save_current_frame_.Clear();
  }
  PostEffect::Clear();
  effect_blur_ = 0;
  mesh_quad_ = 0;
}

void PostEffectBlur::PreRender() {
  Check(mesh_quad_);
  Check(effect_blur_);
  original_.SetCurrent();
}

void PostEffectBlur::PostRender() {
  ScopedTechniqueNone technique; // Null render technique
  View* saved_view = g_renderer.view_state.view;
  g_renderer.view_state.view = NULL;
  StepDownSample(down_sample_, original_);
  StepHorizontalBlur(horizontal_blur_, down_sample_);
  StepVerticalBlur(vertical_blur_, horizontal_blur_);
  StepLastFrameBlur(last_frame_blur_, vertical_blur_, save_current_frame_);
  StepBlendOriginal(blend_original_, original_, last_frame_blur_);
  StepCopyCurrentFrame(save_current_frame_, last_frame_blur_);
  StepGamma(*g_renderer.view_state.target, blend_original_);
  g_renderer.view_state.view = saved_view;
}

void PostEffectBlur::SetEffectVertexOffset(ShaderEffect* effect, const Size2& size) {
  Check(effect);
  Vector4 offset(-1.f/size.x, 1.f/size.y, 0, 0);
  effect->SetVector4ByName(TXT("BlurVertexTexcoordOffset"), offset);
}

void PostEffectBlur::StepDownSample(RenderTarget& target, RenderTarget& source) {
  target.SetCurrent();
  const ColorInt kClearColor = DG_MAKE_COLOR_INT(0, 0, 0, 0);
  const bool kIsDepthClear = true;
  g_renderer.ClearScreen(kClearColor, kIsDepthClear);
  {
    SetEffectVertexOffset(effect_blur_.ptr(), target.size());
    effect_blur_->SetTechniqueByName(TXT("TechniqueForDownSample"));
    effect_blur_->SetTextureByName(TXT("SourceTextureSampler"), source.texture_.ptr());
    effect_blur_->SetBlurPhase(BlurShaderEffect::BlurPhase_DownSample);
    effect_blur_->SubmitMesh(mesh_quad_.ptr(), NULL);
  }
}

void PostEffectBlur::StepHorizontalBlur(RenderTarget& target, RenderTarget& source) {
  target.SetCurrent();
  const ColorInt kClearColor = DG_MAKE_COLOR_INT(0, 0, 0, 0);
  const bool kIsDepthClear = true;
  g_renderer.ClearScreen(kClearColor, kIsDepthClear);
  {
    SetEffectVertexOffset(effect_blur_.ptr(), target.size());
    effect_blur_->SetTechniqueByName(TXT("TechniqueForHorizontalBlur"));
    effect_blur_->SetTextureByName(TXT("SourceTextureSampler"), source.texture_.ptr());
    effect_blur_->SetBlurPhase(BlurShaderEffect::BlurPhase_Blur);
    effect_blur_->SubmitMesh(mesh_quad_.ptr(), NULL);
  }
}

void PostEffectBlur::StepVerticalBlur(RenderTarget& target, RenderTarget& source) {
  target.SetCurrent();
  const ColorInt kClearColor = DG_MAKE_COLOR_INT(0, 0, 0, 0);
  const bool kIsDepthClear = true;
  g_renderer.ClearScreen(kClearColor, kIsDepthClear);
  {
    SetEffectVertexOffset(effect_blur_.ptr(), target.size());
    effect_blur_->SetTechniqueByName(TXT("TechniqueForVerticalBlur"));
    effect_blur_->SetTextureByName(TXT("SourceTextureSampler"), source.texture_.ptr());
    effect_blur_->SetBlurPhase(BlurShaderEffect::BlurPhase_Blur);
    effect_blur_->SubmitMesh(mesh_quad_.ptr(), NULL);
  }
}

void PostEffectBlur::StepLastFrameBlur(
    RenderTarget& target,
    RenderTarget& source,
    RenderTarget& last) {
  target.SetCurrent();
  const ColorInt kClearColor = DG_MAKE_COLOR_INT(0, 0, 0, 0);
  const bool kIsDepthClear = true;
  g_renderer.ClearScreen(kClearColor, kIsDepthClear);
  {
    SetEffectVertexOffset(effect_blur_.ptr(), target.size());
    effect_blur_->SetTechniqueByName(TXT("TechniqueForLastFrameBlend"));
    effect_blur_->SetTextureByName(TXT("SourceTextureSampler"), source.texture_.ptr());
    if (!is_last_frame_updated_) {
      effect_blur_->SetTextureByName(TXT("TargetTextureSampler"), source.texture_.ptr());
    } else {
      effect_blur_->SetTextureByName(TXT("TargetTextureSampler"), last.texture_.ptr());
    }
    effect_blur_->SubmitMesh(mesh_quad_.ptr(), NULL);
  }
}

void PostEffectBlur::StepBlendOriginal(
    RenderTarget& target,
    RenderTarget& source,
    RenderTarget& last) {
  target.SetCurrent();
  const ColorInt kClearColor = DG_MAKE_COLOR_INT(0, 0, 0, 0);
  const bool kIsDepthClear = true;
  g_renderer.ClearScreen(kClearColor, kIsDepthClear);
  {
    SetEffectVertexOffset(effect_blur_.ptr(), target.size());
    effect_blur_->SetTechniqueByName(TXT("TechniqueForOriginalBlend"));
    effect_blur_->SetTextureByName(TXT("SourceTextureSampler"), source.texture_.ptr());
    effect_blur_->SetTextureByName(TXT("TargetTextureSampler"), last.texture_.ptr());
    effect_blur_->SubmitMesh(mesh_quad_.ptr(), NULL);
  }
}

void PostEffectBlur::StepCopyCurrentFrame(RenderTarget& target, RenderTarget& source) {
  target.SetCurrent();
  const ColorInt kClearColor = DG_MAKE_COLOR_INT(0, 0, 0, 0);
  const bool kIsDepthClear = true;
  // #todo no need to clear the scene. just set the flags not to update the depth buffer
  g_renderer.ClearScreen(kClearColor, kIsDepthClear);
  {
    SetEffectVertexOffset(effect_blur_.ptr(), target.size());
    effect_blur_->SetTechniqueByName(TXT("TechniqueForCopyCurrentFrame"));
    effect_blur_->SetTextureByName(TXT("SourceTextureSampler"), source.texture_.ptr());
    effect_blur_->SubmitMesh(mesh_quad_.ptr(), NULL);
  }
  is_last_frame_updated_ = true;
}

void PostEffectBlur::StepGamma(RenderTarget& target, RenderTarget& source) {
  target.SetCurrent();
  const ColorInt kClearColor = DG_MAKE_COLOR_INT(0, 0, 0, 0);
  const bool kIsDepthClear = true;
  g_renderer.ClearScreen(kClearColor, kIsDepthClear);

  {
    SetEffectVertexOffset(effect_blur_.ptr(), target.size());
    effect_blur_->SetTechniqueByName(TXT("TechniqueForGammaCorrected"));
    effect_blur_->SetTextureByName(TXT("SourceTextureSampler"), source.texture_.ptr());
    effect_blur_->SubmitMesh(mesh_quad_.ptr(), NULL);
  }
}

RenderTarget* PostEffectBlur::render_target() {
  return &original_;
}

int PostEffectBlur::GetInvScale() {
  return 4;
}

} // namespace dg
