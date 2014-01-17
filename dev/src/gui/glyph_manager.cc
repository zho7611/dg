// Copyright (C) 2010 by Jiho Choi. All rights reserved.
// [License]

#include "./gui_first.h"

GlyphManager Gui::GlyphManager_;

void GlyphManager::Open() {
}

void GlyphManager::Close() {
}

bool GlyphManager::OpenDevice() {
  return true;
}

bool GlyphManager::CloseDevice() {
  return true;
}

void GlyphManager::DrawGlyph(
    const Font* inFont,
    const wchar_t* inText,
    const Point2& inPosition,
    const ColorIntArgb inColor ) {
  Unimplemented();
}

GlyphTexture* GlyphManager::RegisterText(
    const Font* inFont,
    const wchar_t* inText,
    const Point2& inPosition,
    const ColorIntArgb inColor ) {
  Unimplemented();
  return 0;
}
