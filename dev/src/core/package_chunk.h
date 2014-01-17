// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class PackageChunk {
public:
  static Chunk* CreateReadChunk(const Cstr* path);
  static Chunk* CreateWriteChunk(const Cstr* path);
};

} // namespace dg
