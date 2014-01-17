// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

void FontFace::ReadFrom(Stream* stream) {
  StreamReadFlat(stream, width_);
  StreamReadFlat(stream, height_);
  StreamReadPointer(stream, font_);
}

void FontFace::WriteTo(Stream* out_stream) {
  StreamWriteFlat(out_stream, width_);
  StreamWriteFlat(out_stream, height_);
  StreamWritePointer(out_stream, font_);
}

bool FontFace::Init(Font* font, int width, int height) {
  Check(font);
  font_ = font;
  width_ = width;
  height_ = height;
  return Init();
}

FontFace* FontFace::GetFontFace(const Cstr* font_face_name) {
  return g_font_manager.GetFontFace(font_face_name);
}

} // namespace dg
