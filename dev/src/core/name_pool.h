// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {
class Chunk;

class NamePool {
public:
  static const int kMaxNameLength = 127;
  static const int kInvalidIndex = -1;

  NamePool();
  ~NamePool();
  int CreateName(const Cstr* cstr);
  void DeleteName(int index);
  Cstr* GetData(int index);
  int FindNameSlow(const Cstr* cstr);
  void ReadFrom(Stream* stream);
  void WriteTo(Stream* out_stream);

private:
  class NameChunk {
  public:
    Cstr data_[kMaxNameLength+1];

    NameChunk() {
      MyMemSet(data_, 0, sizeof(data_));
    }
    ~NameChunk() { }
    void ReadFrom(Stream* stream);
    void WriteTo(Stream* out_stream);
  };

  typedef FixedArray<NameChunk> ChunkPoolType;
  typedef List<int> FreeIndexPoolType;

  ChunkPoolType chunk_pool_;
  FreeIndexPoolType free_pool_;
  int num_free_indices_;

  static const int kMaxChunkCount = 1024;

  int PopFreeIndex();
  bool IsValidIndex(int index) const {
    return (index >= 0) && (index < kMaxChunkCount);
  }
  bool IsValidData(const Cstr* cstr) const;
};

}
