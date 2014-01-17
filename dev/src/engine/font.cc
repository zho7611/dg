// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

Font::Font() {}

Font::~Font() {}

void Font::ReadFrom(Stream* stream) {
  StreamReadClass(stream, blob_);
  OnReadDone();
}

void Font::WriteTo(Stream* out_stream) {
  StreamWriteClass(out_stream, blob_);
  OnWriteDone();
}

} // namespace dg
