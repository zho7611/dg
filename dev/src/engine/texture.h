// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class Texture : public RenderResource {
public:
  class ScopedLock;

  Texture();
  virtual ~Texture();
  void SubmitTexture(int stage);
  bool Init(
      GpuUsage usage,
      Size2 size,
      GpuFormat format,
      int num_mip_levels = 0 // 0 = all
      );
  bool Clear();
  const Size2& size() const {
    return size_;
  }
  int mip_levels() const {
    return mip_levels_;
  }
  GpuFormat format() const {
    return format_;
  }
  bool CopyFrom(BlobBase* blob);
  bool HasBlob() { return !blob_.IsEmpty(); }
  virtual bool SerializeToDevice();
  virtual bool SerializeFromDevice();
  virtual bool OnInitDevice();
  virtual bool OnClearDevice();
  virtual void ReadFrom(Stream* stream);
  virtual void WriteTo(Stream* out_stream);
  void LockTexture(
      uint8_t*& out_buffer,
      int& out_pitch,
      GpuLockType lock_flag = GpuLockType_None);
  void UnlockTexture();
  TextureHandle& device_texture_handle() {
    return device_texture_handle_;
  }

protected:
  TextureHandle device_texture_handle_;

  Size2 size_;
  // Number of mip levels. 0 = all mip levels
  int mip_levels_;
  GpuUsage usage_;
  GpuPool pool_;
  GpuFormat format_;
  // Source image blob, not the raw buffer
  AutoDeleteBlob blob_;

  DG_DECLARE_RESOURCE(Texture, Resource);
};

class Texture::ScopedLock {
public:
  ScopedLock(Texture& texture, uint8_t*& out_buffer, int& out_pitch,
      GpuLockType lock_flag = GpuLockType_None);
  ~ScopedLock();

private:
  Texture& texture_;
};

namespace texture_util {

struct RawTextureHeader {
  char signature[4]; // "DGTX"
  int version; // 0 for now
  int width;
  int height;
  GpuFormat storage_format; // For now, only GpuFormat_A8B8G8R8
  int num_mips;

  void Init(int width, int height) {
    this->width = width;
    this->height = height;
    this->num_mips = 1;
    this->storage_format = GpuFormat_A8B8G8R8;
    this->version = 0;
    MyMemCpy(&this->signature, GetSignature(), 4);
  }

  static const char* GetSignature() {
    return "DGTX";
  }

  void CopyToBlob(BlobBase* blob, uint8_t* data, int data_size);
};

void InitSolidColorTexture(Texture* tex, int width, int height, ColorInt color);

} // namespace texture_util

} // namespace dg
