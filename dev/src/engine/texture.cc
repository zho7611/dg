// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

Texture::Texture() :
  usage_(GpuUsage_None),
  pool_(GpuPool_Managed),
  format_(GpuFormat_Unknown),
  mip_levels_(0) {
}

Texture::~Texture() {
}

bool Texture::CopyFrom(BlobBase* blob) {
  if (blob) {
    Check(blob_.IsEmpty());
    blob_.CopyFrom(*blob);
  }
  return true;
}

void Texture::ReadFrom(Stream* stream) {
  Chunk* chunk = static_cast<Chunk*>(stream);
  blob_.TakeFromChunk(*chunk);
  OnReadDone();
}

void Texture::WriteTo(Stream* out_stream) {
  StreamWriteFlatMultiple(out_stream, blob_.data(), blob_.size());
  OnWriteDone();
}

bool Texture::SerializeFromDevice() {
  ResourcePath path = GetResourcePath();
  int path_length = string_util::GetLength(path.Get());
  const Cstr* extension = path.Get() + path_length - 3;
  RC_TextureToBlob(resource_name(), device_texture_handle_, extension, &blob_);
  return Super::SerializeFromDevice();
}

bool Texture::SerializeToDevice() {
  // Blob to device after locking
  DG_UNIMPLEMENTED();
  return Super::SerializeToDevice();
}

bool Texture::OnInitDevice() {
  Check(!device_texture_handle_);
  if ((usage_ == GpuUsage_Dynamic) || (usage_ == GpuUsage_RenderTarget) || blob_.IsEmpty()) {
    if (size_.IsZero()) return false;
    RC_CreateTexture2D(resource_name(), device_texture_handle_, size_.x, size_.y, mip_levels_, usage_, format_, pool_);
  } else {
    RC_CreateTexture2DFromBlob(resource_name(), device_texture_handle_, &blob_, usage_, pool_, size_.x, size_.y, mip_levels_, format_);
    Check(size_.x > 0);
    Check(size_.y > 0);
  }
  Check(device_texture_handle_);
  return (device_texture_handle_) ? true : false;
}

bool Texture::OnClearDevice() {
  device_texture_handle_.Release();
  return true;
}

void Texture::SubmitTexture(int stage) {
  RC_SetTexture(resource_name(), stage, device_texture_handle_);
}

bool Texture::Init(
    GpuUsage usage,
    Size2 size,
    GpuFormat format,
    int num_mip_levels) {
  Check(!device_texture_handle_);
  size_ = size;
  size_.x = size.x;
  size_.y = size.y;
  if ((usage == GpuUsage_DepthStencil) ||
      (usage == GpuUsage_RenderTarget) ||
      (usage == GpuUsage_Dynamic)) {
    pool_ = GpuPool_Default;
  } else {
    pool_ = GpuPool_Managed;
  }
  format_ = format;
  usage_ = usage;
  mip_levels_ = num_mip_levels;
  OnReadDone();
  return true;
}

bool Texture::Clear() {
  if (device_texture_handle_) {
    UnbindResource();
  }
  size_ = Size2::kZero;
  usage_ = GpuUsage_None;
  pool_ = GpuPool_Managed;
  format_ = GpuFormat_Unknown;
  return true;
}

void Texture::LockTexture(
    uint8_t*& out_buffer,
    int& out_pitch,
    GpuLockType lock_flag) {
  BindResource();
  Check(device_texture_handle_);
  const int level = 0;
  RC_LockTexture(device_texture_handle_, level, out_buffer, out_pitch, lock_flag);
}

void Texture::UnlockTexture() {
  const int level = 0;
  RC_UnlockTexture(device_texture_handle_, level);
}

Texture::ScopedLock::ScopedLock(
    Texture& texture,
    uint8_t*& out_buffer,
    int& out_pitch,
    GpuLockType lock_flag)
  : texture_(texture) {
  texture_.LockTexture(out_buffer, out_pitch, lock_flag);
}

Texture::ScopedLock::~ScopedLock() {
  texture_.UnlockTexture();
}

void texture_util::RawTextureHeader::CopyToBlob(BlobBase* blob, uint8_t* data, int data_size) {
  blob->Clear();
  Check(this->storage_format == GpuFormat_A8B8G8R8);
  blob->Reserve(sizeof(RawTextureHeader) + this->width*4*this->height);
  MemoryStream stream(blob->data());
  stream.WriteBytes((uint8_t*)this, sizeof(RawTextureHeader));
  stream.WriteBytes(data, data_size);
}

void texture_util::InitSolidColorTexture(Texture* tex, int width, int height, ColorInt color) {
  tex->Init(GpuUsage_None, Size2(width, height), GpuFormat_A8R8G8B8);
  uint8_t* buffer = NULL;
  int pitch = 0;
  {
    Texture::ScopedLock locker(*tex, buffer, pitch);
    Check(buffer);
    for (int y = 0; y < height; ++y) {
      for (int x = 0; x < width; ++x) {
        ColorInt* texel = (ColorInt*)(buffer + pitch*y + x*sizeof(ColorInt));
        *texel = color;
      }
    }
  }
  tex->OnReadDone();
}

} // namespace dg
