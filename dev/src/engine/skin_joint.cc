// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

void SkinJoint::ReadFrom(Stream* stream) {
  StreamReadFlat(stream, *this);
}

void SkinJoint::WriteTo(Stream* out_stream) {
  StreamWriteFlat(out_stream, *this);
}

SkinMesh::SkinMesh() {
}

SkinMesh::~SkinMesh() {
}

} // namespace dg
