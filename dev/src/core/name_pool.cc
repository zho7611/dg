// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "core_first.h"

namespace dg {

void NamePool::NameChunk::ReadFrom(Stream* stream) {
  StreamReadFlatMultiple(stream, data_, kMaxNameLength+1);
}

void NamePool::NameChunk::WriteTo(Stream* out_stream) {
  StreamWriteFlatMultiple(out_stream, data_, kMaxNameLength+1);
}

NamePool::NamePool() :
  chunk_pool_(kMaxChunkCount),
  num_free_indices_(0) {}

NamePool::~NamePool() {}

// Returns a free index
int NamePool::PopFreeIndex() {
  if (free_pool_.IsEmpty()) {
    return num_free_indices_++;
  }
  return free_pool_.PopFront();
}

// Returns the newly created index
int NamePool::CreateName(const Cstr* cstr) {
  if (cstr == 0) {
    return kInvalidIndex;
  }
  if (!IsValidData(cstr)) {
    return kInvalidIndex;
  }
  int newIdx = PopFreeIndex();
  NameChunk& curChunk = chunk_pool_.Get(newIdx);
  Check(string_util::GetLength(cstr) <= kMaxNameLength);
  MyStrCpy(curChunk.data_, cstr);
  return newIdx;
}

// You don't need to call this when you finished using the Name
// This method is for Unregister the name because it is not needed to be
// Cached
void NamePool::DeleteName(int index) {
  Check(IsValidIndex(index));
  free_pool_.PushBack(index);
}

Cstr* NamePool::GetData(int index) {
  return (!IsValidIndex(index)) ? NULL : chunk_pool_.Get(index).data_;
}

bool NamePool::IsValidData(const Cstr* cstr) const {
  if (cstr == 0) {
    return false;
  }
  int length = string_util::GetLength(cstr);
  if ((length == 0) || (length > kMaxNameLength)) {
    return false;
  }
  return true;
}

int NamePool::FindNameSlow(const Cstr* cstr) {
  // Couldn't use chunk_pool_.FindSlow() because of the T type mismatch
  const int kCount = chunk_pool_.Count();
  for (int idx = 0; idx < kCount; ++idx) {
    if (string_util::IsEqual(chunk_pool_.Get(idx).data_, cstr)) {
      return idx;
    }
  }
  return kInvalidIndex;
}

void NamePool::ReadFrom(Stream* stream) {
  StreamReadFlat(stream, num_free_indices_);
  StreamReadClass(stream, chunk_pool_);
  // No need to read the free_pool_ as it should be empty when loading
}

void NamePool::WriteTo(Stream* out_stream) {
  StreamWriteFlat(out_stream, num_free_indices_);
  StreamWriteClass(out_stream, chunk_pool_);
  // No need to write the free_pool_ as it should be empty when it is to be loaded
}

} // namespace dg
