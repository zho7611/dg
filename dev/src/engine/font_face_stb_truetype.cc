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

void FontFace::RenderText(
    Texture* texture,
    const Cstr* text,
    const Point2& position) {
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
  Check(texture->format() == GpuFormat_A8);
  int texture_pitch;
  uint8_t* texture_buffer = NULL;
  Texture::ScopedLock locker(*texture, texture_buffer, texture_pitch, GpuLockType_Discard);
  uint8_t* buff = NULL;
  int text_len = MyStrLen(text);
  if (texture_buffer) {
    MyMemSet(texture_buffer, 0, texture->size().y*texture_pitch);
    int width = 0, height = 0;
    float scaley = stbtt_ScaleForPixelHeight(stb_font_info_, (float)this->height_);
    int offsetx = 0, offsety = 0;
    for (int idx = 0; idx < kNumChars; ++idx) {
      unsigned char *bitmap;
      bitmap = stbtt_GetCodepointBitmap(stb_font_info_, 0, scaley, wide_text[idx], &width, &height, &offsetx, &offsety);
    }
  }
#if (0)
   scale = stbtt_ScaleForPixelHeight(&font, 15);
   stbtt_GetFontVMetrics(&font, &ascent,0,0);
   baseline = (int) (ascent*scale);

   while (text[ch]) {
      int advance,lsb,x0,y0,x1,y1;
      float x_shift = xpos - (float) floor(xpos);
      stbtt_GetCodepointHMetrics(&font, text[ch], &advance, &lsb);
      stbtt_GetCodepointBitmapBoxSubpixel(&font, text[ch], scale,scale,x_shift,0, &x0,&y0,&x1,&y1);
      stbtt_MakeCodepointBitmapSubpixel(&font, &screen[baseline + y0][(int) xpos + x0], x1-x0,y1-y0, 79, scale,scale,x_shift,0, text[ch]);
      // note that this stomps the old data, so where character boxes overlap (e.g. 'lj') it's wrong
      // because this API is really for baking character bitmaps into textures. if you want to render
      // a sequence of characters, you really need to render each bitmap to a temp buffer, then
      // "alpha blend" that into the working buffer
      xpos += (advance * scale);
      if (text[ch+1])
         xpos += scale*stbtt_GetCodepointKernAdvance(&font, text[ch],text[ch+1]);
      ++ch;
   }
#endif
}
