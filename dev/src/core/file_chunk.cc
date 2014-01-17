// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "core_first.h"

namespace dg {

Chunk* FileChunk::CreateReadChunk(const Cstr* path) {
  Chunk* chunk = NULL;
  if (File::IsFound(path)) {
    File file(path, File::kRead);
    if (file.IsOpened()) {
      file.PrepareBuffer();
      chunk = new Chunk();
      chunk->SetData(file.data(), file.size());
      file.SetDataZero();
    }
  } else {
    DG_LOG_LINE(TXT("filechunk.createreadchunk.error: file-not-found:%s"), path);
  }
  return chunk;
}

class FileWriteChunk : public Chunk {
public:
  ~FileWriteChunk() {
    FileWriteChunk::Flush();
    Clear();
  }
  virtual void Flush() {
    File file(file_path_.Get(), File::kWrite);
    file.StreamWriteFlatMultiple(data(), size());
  }
  String file_path_;
};

Chunk* FileChunk::CreateWriteChunk(const Cstr* path) {
  FileWriteChunk* chunk = new FileWriteChunk();
  chunk->file_path_ = path;
  return chunk;
}

} // namespace dg
