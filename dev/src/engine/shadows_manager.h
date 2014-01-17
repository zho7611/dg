// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

// Cascaded Shadows Manager
class ShadowsManager {
public:
  void Init();
  void Clear();

  Ptr<Camera> shadow_cameras_[kNumShadowCascades];
  float depths_[kNumShadowCascades];
  Vector3 scales_[kNumShadowCascades];
  Vector3 offsets_[kNumShadowCascades];
  Vector3 original_scales_[kNumShadowCascades];
  Vector3 original_offsets_[kNumShadowCascades];
};

extern ShadowsManager g_shadow_manager;

} // namespace dg
