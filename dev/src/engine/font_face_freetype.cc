// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"
#include <ft2build.h>
#include FT_FREETYPE_H

namespace dg {

class FaceBitmapDesc {
public:
  int left;
  int top_;
  FT_Bitmap* bitmap_;
  int bitmap_pitch_;
  int texture_pitch_;
  int texture_width_;
  int texture_height_;

  void FillTextureAsGray(uint8_t* texture_buffer) {
    int width = bitmap_->width;
    int height = bitmap_->rows;
    for (int y = 0, target_y = top_; y < height; ++y, ++target_y) {
      for (int x = 0, target_x = left; x < width; ++x, ++target_x) {
        if ((target_y < 0) || (target_x < 0) ||
            (target_x >= texture_width_) ||
            (target_y >= texture_height_)) {
          continue;
        }
        texture_buffer[target_y * texture_pitch_ + target_x] =
            bitmap_->buffer[y * width + x];
      }
    }
  }

  void FillTextureAsMono(uint8_t* texture_buffer) {
    int width = bitmap_->width;
    int height = bitmap_->rows;
    int idx = 0;
    unsigned char bitmap_data = 0;
    for (int y = 0, target_y = top_; y < height; ++y, ++target_y) {
      for (int x = 0, targetX = left; x < bitmap_pitch_; x++, targetX += 8) {
        if ((target_y < 0) ||
            (targetX < 0) ||
            (targetX >= texture_width_) ||
            (target_y >= texture_height_)) {
          continue;
        }
        idx = target_y * texture_pitch_ + targetX;
        bitmap_data = bitmap_->buffer[y * bitmap_pitch_ + x];
        texture_buffer[idx++] = (bitmap_data & 0x80) ? 255 : 0;
        texture_buffer[idx++] = (bitmap_data & 0x40) ? 255 : 0;
        texture_buffer[idx++] = (bitmap_data & 0x20) ? 255 : 0;
        texture_buffer[idx++] = (bitmap_data & 0x10) ? 255 : 0;
        texture_buffer[idx++] = (bitmap_data & 0x8) ? 255 : 0;
        texture_buffer[idx++] = (bitmap_data & 0x4) ? 255 : 0;
        texture_buffer[idx++] = (bitmap_data & 0x2) ? 255 : 0;
        texture_buffer[idx++] = (bitmap_data & 0x1) ? 255 : 0;
      }
    }
  }
};

FontFace::FontFace() :
  width_(0),
  height_(0),
  face_ft2_(NULL) {}

FontFace::~FontFace() {
  Check(face_ft2_ == 0);
}

bool FontFace::Init() {
  FT_Library font_library = reinterpret_cast<FT_Library>(
      g_font_manager.GetFontLibrary());
  Check(!font_->blob_.IsEmpty());
  FT_Error err = FT_New_Memory_Face(
      font_library,
      font_->blob_.data_const(),
      font_->blob_.size(),
      0,
      &face_ft2_);
  Check(!err);
  FT_Set_Pixel_Sizes(face_ft2_, width_, height_);
  return true;
}

void FontFace::Clear() {
  font_ = NULL;
  if (face_ft2_) {
    FT_Error err = FT_Done_Face(face_ft2_);
    Check(!err);
    face_ft2_ = NULL;
  }
}

void FontFace::RenderText(
    Texture* texture,
    const Cstr* text,
    const Point2& position) {
  Check(face_ft2_);
  FT_GlyphSlot slot = face_ft2_->glyph;
  UnicodeStr* wide_text = NULL;
#ifdef _UNICODE
  const int kNumChars = string_util::GetLength(text);
#else
  const int kNumChars = string_util::AnsiToUnicode(text, -1, wide_text, 0);
#endif
  Check(kNumChars > 0);
  wide_text = new UnicodeStr[kNumChars+1];
#ifdef _UNICODE
  MyStrnCpy(wide_text, text, kNumChars+1);
#else
  string_util::AnsiToUnicode(text, -1, wide_text, kNumChars);
#endif
  FT_Vector pen;
  pen.x = position.x;
  pen.y = position.y;
  Check(texture->format() == GpuFormat_A8);
  int texture_pitch;
  uint8_t* texture_buffer = NULL;
  Texture::ScopedLock locker(*texture, texture_buffer, texture_pitch, GpuLockType_Discard);
  if (texture_buffer) {
    MyMemSet(texture_buffer, 0, texture->size().y*texture_pitch);
    FaceBitmapDesc bitmap_desc;
    bitmap_desc.texture_pitch_ = texture_pitch;
    bitmap_desc.texture_width_ = texture->size().x;
    bitmap_desc.texture_height_ = texture->size().y;
    bitmap_desc.bitmap_ = &slot->bitmap;
    bitmap_desc.left = pen.x;
    bitmap_desc.top_ = 0;
    bitmap_desc.bitmap_pitch_ = 0;
    for (int idx = 0; idx < kNumChars; ++idx) {
      FT_ULong char_code = wide_text[idx];
      FT_Load_Char(face_ft2_, char_code, FT_LOAD_RENDER);
      if (slot->bitmap.pixel_mode == FT_PIXEL_MODE_MONO) {
        // In mono mode, the bitmap_left is invalid
        bitmap_desc.left = pen.x;
        bitmap_desc.top_ = pen.y - slot->bitmap_top;
        bitmap_desc.bitmap_pitch_ = slot->bitmap.pitch;
        bitmap_desc.FillTextureAsMono(texture_buffer);
      } else {
        bitmap_desc.left = pen.x + slot->bitmap_left;
        bitmap_desc.top_ = pen.y - slot->bitmap_top;
        bitmap_desc.FillTextureAsGray(texture_buffer);
      }
      pen.x += slot->advance.x >> 6;
      pen.y += slot->advance.y >> 6;
    }
  }
  delete [] wide_text;
}

} // namespace dg

