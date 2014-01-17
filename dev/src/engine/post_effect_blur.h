// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class ShaderEffect;
class BlurShaderEffect;
class BlendShaderEffect;
class Mesh;

class PostEffectBlur : public PostEffect {
public:
  RenderTarget original_;
  RenderTarget down_sample_;
  RenderTarget horizontal_blur_;
  RenderTarget vertical_blur_;
  RenderTarget last_frame_blur_;
  RenderTarget blend_original_;
  RenderTarget save_current_frame_;

  PostEffectBlur();
  virtual ~PostEffectBlur();
  virtual void Init(const Size2& size);
  virtual void Clear();
  virtual void SetSize(const Size2& size);
  virtual void PreRender();
  virtual void PostRender();
  virtual RenderTarget* render_target();
  static int GetInvScale();

private:
  bool is_last_frame_updated_;
  Ptr<Mesh> mesh_quad_;
  Ptr<BlurShaderEffect> effect_blur_;

  void SetEffectVertexOffset(ShaderEffect* effect, const Size2& size);
  void StepDownSample(RenderTarget& target, RenderTarget& source);
  void StepHorizontalBlur(RenderTarget& target, RenderTarget& source);
  void StepVerticalBlur(RenderTarget& target, RenderTarget& source);
  void StepBlendOriginal(RenderTarget& target, RenderTarget& source, RenderTarget& last);
  void StepLastFrameBlur(RenderTarget& target, RenderTarget& source, RenderTarget& last);
  void StepCopyCurrentFrame(RenderTarget& target, RenderTarget& source);
  void StepGamma(RenderTarget& target, RenderTarget& source);

  DG_DECLARE_RESOURCE(PostEffectBlur, PostEffect);
};

} // namespace dg
