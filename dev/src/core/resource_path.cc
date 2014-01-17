// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "core_first.h"

namespace dg {

ResourcePath::ResourcePath(const Cstr* path /* = 0 */) {
  if (path) {
    Set(path);
  }
}

ResourcePath::ResourcePath(const Cstr* file_path, const Cstr* chunk_name) {
  Combine(file_path, chunk_name);
}

void ResourcePath::ExtractFileChunkFromPath(
    const String& resource_path,
    String& outFilepath,
    String& outChunkName) {
  Check(resource_path.size()> 1);
  Check(resource_path.GetAt(0) != TXT('#'));
  int kPathLength = resource_path.size();
  ScopedDelete<Cstr> cstrBuff(new Cstr[kPathLength+1]);
  MyStrnCpy(cstrBuff.ptr(), resource_path.GetConst(), kPathLength+1);
  const Cstr kSeparators[] = TXT("#");
  int token_size = 0;
  const Cstr* nextBuff = string_util::GetToken(cstrBuff.ptr(), kSeparators, token_size);
  if (nextBuff) {
    outFilepath.CopyFromRange(cstrBuff.ptr(), 0, token_size);
    string_util::GetToken(nextBuff, kSeparators, token_size);
    Check(token_size);
    outChunkName.CopyFromRange(nextBuff, 0, token_size);
  } else {
    outFilepath.Set(cstrBuff.ptr());
    outChunkName.SetSize(0);
  }
}

ResourcePath& ResourcePath::Combine(const Cstr* file_path, const Cstr* chunk_name) {
  file_path_.Set(file_path);
  chunk_name_.Set(chunk_name);
  resource_path_.Set(file_path_);
  resource_path_.AddRight(TXT("#"));
  resource_path_.AddRight(chunk_name_);
  return *this;
}

void ResourcePath::Set(const Cstr* path) {
  resource_path_.Set(path);
  if (!string_util::IsEmpty(path)) {
    resource_path_.FixupPathSeparators();
    ResourcePath::ExtractFileChunkFromPath(resource_path_, file_path_, chunk_name_);
  }
}

const Cstr* ResourcePath::Get() const {
  return resource_path_.GetConst();
}

ResourcePathType ResourcePath::GetResourcePathType() const {
  if (!chunk_name_.IsEmpty()) {
    return ResourcePathType_PackageFile;
  }
  if (!file_path_.IsEmpty()) {
    return ResourcePathType_File;
  }
  return ResourcePathType_Unknown;
}

bool ResourcePath::IsEmpty() const {
  return resource_path_.IsEmpty();
}

bool ResourcePath::operator<(const ResourcePath& rhs) const {
  return string_util::IsLess(Get(), rhs.Get());
}

bool ResourcePath::operator==(const ResourcePath& rhs) const {
  return string_util::IsEqual(Get(), rhs.Get());
}

void ResourcePath::ReadFrom(Stream* stream) {
  String path;
  path.ReadFrom(stream);
  Set(path.Get());
}

void ResourcePath::WriteTo(Stream* out_stream) {
  resource_path_.WriteTo(out_stream);
}

} // namespace dg
