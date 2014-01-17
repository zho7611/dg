// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

bool RenderResource::InitResource() {
  g_renderer.resources.AddResource(this);
  return OnInitDevice();
}

bool RenderResource::ClearResource() {
  g_renderer.resources.RemoveResource(this);
  return OnClearDevice();
}

} // namespace dg
