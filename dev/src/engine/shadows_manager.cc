// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

ShadowsManager dg::g_shadow_manager;

void ShadowsManager::Init() {
  for (int idx = 0; idx < sizeof(depths_) / sizeof(depths_[0]); ++idx) {
    depths_[idx] = FLT_MAX;
  }
}

void ShadowsManager::Clear() {
  for (int idx = 0; idx < sizeof(shadow_cameras_) / sizeof(shadow_cameras_[0]); ++idx) {
    shadow_cameras_[idx] = NULL;
  }
}

} // namespace dg
