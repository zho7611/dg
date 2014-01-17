// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

PostEffect::PostEffect() {
  is_enabled_ = true;
}

PostEffect::~PostEffect() {
}

void PostEffect::Init(const Size2& size) {
  size_ = size;
}

void PostEffect::Clear() {}

bool PostEffect::HasDepthSurface() {
  RenderTarget* target = render_target();
  return (target) ? target->HasDepthSurface() : false;
}

} // namespace dg
