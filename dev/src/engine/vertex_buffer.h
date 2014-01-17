// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class VertexBuffer : public RenderResource {
public:
  friend class MeshBuilder;
  class ScopedLock;

  VertexBufferHandle device_vertex_buffer_handle_;

  VertexBuffer();
  ~VertexBuffer();
  bool InitVertexBufferWithDecl(
    VertexDecl* decl,
    int num_vertices,
    GpuUsage usage = GpuUsage_WriteOnly,
    GpuPool pool = GpuPool_Managed);
  bool InitVertexBuffer(
      VertexDecl::Preset type,
      int num_vertices,
      GpuUsage usage = GpuUsage_WriteOnly,
      GpuPool pool = GpuPool_Managed);
  void SubmitVertexBuffer();
  virtual bool OnInitDevice();
  virtual bool OnClearDevice();
  bool SerializeToDevice();
  bool SerializeFromDevice();
  void ReadFrom(Stream* stream);
  void WriteTo(Stream* out_stream);
  int num_vertices() const {
    return num_vertices_;
  }
  const BlobBase& blob() const {
    return blob_;
  }
  void SetBlob(uint8_t* data, int size_bytes) {
    blob_.SetData(data, size_bytes);
  }
  int stride() const {
    return vertex_decl_.stride();
  }
  VertexDecl::Preset GetDeclPreset() const {
    return vertex_decl_.GetPreset();
  }
  VertexDecl& vertex_decl() {
    return vertex_decl_;
  }
  bool Lock(
      int offset,
      int size,
      uint8_t*& out_buffer,
      GpuLockType lock_flag);
  void Unlock();

private:
  VertexDecl vertex_decl_;
  GpuUsage usage_;
  GpuPool pool_;
  int num_vertices_;
  AutoDeleteBlob blob_;

  int GetBufferSizeInBytes() const {
    return (num_vertices_*vertex_decl_.stride());
  }
};

// Lock via blob
class VertexBuffer::ScopedLock {
public:
  template<class T>
  ScopedLock(
      VertexBuffer& in_vertex_buffer, T*& out_buffer,
      GpuLockType lock_flag = GpuLockType_None)
    : vertex_buffer_(in_vertex_buffer) {
    Check(!vertex_buffer_.blob_.IsEmpty());
    size_ = vertex_buffer_.blob_.size();
    vertex_buffer_.Lock(0, size_, (uint8_t*&)out_buffer, lock_flag);
    data_ = (uint8_t*)out_buffer;
  }
  ~ScopedLock() {
    vertex_buffer_.blob_.CopyFrom(data_, size_);
    vertex_buffer_.Unlock();
  };

private:
  VertexBuffer& vertex_buffer_;
  uint8_t* data_;
  unsigned int size_;
};

} // namespace dg
