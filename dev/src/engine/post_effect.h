// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class PostEffect : public Resource {
public:
  PostEffect();
  virtual ~PostEffect();
  bool HasDepthSurface();
  virtual void Init(const Size2& size);
  virtual void Clear();
  virtual void SetSize(const Size2& size) DG_PURE;
  virtual void PreRender() DG_PURE;
  virtual void PostRender() DG_PURE;
  virtual RenderTarget* render_target() {
    DG_UNREACHABLE();
    return 0;
  }
  virtual bool SerializeToDevice() {
    return true;
  }
  virtual bool SerializeFromDevice() {
    return true;
  }
  virtual void ReadFrom(Stream* stream) {
    DG_UNREACHABLE();
  }
  virtual void WriteTo(Stream* out_stream) {
    DG_UNREACHABLE();
  }

  bool is_enabled_;

protected:
  Size2 size_;
};

} // namespace dg
