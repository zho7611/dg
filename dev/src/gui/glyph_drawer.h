// Copyright (C) 2010 by Jiho Choi. All rights reserved.
// [License]

#pragma once

namespace Gui {
class GlyphDrawer {
public:
  void PreRender();
  void PostRender();
  void DrawGlyph(
      const GlyphTexture::GlyphItem& inGlyphItem,
      const Texture* inTexture,
      const Point2& inStartInScreen,
      const Point2& inEndInSceen,
      const ColorInt& inColor );
  bool OpenDevice();
  bool CloseDevice();

private:
  VertexDecl VertexDecl_;
  Ptr<EffectLine> EffectFont_;
};
}