// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

enum ResourcePathType {
  ResourcePathType_Unknown = 0,
  ResourcePathType_File = 1,
  ResourcePathType_PackageFile = 2,
};

class ResourcePath {
public:
  ResourcePath(const Cstr* path = 0);
  ResourcePath(const Cstr* file_path, const Cstr* chunk_name);
  void Set(const Cstr* path);
  ResourcePath& Combine(const Cstr* file_path, const Cstr* chunk_name);
  // Get the full resource path
  const Cstr* Get() const;
  // Get the chunk name only, not the file name. e.g. c:\temp\file_path.pak#model.mod -> model.mod
  const Cstr* GetChunkName() const {
    return chunk_name_.GetConst();
  }
  // Get the file path only, not the chunk name. e.g. c:\temp\file_path.pak#model.mod -> c:\temp\file_path.pak
  const Cstr* GetFilePath() const {
    return file_path_.GetConst();
  }
  ResourcePathType GetResourcePathType() const;
  bool IsEmpty() const;
  bool operator<(const ResourcePath& rhs) const;
  bool operator==(const ResourcePath& rhs) const;
  void ReadFrom(Stream* stream);
  void WriteTo(Stream* out_stream);

private:
  String file_path_;
  String chunk_name_;
  String resource_path_;

  static void ExtractFileChunkFromPath(
      const String& resource_path, String& outFilepath, String& outChunkName);
};

} // namespace dg
