// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

using namespace dg;

FontManager dg::g_font_manager;

void FontManager::ReadFrom(Stream* stream) {
  int num_items;
  StreamReadFlat(stream, num_items);
  for (int idx = 0; idx < num_items; ++idx) {
    FontFace* font_face = new FontFace;
    String face_name;
    StreamReadAuto(stream, face_name);
    font_face->ReadFrom(stream);
    font_faces_.Add(face_name, font_face);
  }
}

void FontManager::WriteTo(Stream* out_stream) {
  int num_items = font_faces_.Count();
  StreamWriteFlat(out_stream, num_items);
  FontFaceType::Iterator it = font_faces_.Begin();
  FontFaceType::Iterator ie = font_faces_.End();
  for (; it != ie; ++it) {
    String& faceName = it.Key();
    StreamWriteAuto(out_stream, faceName);
    FontFace* FontFace = it.Value();
    FontFace->WriteTo(out_stream);
  }
}

void FontManager::Init() {
  InitImpl();
  InitFaces();
}

void FontManager::InitFaces() {
  FontFaceType::Iterator it = font_faces_.Begin();
  FontFaceType::Iterator ie = font_faces_.End();
  for (; it != ie; ++it) {
    FontFace* font_face = it.Value();
    font_face->Init();
  }
}

void FontManager::ClearFaces() {
  FontFaceType::Iterator it = font_faces_.Begin();
  FontFaceType::Iterator ie = font_faces_.End();
  for (; it != ie; ++it) {
    FontFace* font_face = it.Value();
    font_face->Clear();
  }
  dg::DeleteAll(font_faces_);
  font_faces_.Clear();
}

void FontManager::Clear() {
  ClearFaces();
  ClearImpl();
}

FontFace* FontManager::GetFontFace(const Cstr* font_face_name) {
  FontFaceType::Iterator it = font_faces_.Find(font_face_name);
  if (it != font_faces_.End()) {
    return it.Value();
  }
  return 0;
}
