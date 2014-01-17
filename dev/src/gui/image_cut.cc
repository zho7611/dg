// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "gui_first.h"

namespace dg {

void ImageCut::ReadFrom(Stream* stream) {
  StreamReadClass(stream, draw_rect);
  StreamReadClass(stream, tex_coord_rect);
  StreamReadPointer(stream, texture);
}

void ImageCut::WriteTo(Stream* out_stream) {
  StreamWriteClass(out_stream, draw_rect);
  StreamWriteClass(out_stream, tex_coord_rect);
  StreamWritePointer(out_stream, texture);
}

} // namespace dg
