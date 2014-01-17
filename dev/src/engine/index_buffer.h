// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

#define DG_USE_32BIT_INDEX

class IndexBuffer : public RenderResource {
public:
#if defined(DG_USE_32BIT_INDEX)
  typedef unsigned int IndexType;
#else
  typedef unsigned short IndexType;
#endif

  class ScopedLock;
  friend class MeshBuilder;

  IndexBufferHandle device_index_buffer_handle_;
  IndexBuffer();
  ~IndexBuffer();
  bool InitIndexBuffer(
      int num_indices,
      GpuUsage usage = GpuUsage_WriteOnly,
      GpuPool pool = GpuPool_Managed);
  void SubmitIndexBuffer();
  void ReadFrom(Stream* stream);
  void WriteTo(Stream* out_stream);
  void OnRelease();
  bool OnInitDevice();
  bool SerializeToDevice();
  bool SerializeFromDevice();
  bool OnClearDevice();
  BlobBase& blob() {
    return blob_;
  }
  const BlobBase& blob_const() const {
    return blob_;
  }
  void SetBlob(uint8_t* data, int size_bytes) {
    blob_.SetData(data, size_bytes);
  }
  int stride() const {
    return sizeof(IndexType);
  }
  int num_indices() const {
    return num_indices_;
  }

private:
  int num_indices_;
  unsigned int size_in_bytes_;
  GpuUsage usage_;
  GpuPool pool_;
  AutoDeleteBlob blob_;

  bool Lock(int offfset, int size, IndexType*& out_buffer, GpuLockType lock_flag);
  void Unlock();
};

// Lock via blob
class IndexBuffer::ScopedLock {
public:
  ScopedLock(
      IndexBuffer& index_buffer, IndexType*& out_buffer,
      GpuLockType lock_flag = GpuLockType_None) :
    index_buffer_(index_buffer) {
    size_ = index_buffer_.blob_.size();
    offset_ = 0;
    index_buffer_.Lock(offset_, size_, out_buffer, lock_flag);
    data_ = out_buffer;
  }
  ~ScopedLock() {
    index_buffer_.blob_.CopyFrom((uint8_t*)data_, size_);
    index_buffer_.Unlock();
  }

private:
  IndexBuffer& index_buffer_;
  IndexType* data_;
  int offset_;
  int size_;
};

} // namespace dg
