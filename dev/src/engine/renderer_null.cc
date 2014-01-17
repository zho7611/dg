// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

Renderer::Renderer() :
  is_device_lost_(false),
  view_(NULL),
  phase_(Phase_Normal),
  is_wire_mode(0),
  is_in_begin_scene_(false) {}

Renderer::~Renderer() {}

void Renderer::Init() {}

void Renderer::Clear() {}

bool Renderer::BindResource() {
  return true;
}

bool Renderer::UnbindResource() {
  return true;
}

bool Renderer::OnInitDevice() {
  return true;
}

bool Renderer::OnClearDevice() {
  return true;
}

void Renderer::BeginScene() {
  Check(!is_in_begin_scene_);
  if (!is_in_begin_scene_) {
    is_in_begin_scene_ = true;
  }
}

void Renderer::EndScene() {
  Check(is_in_begin_scene_);
  if (is_in_begin_scene_) {
    is_in_begin_scene_ = false;
  }
}

bool Renderer::ResetRendererDevice() {
  return true;
}
