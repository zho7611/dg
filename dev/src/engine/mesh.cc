// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

Mesh::Mesh() :
  prim_type_(DrawPrimitiveType_None) {}

Mesh::~Mesh() {
  vertex_buffer_.UnbindResource();
  index_buffer_.UnbindResource();
}

void Mesh::ReadFrom(Stream* stream) {
  StreamReadFlat(stream, prim_type_);
  StreamReadClass(stream, vertex_buffer_);
  StreamReadClass(stream, index_buffer_);
  OnReadDone();
}

void Mesh::WriteTo(Stream* out_stream) {
  StreamWriteFlat(out_stream, prim_type_);
  StreamWriteClass(out_stream, vertex_buffer_);
  StreamWriteClass(out_stream, index_buffer_);
  OnWriteDone();
}

void Mesh::SetCurrent() {
  if (resource_state_.IsDirty()) {
    index_buffer_.BindResource();
    vertex_buffer_.BindResource();
    resource_state_.SetDirty(false);
  }
}

void Mesh::SubmitMesh() {
  SetCurrent();
  vertex_buffer_.SubmitVertexBuffer();
  index_buffer_.SubmitIndexBuffer();
  unsigned int num_primitives;
  switch (prim_type_) {
  case DrawPrimitiveType_TriangleList:
    num_primitives = index_buffer_.num_indices()/3;
    break;
  case DrawPrimitiveType_None:
    Check(0); // Primitive type is not set
    return;
    break;
  default:
    Check(0); // Not supported primitive type
    break;
  }
  RC_DrawIndexedPrimitive(prim_type_, num_primitives);
}

int Mesh::num_vertices() const {
  return vertex_buffer_.num_vertices();
}

} // namespace dg
