// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

LightManager::LightManager() :
  global_light_(NULL) {
}

LightManager::~LightManager() {
}

void LightManager::Init() {
  global_light_ = new Light;
}

void LightManager::Clear() {
  DG_SAFE_DELETE(global_light_);
}

} // namespace dg
