// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"
#include <ft2build.h>
#include FT_FREETYPE_H

namespace dg {

#if defined(DG_TARGET_WIN32)
  #if defined(DG_DEBUG)
    #pragma comment(lib, "../externals/freetype2/objs/freetype221MT_D.lib")
  #else
    #pragma comment(lib, "../externals/freetype2/objs/freetype221MT.lib")
  #endif
#endif

static FT_Library FontLibraryFt2_ = NULL;

void FontManager::InitImpl() {
  Check(!FontLibraryFt2_);
  FT_Init_FreeType(&FontLibraryFt2_);
}

void FontManager::ClearImpl() {
  Check(FontLibraryFt2_);
  FT_Done_FreeType(FontLibraryFt2_);
}

void* FontManager::GetFontLibrary() {
  Check(FontLibraryFt2_);
  return FontLibraryFt2_;
}

} // namespace dg
