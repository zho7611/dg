// Copyright (C) 2010 by Jiho Choi. All rights reserved.
// [License]

#pragma once

namespace Gui {
class GlyphTexture {
public:
  class GlyphItem {
  public:
    wchar_t Glyph_;
    Rect Rect_;
    Point2 DrawOffset;
  };

  GlyphTexture();
  ~GlyphTexture();
  void Init(  Size2 inGlyphSize );
  const GlyphItem* FindOrAdd(  const wchar_t inGlyph );
  bool OpenDevice();
  bool CloseDevice();
  Texture* GetTexture();

private:
  const GlyphItem* Find( const wchar_t inGlyph );
  const GlyphItem* Add( const wchar_t inGlyph );

  Size2 GlyphSize_;
  List<GlyphItem> EmptyItems_;
  TreeMap<wchar_t, GlyphItem> ActiveItems_;
  Ptr<Texture> Texture_;
};
}
