// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "gui_first.h"

namespace dg {

FontText::FontText() : font_face_(0) {
}

FontText::~FontText() {
  Clear();
}

Size2 FontText::GetMargins() const {
  Size2 font_size = font_face_->size();
  return Size2(font_size.x, font_size.y);
}

Point2 FontText::GetPenPosition() const {
  Size2 margins = GetMargins();
  return Point2(margins.x / 2, margins.y / 2 + font_face_->size().y);
}

void FontText::Init() {
  Size2 new_texture_size;
  // Set the texture size
  {
    Size2 font_size = font_face_->size();
    Size2 margins = GetMargins();
    size_.x = margins.x + text_.size() * font_size.x;
    size_.y = margins.y + font_size.y;
    new_texture_size = size_;
    new_texture_size.MakePowerOf2();
  }
  bool is_new_texture = true;
  if (texture_ && !texture_->size().IsSmallerThan(new_texture_size)) {
    is_new_texture = false;
  }
  if (is_new_texture) {
    texture_ = NULL;
    texture_ = new Texture;
    texture_->Init(
        GpuUsage_Dynamic, new_texture_size, GpuFormat_A8);
  }
}

void FontText::Init(FontFace* InFontFace, const Cstr* text) {
  font_face_ = InFontFace;
  text_.Set(text);
  Init();
}

void FontText::Clear() {
  font_face_ = NULL;
  texture_ = NULL;
  text_.Clear();
  size_ = Size2::kZero;
}

void FontText::ReadFrom(Stream* stream) {
  StreamReadAuto(stream, text_);
  DG_UNIMPLEMENTED();
}

void FontText::WriteTo(Stream* out_stream) {
  StreamWriteAuto(out_stream, text_);
  DG_UNIMPLEMENTED();
}

void FontText::DrawText(Canvas& canvas, Point2& position, ColorInt color) {
  TextDrawer::DrawText(canvas, this, position, color);
}

} // namespace dg
