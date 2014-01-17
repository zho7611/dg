// Copyright (C) 2010 by Jiho Choi. All rights reserved.
// [License]

#include "./gui_first.h"

GlyphTexture::GlyphTexture() {
}

GlyphTexture::~GlyphTexture() {
}

void GlyphTexture::Init(  Size2 inGlyphSize ) {
  Unimplemented();
}

const GlyphTexture::GlyphItem* GlyphTexture::FindOrAdd(  const wchar_t inGlyph ) {
  Unimplemented();
  return 0;
}

bool GlyphTexture::OpenDevice() {
  return true;
}

bool GlyphTexture::CloseDevice() {
  return true;
}

Texture* GlyphTexture::GetTexture() {
  return Texture_.GetPtr();
}

const GlyphTexture::GlyphItem* GlyphTexture::Find( const wchar_t inGlyph ) {
  Unimplemented();
  return 0;
}

const GlyphTexture::GlyphItem* GlyphTexture::Add( const wchar_t inGlyph ) {
  Unimplemented();
  return 0;
}
