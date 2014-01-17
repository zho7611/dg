// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class FontText {
public:
  FontText();
  ~FontText();
  void Init(FontFace* FontFace, const Cstr* text);
  void Clear();
  void ReadFrom(Stream* stream);
  void WriteTo(Stream* out_stream);
  FontFace* GetFontFace() {
    return font_face_;
  }
  Texture* GetTexture() {
    return texture_.ptr();
  }
  const Cstr* GetText() {
    return text_.GetConst();
  }
  void DrawText(Canvas& canvas, Point2& position, ColorInt color);
  Size2 size() const {
    return size_;
  }
  Point2 GetPenPosition() const;

private:
  void Init();
  Size2 GetMargins() const;

  FontFace* font_face_;
  String text_;
  Size2 size_;
  Ptr<Texture> texture_;
};

} // namespace dg
