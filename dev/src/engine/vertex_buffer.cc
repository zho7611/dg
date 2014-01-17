// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

VertexBuffer::VertexBuffer(void) :
  pool_(GpuPool_Managed),
  usage_(GpuUsage_None),
  num_vertices_(0) {
}

VertexBuffer::~VertexBuffer() {
  vertex_decl_.UnbindResource();
}

bool VertexBuffer::InitVertexBufferWithDecl(
    VertexDecl* decl,
    int num_vertices,
    GpuUsage usage,
    GpuPool pool) {
  num_vertices_ = num_vertices;
  vertex_decl_.CopyFrom(decl);
  pool_ = pool;
  usage_ = usage;
  blob_.Reserve(num_vertices*stride());
  OnReadDone();
  return true;
}

bool VertexBuffer::InitVertexBuffer(
    VertexDecl::Preset type,
    int num_vertices,
    GpuUsage usage,
    GpuPool pool) {
  num_vertices_ = num_vertices;
  vertex_decl_.InitByPreset(type);
  pool_ = pool;
  usage_ = usage;
  blob_.Reserve(num_vertices*stride());
  OnReadDone();
  return true;
}

bool VertexBuffer::OnInitDevice() {
  if (device_vertex_buffer_handle_) {
    return false;
  }
  Check(!device_vertex_buffer_handle_);
  int size_bytes = GetBufferSizeInBytes();
  Check(size_bytes > 0);
  RC_CreateVertexBuffer(size_bytes, usage_, pool_, device_vertex_buffer_handle_);
  if (device_vertex_buffer_handle_) {
    SerializeToDevice();
  }
  return true;
}

bool VertexBuffer::OnClearDevice() {
  if (!device_vertex_buffer_handle_) {
    return false;
  }
  device_vertex_buffer_handle_.Release();
  return true;
}

void VertexBuffer::SubmitVertexBuffer() {
  BindResource();
  Check(device_vertex_buffer_handle_);
  RC_SetStreamSource(0, device_vertex_buffer_handle_, 0, vertex_decl_.stride());
  // RC_SetStreamSource() should be followed by SubmitVertexDecl() call in OpenGL
  vertex_decl_.SubmitVertexDecl();
}

bool VertexBuffer::Lock(
    int offset,
    int size,
    uint8_t*& out_buffer,
    GpuLockType lock_flag) {
  if (!device_vertex_buffer_handle_) {
    BindResource();
  }
  RC_LockVertexBuffer(device_vertex_buffer_handle_, offset, size, (uint8_t**)&out_buffer, lock_flag);
  return (out_buffer != 0);
}

void VertexBuffer::Unlock() {
  Check(device_vertex_buffer_handle_);
  RC_UnlockVertexBuffer(device_vertex_buffer_handle_);
}

bool VertexBuffer::SerializeToDevice() {
  int buffer_size = GetBufferSizeInBytes();
  Check(buffer_size == blob_.size());
  if (blob_.IsEmpty()) {
    return false;
  }
  uint8_t* buff = NULL;
  if (RC_LockVertexBuffer(device_vertex_buffer_handle_, 0, buffer_size, &buff, GpuLockType_None)) {
    MyMemCpy(buff, blob_.data(), buffer_size);
    RC_UnlockVertexBuffer(device_vertex_buffer_handle_);
    return true;
  }
  return false;
}

bool VertexBuffer::SerializeFromDevice() {
  int buffer_size = GetBufferSizeInBytes();
  uint8_t* buffer = NULL;
  if (!blob_.IsEmpty()) {
    // No need to update. It's already there
    // Index buffer is not updated only in GPU
    return true;
  }
  if (Lock(0, buffer_size, buffer, GpuLockType_ReadOnly)) {
    blob_.CopyFrom(buffer, buffer_size);
    Unlock();
    Check(!blob_.IsEmpty());
    return true;
  }
  return false;
}

void VertexBuffer::ReadFrom(Stream* stream) {
  StreamReadClass(stream, vertex_decl_);
  StreamReadFlat(stream, num_vertices_);
  Check(num_vertices_);
  StreamReadFlat(stream, pool_);
  StreamReadFlat(stream, usage_);
  StreamReadClass(stream, blob_);
  Check(!blob_.IsEmpty());
  OnReadDone();
}

void VertexBuffer::WriteTo(Stream* out_stream) {
  StreamWriteClass(out_stream, vertex_decl_);
  Check(num_vertices_);
  StreamWriteFlat(out_stream, num_vertices_);
  StreamWriteFlat(out_stream, pool_);
  StreamWriteFlat(out_stream, usage_);
  Check(!blob_.IsEmpty());
  StreamWriteClass(out_stream, blob_);
  OnWriteDone();
}

} // namespace dg
