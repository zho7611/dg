// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

//#define STB_TRUETYPE_IMPLEMENTATION
//#include "../externals/stb_truetype/stb_truetype.h"

using namespace dg;

FontFace::FontFace() :
  width_(0),
  height_(0),
  stb_font_info_(NULL) {}

FontFace::~FontFace() {}

bool FontFace::Init() {
  stb_font_info_ = new stbtt_fontinfo();
  stbtt_InitFont(stb_font_info_, this->font_->blob_.data(), 0);
  return true;
}

void FontFace::Clear() {
  font_ = NULL;
  delete stb_font_info_;
  stb_font_info_ = NULL;
}

void FontFace::RenderText(
    Texture* texture,
    const Cstr* text,
    const Point2& position) {
  String textstr(text);
  const UnicodeStr* wide_text = textstr.GetUnicodeStr();
  Check(texture->format() == GpuFormat_A8);
  int texture_pitch;
  uint8_t* texture_buffer = NULL;
  Texture::ScopedLock locker(*texture, texture_buffer, texture_pitch, GpuLockType_Discard);
  uint8_t* buff = NULL;
  if (texture_buffer) {
    int target_w = texture->size().x;
    int target_h = texture->size().y;
    int texture_buffer_size = target_h*texture_pitch;
    MyMemSet(texture_buffer, 0, texture_buffer_size);
    int ascent, baseline, ch = 0;
    float scale, xpos = 0;
    scale = stbtt_ScaleForPixelHeight(stb_font_info_, (float)this->height_);
    stbtt_GetFontVMetrics(stb_font_info_, &ascent, 0, 0);
    // Do not add vertical height. Caller should consider it
    //baseline = (int)(ascent*scale) + position.y;
    baseline = position.y;
    int out_stride = texture_pitch;
    // Debug draw the target texture bbox
    if (0) {
      for (int y = 0; y < target_h; ++y) {
        for (int x = 0; x < target_w; ++x) {
          uint8_t* target = texture_buffer;
          if ((y == 0) || (y == (target_h-1)) || (x == 0) || (x == (target_w-1))) {
            target[y*target_w+ x] = 0x80;
          }
        }
      }
    }
    for (int idx = 0; idx < textstr.size(); ++idx) {
      int advance, lsb, x0, y0, x1, y1;
      float x_shift = xpos - (float)floor(xpos);
      stbtt_GetCodepointHMetrics(stb_font_info_, wide_text[idx], &advance, &lsb);
      stbtt_GetCodepointBitmapBoxSubpixel(
          stb_font_info_, wide_text[idx], scale, scale,
          x_shift, 0, &x0, &y0, &x1, &y1);
      int target_x = position.x + (int)xpos + x0;
      int target_y = (baseline + y0);
      int out_w = x1-x0;
      int out_h = y1-y0;
      // #todo Should consider when the glyph is located outside the target texture
      // Allocate an internal bitmap per glyph and composite them?
      Check((target_x >= 0) && (target_y >= 0));
      out_w = ((target_x + out_w) > target_w) ? (out_w - (target_x + out_w - target_w)) : out_w;
      out_h = ((target_y + out_h) > target_h) ? (out_h - (target_y + out_h - target_h)) : out_h;
      int buffer_offset = target_y*texture_pitch + target_x;
      if (buffer_offset < texture_buffer_size) {
        uint8_t* target = &texture_buffer[buffer_offset];
        stbtt_MakeCodepointBitmapSubpixel(
            stb_font_info_, target, out_w, out_h,
            out_stride, scale, scale, x_shift, 0, wide_text[idx]);
      }
      // Debug draw the bbox
      if (0) {
        for (int y = 0; y < out_h; ++y) {
          for (int x = 0; x < out_w; ++x) {
            uint8_t* target = &texture_buffer[buffer_offset];
            if ((y == 0) || (y == (out_h-1)) || (x == 0) || (x == (out_w-1))) {
              target[y*texture_pitch + x] = 0x40;
            }
          }
        }
      }
      xpos += (advance * scale);
      if (wide_text[idx+1])
         xpos += scale*stbtt_GetCodepointKernAdvance(stb_font_info_, wide_text[idx], wide_text[idx+1]);
    }
  }
}
