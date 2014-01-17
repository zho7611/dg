// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class Texture;

class RenderTarget : public RenderResource {
public:
  Ptr<Texture> texture_;

  RenderTarget();
  virtual ~RenderTarget();
  void SetCurrent();
  void Init();
  void Init(
      const Size2& size_in_pixel,
      GpuFormat color_format,
      GpuFormat depth_format);
  void Clear();
  int width() const { return size_.x; }
  int height() const { return size_.y; }
  const Size2& size() const { return size_; }
  void set_size(const Size2& size);
  bool HasDepthSurface() const;
  void ReadFrom(Stream* stream) {
    DG_UNREF(stream);
    DG_UNIMPLEMENTED();
  }
  void WriteTo(Stream* out_stream);
  bool OnInitDevice();
  bool OnClearDevice();
  bool SerializeToDevice() {
    return true;
  }
  bool SerializeFromDevice() {
    return true;
  }

protected:
  bool IsForRenderer() const;

  Size2 size_;
  GpuFormat color_format_;
  GpuFormat depth_format_;

protected:
  bool CreateSurfaces();
  bool CreateSurfacesForRenderer();

  RenderTargetHandle device_render_target_handle_;
  SurfaceHandle device_depth_surface_handle_;
};

} // namespace dg
