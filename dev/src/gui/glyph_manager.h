// Copyright (C) 2010 by Jiho Choi. All rights reserved.
// [License]

#pragma once

namespace Gui {
class GlyphManager {
public:
  void Open();
  void Close();
  bool OpenDevice();
  bool CloseDevice();
  void DrawGlyph(
      const Font* inFont,
      const wchar_t* inText,
      const Point2& inPosition,
      const ColorIntArgb inColor );

private:
  GlyphTexture* RegisterText(
      const Font* inFont,
      const wchar_t* inText,
      const Point2& inPosition,
      const ColorIntArgb inColor );

  typedef List<GlyphTexture*> GlyphTexturesType;
  TreeMap<const Font*, GlyphTexturesType > Texts_;
};

extern GlyphManager GlyphManager_;
}
