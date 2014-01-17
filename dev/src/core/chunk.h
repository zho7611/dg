// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {
class Package;

// Reference-counted memory buffer
class Chunk : public RefCount, public Stream {
public:
  Chunk();
  virtual ~Chunk();
  virtual int position() const {
    return position_;
  }
  virtual void set_position(int position) {
    Check(position >= 0);
    Check(position <= size_);
    position_ = position;
  }
  void CopyFrom(Chunk& stream);
  // Reserve the internal blob buffer with size and
  // Copy the content from the data.
  void CopyFrom(uint8_t* data, int size);
  // Actually perform the read or write operation
  virtual void Flush() {}
  // Clear internal blob buffer
  void SetDataZero();
  // Not copy but just set the buffer pointer and size
  void SetData(uint8_t* data, int size);
  void SetData(Chunk& chunk);
  uint8_t* data() {
    return alloc_.data();
  }
  const uint8_t* data_const() const {
    return alloc_.data_const();
  }
  int size() const {
    return size_;
  }
  bool IsEmpty() const;
  bool IsEnd() const;
  void Clear();
  void ReadFrom(Stream* stream);
  void WriteTo(Stream* out_stream);
  int ReadBytes(uint8_t* out_buffer, int size);
  int WriteBytes(const uint8_t* buffer, int size);

private:
  int position_;
  int size_;
  AutoDeleteBlob alloc_;

  static const int kInitAllocSizeByte = 4096;

  void FreeData();
};

inline bool Chunk::IsEnd() const {
  return (position_ == size_);
}

inline bool Chunk::IsEmpty() const {
  return (size_ <= 0);
}

inline void Chunk::SetDataZero() {
  alloc_.SetDataZero();
  position_ = 0;
  size_ = 0;
}

} // namespace dg
