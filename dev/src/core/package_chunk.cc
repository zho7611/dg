// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "core_first.h"

namespace dg {

static Chunk* CreatePackageChunk(const Cstr* path, enum Package::OpenChunkOption option) {
  ResourcePath resource_path(path);
  Package* package = static_cast<Package*>(g_resource_manager->FindResource(resource_path.GetFilePath()));
  if (!package) {
    package = LoadResource<Package>(resource_path.GetFilePath());
  }
  if (!package) {
    return NULL;
  }
  return package->OpenChunk(resource_path.GetChunkName(), option);
}

Chunk* PackageChunk::CreateReadChunk(const Cstr* path) {
  return CreatePackageChunk(path, Package::kRead);
}

Chunk* PackageChunk::CreateWriteChunk(const Cstr* path) {
  return CreatePackageChunk(path, Package::kWrite);
}

} // namespace dg
