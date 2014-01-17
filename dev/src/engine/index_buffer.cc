// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

IndexBuffer::IndexBuffer(void) :
  size_in_bytes_(0),
  num_indices_(0),
  usage_(GpuUsage_None),
  pool_(GpuPool_Managed) {}

IndexBuffer::~IndexBuffer(void) {
}

bool IndexBuffer::InitIndexBuffer(int num_indices, GpuUsage usage, GpuPool pool) {
  num_indices_ = num_indices;
  size_in_bytes_ = num_indices_*stride();
  usage_ = usage;
  pool_ = pool;
  blob_.Reserve(size_in_bytes_);
  OnReadDone();
  return true;
}

void IndexBuffer::ReadFrom(Stream* stream) {
  StreamReadFlat(stream, num_indices_);
  size_in_bytes_ = num_indices_*stride();
  usage_ = GpuUsage_WriteOnly;
  pool_ = GpuPool_Managed;
  StreamReadClass(stream, blob_);
  Check(!blob_.IsEmpty());
  OnReadDone();
}

void IndexBuffer::WriteTo(Stream* out_stream) {
  Check(num_indices_);
  Check(!blob_.IsEmpty());
  StreamWriteFlat(out_stream, num_indices_);
  StreamWriteClass(out_stream, blob_);
  OnWriteDone();
}

bool IndexBuffer::SerializeToDevice() {
  Check(blob_.size() == size_in_bytes_);
  if (blob_.IsEmpty()) {
    return false;
  }
  uint8_t* buff = NULL;
  if (RC_LockIndexBuffer(device_index_buffer_handle_, 0, size_in_bytes_, &buff, GpuLockType_None)) {
    MyMemCpy(buff, blob_.data(), size_in_bytes_);
    RC_UnlockIndexBuffer(device_index_buffer_handle_);
    return true;
  }
  return false;
}

bool IndexBuffer::SerializeFromDevice() {
  IndexType* buff = NULL;
  if (!blob_.IsEmpty()) {
    // No need to update because it's already filled
    return true;
  }
  if (Lock(0, size_in_bytes_, buff, GpuLockType_ReadOnly)) {
    blob_.CopyFrom((const uint8_t*)buff, size_in_bytes_);
    Unlock();
    Check(!blob_.IsEmpty());
    return true;
  }
  return false;
}

void IndexBuffer::OnRelease() {
  UnbindResource();
}

bool IndexBuffer::OnInitDevice() {
  if (device_index_buffer_handle_) {
    return false;
  }
  Check(size_in_bytes_);
  RC_CreateIndexBuffer(size_in_bytes_, usage_, pool_, device_index_buffer_handle_);
  if (device_index_buffer_handle_) {
    SerializeToDevice();
  }
  return device_index_buffer_handle_;
}

bool IndexBuffer::OnClearDevice() {
  if (!device_index_buffer_handle_) {
    return false;
  }
  device_index_buffer_handle_.Release();
  return true;
}

void IndexBuffer::SubmitIndexBuffer() {
  BindResource();
  RC_SetIndices(device_index_buffer_handle_);
}

bool IndexBuffer::Lock(int offset, int size, IndexType*& out_buffer, GpuLockType lock_flag) {
  if (!device_index_buffer_handle_) {
    BindResource();
  }
  RC_LockIndexBuffer(device_index_buffer_handle_, offset, size, reinterpret_cast<uint8_t**>(&out_buffer), lock_flag);
  return (out_buffer != 0);
}

void IndexBuffer::Unlock() {
  if (device_index_buffer_handle_) {
    RC_UnlockIndexBuffer(device_index_buffer_handle_);
  }
}

} // namespace dg
