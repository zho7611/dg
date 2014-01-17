// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

#if defined(DG_RENDERER_DX9)

// Do not copy the device resource pointer. It will be released more than once.
template<class GpuResourceType>
class GpuResourceHandleDx9 {
public:
  GpuResourceHandleDx9() : handle_(0) {}
  GpuResourceHandleDx9(void* rhs) {
    handle_ = (GpuResourceType*)rhs;
  }
  ~GpuResourceHandleDx9() {
    DG_SAFE_RELEASE(handle_);
  }
  void Release() {
    DG_SAFE_RELEASE(handle_);
  }
  GpuResourceType** GetRef() {
    return reinterpret_cast<GpuResourceType**>(&handle_);
  }
  GpuResourceType* Get() const {
    return reinterpret_cast<GpuResourceType*>(handle_);
  }
  GpuResourceType* operator->() const {
    Check(handle_);
    return Get();
  }
  operator bool() const {
    return handle_ != NULL;
  }
  bool operator==(const GpuResourceHandleDx9<GpuResourceType>& rhs) const {
    return handle_ == rhs.handle_;
  }
  bool operator!=(const GpuResourceHandleDx9<GpuResourceType>& rhs) const {
    return handle_ != rhs.handle_;
  }
  operator GpuResourceType*() const {
    return reinterpret_cast<GpuResourceType*>(handle_);
  }

private:
  GpuResourceType* handle_;
};

class IDummyInterface {
public:
  IDummyInterface(void*) {}
  void Release() {}
};

typedef GpuResourceHandleDx9<IDirect3DTexture9> TextureHandle;
typedef GpuResourceHandleDx9<IDirect3DVertexShader9> VertexShaderHandle;
typedef GpuResourceHandleDx9<IDirect3DPixelShader9> PixelShaderHandle;
typedef GpuResourceHandleDx9<IDirect3DSurface9> SurfaceHandle;
typedef GpuResourceHandleDx9<IDirect3DStateBlock9> StateBlockHandle;
typedef GpuResourceHandleDx9<IDirect3DVertexBuffer9> VertexBufferHandle;
typedef GpuResourceHandleDx9<IDirect3DIndexBuffer9> IndexBufferHandle;
typedef GpuResourceHandleDx9<IDirect3DVertexDeclaration9> VertexLayoutHandle;
typedef GpuResourceHandleDx9<IDummyInterface> ShaderProgramHandle;
typedef GpuResourceHandleDx9<IDirect3DSurface9> RenderTargetHandle;

#elif defined(DG_RENDERER_GL2)

enum GpuResourceType {
  GpuResourceType_Default,
  GpuResourceType_VertexLayout,
  GpuResourceType_Buffer,
  GpuResourceType_Shader,
  GpuResourceType_ShaderProgram,
  GpuResourceType_RenderTarget,
  GpuResourceType_Texture,
  GpuResourceType_Surface,
};

// Forward declaration for ReleaseGpuResource
template<GpuResourceType T, typename HandleType>
class GpuResourceHandleGl;

template<GpuResourceType T, typename HandleType>
void ReleaseGpuResource(class GpuResourceHandleGl<T, HandleType>* resource_handle) {
  resource_handle->handle_ = NULL;
  DG_UNIMPLEMENTED();
}

template<GpuResourceType T, typename HandleType = unsigned int> // Unsigned int <= GLuint
class GpuResourceHandleGl {
public:
  GpuResourceHandleGl(HandleType handle = 0) : handle_(handle), custom_(0) {}
  void operator=(HandleType rhs) {
    handle_ = rhs;
    custom_ = NULL;
  }
  HandleType Get() {
    return handle_;
  }
  HandleType* GetRef() {
    return &handle_;
  }
  void Release() {
    ReleaseGpuResource<T, HandleType>(this);
  }
  bool operator==(const GpuResourceHandleGl<T, HandleType>& rhs) {
    return handle_ == rhs.handle_;
  }
  bool operator!=(const GpuResourceHandleGl<T, HandleType>& rhs) {
    return handle_ != rhs.handle_;
  }
  operator bool() const {
    return handle_ != NULL;
  }

  HandleType handle_;
  void* custom_;
};

typedef GpuResourceHandleGl<GpuResourceType_Texture> TextureHandle;
typedef GpuResourceHandleGl<GpuResourceType_Shader> VertexShaderHandle;
typedef GpuResourceHandleGl<GpuResourceType_Shader> PixelShaderHandle;
typedef GpuResourceHandleGl<GpuResourceType_Shader> DeviceShaderHandle;
typedef GpuResourceHandleGl<GpuResourceType_ShaderProgram> ShaderProgramHandle;
typedef GpuResourceHandleGl<GpuResourceType_RenderTarget> RenderTargetHandle;
typedef GpuResourceHandleGl<GpuResourceType_Surface> SurfaceHandle;
typedef GpuResourceHandleGl<GpuResourceType_Buffer> VertexBufferHandle;
typedef GpuResourceHandleGl<GpuResourceType_Buffer> IndexBufferHandle;
typedef GpuResourceHandleGl<GpuResourceType_VertexLayout, void*> VertexLayoutHandle;

#else

class GpuResourceType {
public:
  void Release() {}
};

typedef GpuResourceHandle<GpuResourceType> TextureHandle;
typedef GpuResourceHandle<GpuResourceType> VertexShaderHandle;
typedef GpuResourceHandle<GpuResourceType> PixelShaderHandle;
typedef GpuResourceHandle<GpuResourceType> SurfaceHandle;
typedef GpuResourceHandle<GpuResourceType> VertexBufferHandle;
typedef GpuResourceHandle<GpuResourceType> IndexBufferHandle;
typedef GpuResourceHandle<GpuResourceType> VertexLayoutHandle;

#endif // #if defined(DG_RENDERER_DX9)

} // namespace dg
