// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

Light::Light() :
    position_(Vector3::kZero),
    ambient_color_(0.2f, 0.2f, 0.2f, 1.0f),
    diffuse_color_(1.0f, 1.0f, 1.0f, 1.0f),
    direction_(0, 1, 0),
    is_cast_shadow_(true),
    is_enabled_(true),
    type_(LightType_Directional) {}

Light::~Light() {}

} // namespace dg
