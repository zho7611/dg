// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

void SimpleShaderEffect::SetDepthTexture() {
  View* view = g_renderer.view_state.view;
  if (view) {
    RenderTarget* depth_target = view->depth_target();
    if (depth_target && !depth_target->size().IsZero()) {
      SetTextureByName(TXT("DepthTextureSampler"), depth_target->texture_.ptr());
    }
  }
}

void SimpleShaderEffect::SetShadowTexture() {
  View* light_view = g_renderer.view_state.light_view;
  if (light_view) {
    RenderTarget* shadow_target = light_view->render_target();
    if (shadow_target && !shadow_target->size().IsZero()) {
      SetTextureByName(TXT("ShadowTextureSampler"), shadow_target->texture_.ptr());
    }
  }
}

void SimpleShaderEffect::SetupEffect() {
  const int kShadowMapSizeX = g_engine_config->shadow_map_size_.x;
  const int kShadowMapSizeY = g_engine_config->shadow_map_size_.y;
  const float kOffsetX = 1.f / kShadowMapSizeX;
  const float kOffsetY = 1.f / kShadowMapSizeY;
  const float kOffset2X = 2.f / kShadowMapSizeX;
  const float kOffset2Y = 2.f / kShadowMapSizeY;
  const float kShadowMapOffsets[] = {
    -kOffset2X, -kOffset2Y, 0, -kOffset2Y,
    kOffset2X, -kOffset2Y, -kOffsetX, -kOffsetY,
    kOffsetX, -kOffsetY, -kOffset2X, 0,
    kOffset2X, 0, -kOffsetX, kOffsetY,
    kOffsetX, kOffsetY, -kOffset2X, kOffset2Y,
    0, kOffset2Y, kOffset2X, kOffset2Y
  };
  SetVector4ArrayByName(TXT("GlobalShadowMapOffsets"), kShadowMapOffsets, 6);
  SetMatrixByName(TXT("GlobalWorldMatrix"), g_renderer.view_state.world_matrix);
  SetViewParams();
  SetShadowsParams();
#if WITH_DEPTHOFFIELD
  SetDepthTexture();
#endif
  SetShadowTexture();
  SetLight(g_engine.light_manager->global_light_);
}

void BlendShaderEffect::SetupEffect() {
}

void BlurShaderEffect::SetupEffect() {
  // Computes texture offsets table
  // #todo Not to call this method in every frame if not changed
  // Settings for down-sampling
  Size2 render_target_size;
  GetRenderTargetSize(render_target_size);
  {
    const float kTexelScaleX = 1.f / render_target_size.x;
    const float kTexelScaleY = 1.f / render_target_size.y;
    const int kNumDownSamples = 16;
#if defined(DG_RENDERER_GL2)
    const float kDownSampleOffsetScales[kNumDownSamples][2] = {
      { 1.0, -1.0 }, { 1.0, -0.0 }, { 1.0, 0.0 }, { 1.0, 1.0 },
      { 0.0, -1.0 }, { 0.0, -0.0 }, { 0.0, 0.0 }, { 0.0, 1.0 },
      {-0.0, -1.0 }, {-0.0, -0.0 }, {-0.0, 0.0 }, {-0.0, 1.0 },
      {-1.0, -1.0 }, {-1.0, -0.0 }, {-1.0, 0.0 }, {-1.0, 1.0 }
    };
#else
    const float kDownSampleOffsetScales[kNumDownSamples][2] = {
      { 1.5, -1.5 }, { 1.5, -0.5 }, { 1.5, 0.5 }, { 1.5, 1.5 },
      { 0.5, -1.5 }, { 0.5, -0.5 }, { 0.5, 0.5 }, { 0.5, 1.5 },
      {-0.5, -1.5 }, {-0.5, -0.5 }, {-0.5, 0.5 }, {-0.5, 1.5 },
      {-1.5, -1.5 }, {-1.5, -0.5 }, {-1.5, 0.5 }, {-1.5, 1.5 }
    };
#endif
    float down_sample_offsets[kNumDownSamples][4] = {0, };
    // Convert to texel
    for (int idx = 0; idx < kNumDownSamples; ++idx) {
      down_sample_offsets[idx][0] = kDownSampleOffsetScales[idx][0] * kTexelScaleX;
      down_sample_offsets[idx][1] = kDownSampleOffsetScales[idx][1] * kTexelScaleY;
    }
    SetVector4ArrayByName(TXT("DownSampleOffsets"),
        &down_sample_offsets[0][0], kNumDownSamples);
  }
  // Settings for blurring
  {
    float kTexelScaleX = float(PostEffectBlur::GetInvScale()) / render_target_size.x;
    float kTexelScaleY = float(PostEffectBlur::GetInvScale()) / render_target_size.y;
    const int kNumBlurSamples = 15;
    const int kNumHalfSamples = kNumBlurSamples/2;
    float blur_map_offsets[kNumBlurSamples][4] = {0, };
    float blur_map_weights[kNumBlurSamples][4] = {0, };
    float weight_sum = 0;
    for (int idx = 0; idx < kNumBlurSamples; ++idx) {
      const float weight = float( idx - kNumHalfSamples ) / kNumHalfSamples;
      weight_sum += blur_map_weights[idx][0] = expf(-8.f*weight*weight);
    }
    for (int idx = 0; idx < kNumBlurSamples; ++idx) {
      blur_map_offsets[idx][0] = kTexelScaleX*(idx - kNumHalfSamples);
      blur_map_offsets[idx][1] = kTexelScaleY*(idx - kNumHalfSamples);
      blur_map_weights[idx][0] /= weight_sum;
    }
    SetVector4ArrayByName(TXT("BlurMapOffsets"),
        &(blur_map_offsets[0][0]), kNumBlurSamples);
    SetVector4ArrayByName(TXT("BlurMapWeights"),
        &blur_map_weights[0][0], kNumBlurSamples);
  }
  // Setting for the time delta
  Vector4 blur_time_delta_in_sec(g_renderer.timer.GetDeltaInSeconds());
  SetVector4ByName(TXT("BlurTimeDeltaInSec"), blur_time_delta_in_sec);
  Vector4 global_gamma_correction(g_engine_config->global_gamma_correction_);
  SetVector4ByName(TXT("GlobalGammaCorrection"), global_gamma_correction);
  Vector4 global_bloom_weight(g_engine_config->global_bloom_weight_);
  SetVector4ByName(TXT("GlobalBloomWeight"), global_bloom_weight);
}

