// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "gui_first.h"

namespace dg {

void Component::SetLocalRect(int left, int top, int right, int bottom) {
  local_rect_.Set(left, top, right, bottom);
}

void Component::ReadFromPath(const Cstr* path) {
  Chunk* chunk = CreateReadChunk(path);
  ReadFrom(chunk);
}

void Component::WriteToPath(const Cstr* path) {
  Chunk* chunk = CreateWriteChunk(path);
  WriteTo(chunk);
}

} // namespace dg
