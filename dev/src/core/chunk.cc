// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "core_first.h"

namespace dg {

Chunk::Chunk() :
  position_(0),
  size_(0) {
}

Chunk::~Chunk() {
  Clear();
}

void Chunk::CopyFrom(uint8_t* data, int size) {
  CheckWarn(data);
  CheckWarn(size);
  Clear();
  position_ = 0;
  size_ = size;
  alloc_.CopyFrom(data, size);
}

void Chunk::SetData(uint8_t* data, int size) {
  Check(data);
  Check(size);
  position_ = 0;
  size_ = size;
  alloc_.SetData(data, size);
}

void Chunk::SetData(Chunk& chunk) {
  position_ = 0;
  size_ = chunk.size_;
  alloc_.SetData(chunk.alloc_);
  Check(size_);
  Check(alloc_.data());
}

void Chunk::ReadFrom(Stream* stream) {
  int size;
  StreamReadFlat(stream, size);
  alloc_.AddSize(size);
  StreamReadFlatMultiple(stream, data(), size);
  size_ = size;
}

void Chunk::WriteTo(Stream* out_stream) {
  StreamWriteFlat(out_stream, size_);
  StreamWriteFlatMultiple(out_stream, data(), size_);
}

void Chunk::CopyFrom(Chunk& chunk) {
  CopyFrom(chunk.data(), chunk.size());
}

void Chunk::Clear() {
  alloc_.Clear();
  position_ = 0;
  size_ = 0;
}

int Chunk::ReadBytes(uint8_t* out_buffer, int size) {
  int size_remaining = size_ - position_;
  Check(size_remaining >= 0);
  int read_size = MyMin(size_remaining, size);
  MyMemCpy(out_buffer, alloc_.data() + position_, read_size);
  position_ += read_size;
  return read_size;
}

int Chunk::WriteBytes(const uint8_t* buffer, int size) {
  int alloc_size = alloc_.size();
  int alloc_remaining = alloc_size - position_;
  if (alloc_size == 0) {
    // The initial alloc size is the exact size
    alloc_.AddSize(size);
  } else if (alloc_remaining < size) {
    int num_alloc = (size - alloc_remaining) / kInitAllocSizeByte + 1;
    const int kLinearIncrement = 2;
    int kSizeToAdded = kLinearIncrement * kInitAllocSizeByte * num_alloc;
    Check((kSizeToAdded + alloc_remaining) >= size);
    alloc_.AddSize(kSizeToAdded);
  }
  MyMemCpy(alloc_.data() + position_, buffer, size);
  position_ += size;
  size_ += size;
  return size;
}

} // namespace dg