BlurShaderEffect::BlurShaderEffect() : blur_phase_(BlurPhase_None) {
}

BlurShaderEffect::~BlurShaderEffect() {
}

void SpriteShaderEffect::PreRender(Sprite::Type sprite_type) {
  switch(sprite_type) {
  case Sprite::Type_Opaque:
    SetTechniqueByName(TXT("TechniqueForOpaque"));
    break;
  case Sprite::Type_Translucent:
    SetTechniqueByName(TXT("TechniqueForTranslucent"));
    break;
  case Sprite::Type_Font:
    SetTechniqueByName(TXT("TechniqueForFont"));
    break;
  default:
    DG_UNREACHABLE();
    SetTechnique(0);
    break;
  }
  BeginEffect();
  BeginPass(0);
}

void SpriteShaderEffect::PostRender() {
  EndPass();
  EndEffect();
}

void SpriteShaderEffect::SubmitQuad(
    Sprite* sprite,
    Texture* texture,
    const Rect& rect,
    const Rect& tex_coord_rect,
    const ColorInt& color) {
  if (texture) texture->BindResource();
  // Setup effect parameters
  {
    Vector4 surface_size;
    Size2 render_target_size;
    GetRenderTargetSize(render_target_size);
    surface_size.x = 1.f / (render_target_size.x);
    surface_size.y = 1.f / (render_target_size.y);
    surface_size.z = 0;
    surface_size.w = 0;
    SetVector4ByName(TXT("GlobalInvSurfaceSize"), surface_size);
    SetTextureByName(TXT("SpriteTextureSampler"), texture);
  }
  CommitChanges();
  MeshBuilder::DrawQuad(rect, tex_coord_rect, (texture) ? texture->size() : Size2(0, 0), color);
}

LineShaderEffect::LineShaderEffect() : is_begun_(false) {
}

void LineShaderEffect::PreRender() {
  BindResource();
  SetTechnique(0);
  BeginEffect();
  BeginPass(0);
  SetViewParams();
  CommitChanges();
  Check(!is_begun_);
  is_begun_ = true;
}

void LineShaderEffect::PostRender() {
  EndPass();
  EndEffect();
  Check(is_begun_);
  is_begun_ = false;
}

void LineShaderEffect::SubmitLine(const Vector3& start, const Vector3& end, const ColorInt& color) {
  Check(is_begun_);
  MeshBuilder::DrawLine(start, end, color);
}

void Line2ShaderEffect::PreRender() {
  BindResource();
  SetTechnique(0);
  BeginEffect();
  BeginPass(0);
  {
    Vector4 surface_size;
    Size2 render_target_size;
    GetRenderTargetSize(render_target_size);
    surface_size.x = 1.f / (render_target_size.x);
    surface_size.y = 1.f / (render_target_size.y);
    surface_size.z = 0;
    surface_size.w = 0;
    SetVector4ByName(TXT("GlobalInvSurfaceSize"), surface_size);
  }
  SetViewParams();
  CommitChanges();
  Check(!is_begun_);
  is_begun_ = true;
}

void Line2ShaderEffect::PostRender() {
  EndPass();
  EndEffect();
  Check(is_begun_);
  is_begun_ = false;
}

void Line2ShaderEffect::SubmitLine(Point2 start_point, Point2 end_point, ColorInt color) {
  Check(is_begun_);
#if defined(DG_RENDERER_DX9)
  // Make them have the minimum extent
  if (end_point.x > start_point.x) {
    end_point.x++;
  } else {
    start_point.x++;
  }
  if (end_point.y > start_point.y) {
    end_point.y++;
  } else {
    start_point.y++;
  }
#endif
  MeshBuilder::DrawLine(
      Vector3((float)start_point.x, (float)start_point.y, 0.f),
      Vector3((float)end_point.x, (float)end_point.y, 0.f),
      color);
}

void ImguiShaderEffect::PreRender() {
  BindResource();
  SetTechnique(0);
  BeginEffect();
  BeginPass(0);
  Check(!is_begun_);
  is_begun_ = true;
}

void ImguiShaderEffect::PostRender() {
  EndPass();
  EndEffect();
  Check(is_begun_);
  is_begun_ = false;
}

} // namespace dg
