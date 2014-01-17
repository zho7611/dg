// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

PostEffectShadow::~PostEffectShadow() {
  shadow_target_.UnbindResource();
}

void PostEffectShadow::Init(const Size2& size) {
  PostEffect::Init(size);
  shadow_target_.Init(size, GpuFormat_A32B32G32R32F, GpuFormat_D24S8);
  OnReadDone();
}

void PostEffectShadow::SetSize(const Size2& size) {
  size_ = size;
  shadow_target_.set_size(size);
}

void PostEffectShadow::PreRender() {
  shadow_target_.SetCurrent();
}

void PostEffectShadow::PostRender() {}

RenderTarget* PostEffectShadow::render_target() {
  return &shadow_target_;
}

} // namespace dg
