// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class Stream {
public:
  // Returns actual read bytes
  virtual int ReadBytes(uint8_t* out_buffer, int size_bytes) = 0;
  // Returns actual write bytes
  virtual int WriteBytes(const uint8_t* buffer, int size_bytes) = 0;
};

class MemoryStream : public Stream {
public:
  MemoryStream() {
    data_ = NULL;
    position_ = 0;
  }

  MemoryStream(uint8_t* data) {
    data_ = data;
    position_ = 0;
  }

  virtual int ReadBytes(uint8_t* out_data, int size_bytes) {
    Check(out_data);
    if (out_data == 0) return 0;
    MyMemCpy(out_data, data_ + position_, size_bytes);
    position_ += size_bytes;
    return size_bytes;
  }

  virtual int WriteBytes(const uint8_t* data, int size_bytes) {
    Check(data);
    if (data == 0) return 0;
    MyMemCpy(data_ + position_, data, size_bytes);
    position_ += size_bytes;
    return size_bytes;
  }

  uint8_t* GetCurrentData() {
    return data_ + position_;
  }

private:
  uint8_t* data_;
  int position_;
};

} // namespace dg
