// Copyright (C) 2010 by Jiho Choi. All rights reserved.
// [License]

#include "./graphics_first.h"

#if defined( CoreFontFreetype )

#include <ft2build.h>
#include FT_FREETYPE_H

#if defined( CoreDebug )
#pragma comment ( lib, "../Externals/FreeType 2/objs/freetype221MT_D.lib" )
#else
#pragma comment ( lib, "../Externals/FreeType 2/objs/freetype221MT.lib" )
#endif

static FT_Library FontLibraryFt2_ = NULL;

struct BitmapToTextureParam {
  int Left_;
  int Top_;
  unsigned char* TextureBuffer_;
  FT_Bitmap* Bitmap_;
  int BitmapPitch_;
  int TexturePitch_;
  int TextureWidth_;
  int TextureHeight_;
};

void FontManager::Open() {
  Check( !FontLibraryFt2_ );
  FT_Init_FreeType( &FontLibraryFt2_ );
}

void FontManager::Close() {
  Check( FontLibraryFt2_ );
  FT_Done_FreeType( FontLibraryFt2_ );
}

Font::Font() : Width_(0),
  Height_(0),
  FaceFt2_( NULL ) {
}

Font::~Font() {
  Check( !FaceFt2_ );
}

void Font::ReadFromChunk( Chunk& inChunk ) {
  inChunk.ReadFlat( Width_ );
  inChunk.ReadFlat( Height_ );
  inChunk.ReadClass( Blob_ );
}

void Font::WriteToChunk( Chunk& outChunk ) {
  outChunk.WriteFlat( Width_ );
  outChunk.WriteFlat( Height_ );
  outChunk.WriteClass( Blob_ );
}

bool Font::OpenDevice() {
  FT_Error curError;
  curError = FT_New_Memory_Face(
      FontLibraryFt2_,
      Blob_.GetDataConst(),
      Blob_.GetSize(),
      0,
      &FaceFt2_ );
  Check( !curError );
  FT_Set_Pixel_Sizes( FaceFt2_, Width_, Height_ );
  return true;
}

bool Font::CloseDevice() {
  Check( FaceFt2_ );
  FT_Error curError = FT_Done_Face( FaceFt2_ );
  Check( !curError );
  FaceFt2_ = 0;
  return true;
}

static void BitmapToTextureGray( BitmapToTextureParam& inParam ) {
  int width = inParam.Bitmap_->width;
  int height = inParam.Bitmap_->rows;
  for ( int Y = 0, TargetY = inParam.Top_; Y < height; ++Y, ++TargetY ) {
    for ( int X = 0, TargetX = inParam.Left_; X < width; ++X, ++TargetX ) {
      if ( ( TargetY < 0 ) || ( TargetX < 0 ) || ( TargetX >= inParam.TextureWidth_ ) ||
          ( TargetY >= inParam.TextureHeight_ ) ) {
        continue;
      }
      inParam.TextureBuffer_[ TargetY*inParam.TexturePitch_ + TargetX ] =
          inParam.Bitmap_->buffer[  Y*width + X ];
    }
  }
}

static void BitmapToTextureMono( BitmapToTextureParam& inParam ) {
  int width = inParam.Bitmap_->width;
  int height = inParam.Bitmap_->rows;
  int idx = 0;
  int bitmapData = 0;
  for ( int Y = 0, TargetY = inParam.Top_; Y < height; ++Y, ++TargetY ) {
    for ( int X = 0, TargetX = inParam.Left_; X < inParam.BitmapPitch_; X++, TargetX+=8 ) {
      if ( ( TargetY < 0 ) ||
          ( TargetX < 0 ) ||
          ( TargetX >= inParam.TextureWidth_ ) ||
          ( TargetY >= inParam.TextureHeight_ ) ) {
        continue;
      }
      idx = TargetY*inParam.TexturePitch_ + TargetX;
      bitmapData = inParam.Bitmap_->buffer[  Y*inParam.BitmapPitch_ + X ];
      inParam.TextureBuffer_[ idx++] = ( bitmapData & 0x80 ) ? 255 : 0;
      inParam.TextureBuffer_[ idx++] = ( bitmapData & 0x40 ) ? 255 : 0;
      inParam.TextureBuffer_[ idx++] = ( bitmapData & 0x20 ) ? 255 : 0;
      inParam.TextureBuffer_[ idx++] = ( bitmapData & 0x10 ) ? 255 : 0;
      inParam.TextureBuffer_[ idx++] = ( bitmapData & 0x8 ) ? 255 : 0;
      inParam.TextureBuffer_[ idx++] = ( bitmapData & 0x4 ) ? 255 : 0;
      inParam.TextureBuffer_[ idx++] = ( bitmapData & 0x2 ) ? 255 : 0;
      inParam.TextureBuffer_[ idx++] = ( bitmapData & 0x1 ) ? 255 : 0;
    }
  }
}

void Font::RenderText( Texture* inTexture, const char* inText, const Point2& inPosition ) {
  Check( FaceFt2_ );
  FT_GlyphSlot slot = FaceFt2_->glyph;
  wchar_t* wideText = NULL;
  int numWideChar = ::MultiByteToWideChar( CP_ACP, 0, inText, -1, wideText, 0 );
  Check( numWideChar > 0 );
  wideText = new wchar_t[ numWideChar ];
  ::MultiByteToWideChar( CP_ACP, 0, inText, -1, wideText, numWideChar );
  --numWideChar; // Exclude the null-terminator
  FT_Vector thePen;
  {
    thePen.x = inPosition.X_;
    thePen.y = inPosition.Y_;
  }
  Check( inTexture->GetFormat() == Device::Format_A8 );
  int texturePitch;
  unsigned char* textureBuffer = NULL;
  Texture::ScopedLock locker( *inTexture, textureBuffer, texturePitch );
  ::memset( textureBuffer, 0, inTexture->GetSize().Y_*texturePitch );
  BitmapToTextureParam param;
  {
    param.TextureBuffer_ = textureBuffer;
    param.TexturePitch_ = texturePitch;
    param.TextureWidth_ = inTexture->GetSize().X_;
    param.TextureHeight_ = inTexture->GetSize().Y_;
    param.Bitmap_ = &slot->bitmap;
    param.Left_ = thePen.x;
    param.Top_ = 0;
    param.BitmapPitch_ = 0;
  }
  for ( int idx = 0; idx < numWideChar; ++idx ) {
    FT_ULong theCharCode = wideText[ idx ];
    FT_Load_Char( FaceFt2_, theCharCode, FT_LOAD_RENDER );
    if ( slot->bitmap.pixel_mode == FT_PIXEL_MODE_MONO ) {
      // In mono mode, the bitmap_left is invalid.
      param.Left_ = thePen.x;
      param.Top_ = thePen.y - slot->bitmap_top;
      param.BitmapPitch_ = slot->bitmap.pitch;
      BitmapToTextureMono( param );
    } else {
      param.Left_ = thePen.x + slot->bitmap_left;
      param.Top_ = thePen.y - slot->bitmap_top;
      BitmapToTextureGray( param );
    }
    thePen.x += slot->advance.x >> 6;
    thePen.y += slot->advance.y >> 6;
  }
  delete [ ] wideText;
}
#endif
