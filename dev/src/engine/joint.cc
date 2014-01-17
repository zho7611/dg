// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

Joint::Joint() :
  parent_index_(kInvalidIndex),
  index_(kInvalidIndex) {}

Joint::~Joint() {}

void Joint::ReadFrom(Stream* stream) {
  StreamReadClass(stream, id_);
  StreamReadFlat(stream, index_);
  StreamReadFlat(stream, parent_index_);
  StreamReadClass(stream, inverse_bind_matrix_);
}

void Joint::WriteTo(Stream* out_stream) {
  StreamWriteClass(out_stream, id_);
  StreamWriteFlat(out_stream, index_);
  StreamWriteFlat(out_stream, parent_index_);
  StreamWriteClass(out_stream, inverse_bind_matrix_);
}

} // namespace dg
