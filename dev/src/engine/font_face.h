// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

#if defined(DG_FONT_FREETYPE)
struct FT_FaceRec_;
#endif

namespace dg {

// A font face with same size and same font resource
class FontFace {
public:
  FontFace();
  ~FontFace();
  bool Init();
  bool Init(Font* font, int width, int height);
  void Clear();
  virtual void ReadFrom(Stream* stream);
  virtual void WriteTo(Stream* out_stream);
  // Render the text into the texture
  // Position is the local coordinate of the texture
  void RenderText(
      class Texture* texture,
      const Cstr* text,
      const Point2& position);
  Size2 size() const {
    return Size2(width_, height_);
  }
  static FontFace* GetFontFace(const Cstr* font_face_name);

private:
  Ptr<Font> font_;
  // A font face width in pixel
  int width_;
  // A font face height in pixel
  int height_;

#if defined(DG_FONT_FREETYPE)
  FT_FaceRec_* face_ft2_;
#endif
};

} // namespace dg
