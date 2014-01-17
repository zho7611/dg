// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class PostEffectShadow : public PostEffect {
public:
  PostEffectShadow() {}
  virtual ~PostEffectShadow();
  void Init(const Size2& size);
  void SetSize(const Size2& size);
  virtual RenderTarget* render_target();
  virtual void PreRender();
  virtual void PostRender();

private:
  RenderTarget shadow_target_;

  DG_DECLARE_RESOURCE(PostEffectShadow, PostEffect);
};

} // namespace dg
