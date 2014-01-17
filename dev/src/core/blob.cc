// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "core_first.h"

namespace dg {

BlobBase::BlobBase() :
  size_(0),
  data_(0) {}

BlobBase::BlobBase(Chunk* chunk) {
  Check(chunk);
  size_ = chunk->size();
  data_ = chunk->data();
}

BlobBase::~BlobBase() {
  Check(size_ == 0);
  Check(data_ == NULL);
}

void BlobBase::Clear() {
  size_ = 0;
  data_ = NULL;
}

void BlobBase::CopyFrom(const uint8_t * data, int size) {
  if (size == 0) {
    Check(data == NULL);
  } else {
    if (size_ != size) {
      Clear();
      data_ = new uint8_t[size];
      size_ = size;
    }
    MyMemCpy(data_, data, size);
  }
}

void BlobBase::CopyFrom(const BlobBase& blob) {
  CopyFrom(blob.data_, blob.size_);
}

void BlobBase::CopyFrom(Stream* stream) {
  int num_read = 1024;
  uint8_t buff[1024];
  while (num_read = stream->ReadBytes(buff, num_read)) {
    AddData(buff, num_read);
  }
}

void BlobBase::Reserve(int new_size) {
  if (new_size <= size_) {
    size_ = new_size;
  } else {
    Clear();
    data_ = new uint8_t[new_size];
    size_ = new_size;
  }
}

bool BlobBase::IsEqual(const BlobBase& blob) const {
  if (data_ == blob.data_) {
    return true;
  }
  if (size_ != blob.size_) {
    return false;
  }
  return (0 == ::memcmp(data_, blob.data_, size_));
}

void BlobBase::ReadFrom(Stream* stream) {
  Check(IsEmpty());
  int size = 0;
  StreamReadFlat(stream, size);
  Reserve(size);
  StreamReadFlatMultiple(stream, data_, size);
}

void BlobBase::WriteTo(Stream* out_stream) {
  Check(size_ > 0);
  StreamWriteFlat(out_stream, size_);
  StreamWriteFlatMultiple(out_stream, data_, size_);
}

void BlobBase::AddData(const uint8_t* data, int size) {
  int old_size = size_;
  AddSize(size);
  MyMemCpy(data_ + old_size, data, size);
}

void BlobBase::AddSize(int size_to_add_in_bytes) {
  Check(size_to_add_in_bytes >= 0);
  if (size_to_add_in_bytes <= 0) {
    return;
  }
  int new_size = size_ + size_to_add_in_bytes;
  uint8_t* new_data = new uint8_t[new_size];
  MyMemCpy(new_data, data_, size_);
  Clear();
  SetData(new_data, new_size);
}

void BlobBase::CopyFrom(Chunk& chunk) {
  CopyFrom(chunk.data(), chunk.size());
}

void BlobBase::TakeFromChunk(Chunk& chunk) {
  Clear();
  data_ = chunk.data();
  size_ = chunk.size();
  chunk.SetDataZero();
}

} // namespace dg
