// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

using namespace dg;

FontFace::FontFace() :
  width_(0),
  height_(0) {}

FontFace::~FontFace() {}

bool FontFace::Init() {
  return true;
}

void FontFace::Clear() {
  font_ = NULL;
}

void FontFace::RenderText(
    Texture* texture,
    const Cstr* text,
    const Point2& position) {
}
