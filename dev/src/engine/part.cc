// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

Part::Part() {}

Part::~Part() {}

void Part::ReadFrom(Stream* stream) {
  StreamReadPointer(stream, mesh_);
  StreamReadPointer(stream, material_);
  OnReadDone();
}

void Part::WriteTo(Stream* out_stream) {
  StreamWritePointer(out_stream, mesh_);
  StreamWritePointer(out_stream, material_);
  OnWriteDone();
}

void Part::UpdateTransform() {
  mesh_->UpdateTransform();
}

void Part::BindResource() {
  if (mesh_) {
    mesh_->BindResource();
  }
  if (material_) {
    material_->BindResource();
  }
}

} // namespace dg
