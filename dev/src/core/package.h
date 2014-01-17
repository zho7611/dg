// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class Package : public Resource {
public:
  enum OpenChunkOption {
    kRead = 0,
    kWrite = 1,
  };

  // It should keep the orders of chunks
  typedef List<Pair<String, Ptr<Chunk> > > Chunks;
  typedef Chunks::Iterator ChunksIter;

  Chunks chunks_;

  Package();
  bool IsDirty() {
    return resource_state_.IsDirty();
  }
  void SetDirty(bool value) {
    resource_state_.SetDirty(value);
  }
  void RemoveChunk(const Cstr* chunk_name);
  void ReadFrom(Stream* stream);
  void WriteTo(Stream* out_stream);
  Chunk* OpenChunk(const Cstr* chunk_name, OpenChunkOption option);
  void CloseChunk(Chunk* chunk);
  void Clear();

  DG_DECLARE_RESOURCE(Package, Resource);
};

} // namespace dg
