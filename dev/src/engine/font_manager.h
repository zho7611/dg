// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class FontManager {
public:
  void Init();
  void Clear();
  // Init all font faces from the list
  void InitFaces();
  // Clear all font faces from the list
  void ClearFaces();
  void ReadFrom(Stream* stream);
  void WriteTo(Stream* out_stream);
  FontFace* GetFontFace(const Cstr* font_face_name);
  void Add(const Cstr* name, FontFace* font_face) {
    font_faces_.Add(name, font_face);
  }

#if defined(DG_FONT_FREETYPE)
private:
  friend class FontFace;
  static void* GetFontLibrary();
#endif

private:
  void InitImpl();
  void ClearImpl();

  typedef StringMap<FontFace*> FontFaceType;

  FontFaceType font_faces_;
};

extern FontManager g_font_manager;

} // namespace dg
