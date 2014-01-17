// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class Chunk;

// Blob policy that does not delete its data when disposing
class NoDeleteBlobPolicy {
public:
  static void Dispose(uint8_t* data) {
    DG_UNREF(data);
  }
};

// Blob policy that automatically deletes its data when disposing
class AutoDeleteBlobPolicy {
public:
  static void Dispose(uint8_t* data) {
    DG_SAFE_DELETE_ARRAY(data);
  }
};

// Binary large object
class BlobBase {
public:
  BlobBase();
  BlobBase(Chunk* chunk);
  virtual ~BlobBase();
  virtual void Clear();
  int size() const {
    return size_;
  }
  uint8_t* data() {
    return data_;
  }
  const uint8_t* data_const() const {
    return data_;
  }
  void SetDataZero();
  // Set data pointer as the input data. It doesn't copy.
  // It just sets the data from the input data.
  void SetData(uint8_t* data, int size);
  void SetData(BlobBase& blob);
  void CopyFrom(const uint8_t* data, int size);
  void CopyFrom(const BlobBase& blob);
  void CopyFrom(Stream* stream);
  void CopyFrom(Chunk& chunk);
  void TakeFromChunk(Chunk& chunk);
  void Reserve(int new_size);
  void AddSize(int size_to_add_in_bytes);
  void AddData(const uint8_t* data, int size);
  void ReadFrom(Stream* stream);
  void WriteTo(Stream* out_stream);
  bool IsEqual(const BlobBase& blob) const;
  bool IsEmpty() const;
  bool operator ==(const BlobBase& blob) const;

protected:
  int size_;
  uint8_t* data_;
};

// Binary large object template
template<typename Policy = AutoDeleteBlobPolicy>
class BlobT : public BlobBase {
public:
  BlobT() : BlobBase() {}
  BlobT(Chunk* chunk) : BlobBase(chunk) {}
  virtual ~BlobT() {
    Clear();
  }
  virtual void Clear() {
    Policy::Dispose(data_);
    size_ = 0;
    data_ = NULL;
  }
};

typedef BlobT<AutoDeleteBlobPolicy> AutoDeleteBlob;
typedef BlobT<NoDeleteBlobPolicy> NoDeleteBlob;

inline void BlobBase::SetDataZero() {
  data_ = NULL;
  size_ = 0;
}

inline void BlobBase::SetData(uint8_t* in_data, int size) {
  Check(in_data);
  data_ = in_data;
  size_ = size;
}

inline void BlobBase::SetData(BlobBase& blob) {
  data_ = blob.data_;
  Check(data_);
  size_ = blob.size_;
}

inline bool BlobBase::operator ==(const BlobBase& blob) const {
  return IsEqual(blob);
}

inline bool BlobBase::IsEmpty() const {
  return (size_ == 0);
}

} // namespace dg
