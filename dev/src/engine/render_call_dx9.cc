// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

static DWORD UsageToDx9(GpuUsage usage) {
  switch (usage) {
  case GpuUsage_None:
    return 0;
  case GpuUsage_WriteOnly:
    return D3DUSAGE_WRITEONLY;
  case GpuUsage_SoftwareProcessing:
    return D3DUSAGE_SOFTWAREPROCESSING;
  case GpuUsage_Dynamic:
    return D3DUSAGE_DYNAMIC;
  case GpuUsage_RenderTarget:
    return D3DUSAGE_RENDERTARGET;
  case GpuUsage_DepthStencil:
    return D3DUSAGE_DEPTHSTENCIL;
  }
  DG_UNREACHABLE();
  return 0;
}

static D3DPOOL PoolToDx9(GpuPool pool) {
  switch (pool) {
  case GpuPool_Managed:
    return D3DPOOL_MANAGED;
  case GpuPool_Default:
    return D3DPOOL_DEFAULT;
  }
  DG_UNREACHABLE();
  return D3DPOOL_DEFAULT;
}

static DWORD LockTypeToDx9(GpuLockType lockType) {
  switch (lockType) {
  case GpuLockType_None:
    return 0;
  case GpuLockType_Discard:
    return D3DLOCK_DISCARD;
  case GpuLockType_NoDirtyUpdate:
    return D3DLOCK_NO_DIRTY_UPDATE;
  case GpuLockType_NoSysLock:
    return D3DLOCK_NOSYSLOCK;
  case GpuLockType_NoOverwrite:
    return D3DLOCK_NOOVERWRITE;
  case GpuLockType_NoWait:
    return D3DLOCK_NOOVERWRITE;
  case GpuLockType_ReadOnly:
    return D3DLOCK_READONLY;
  }
  DG_UNREACHABLE();
  return GpuLockType_None;
}

static D3DFORMAT FormatToDx9(GpuFormat format) {
  switch (format) {
  case GpuFormat_Unknown:
    return D3DFMT_UNKNOWN;
  case GpuFormat_Index16:
    return D3DFMT_INDEX16;
  case GpuFormat_Index32:
    return D3DFMT_INDEX32;
  case GpuFormat_R8G8B8:
    return D3DFMT_R8G8B8;
  case GpuFormat_A8B8G8R8:
    return D3DFMT_A8B8G8R8;
  case GpuFormat_X8R8G8B8:
    return D3DFMT_X8R8G8B8;
  case GpuFormat_A8R8G8B8:
    return D3DFMT_A8R8G8B8;
  case GpuFormat_A32B32G32R32F:
    return D3DFMT_A32B32G32R32F;
  case GpuFormat_R32F:
    return D3DFMT_R32F;
  case GpuFormat_A16B16G16R16F:
    return D3DFMT_A16B16G16R16F;
  case GpuFormat_D24S8:
    return D3DFMT_D24S8;
  case GpuFormat_D16:
    return D3DFMT_D16;
  case GpuFormat_A8:
    return D3DFMT_A8;
  case GpuFormat_L8:
    return D3DFMT_L8;
  }
  DG_UNREACHABLE();
  return D3DFMT_UNKNOWN;
}

static GpuFormat FormatFromDx9(D3DFORMAT format) {
  switch (format) {
  case D3DFMT_UNKNOWN:
    return GpuFormat_Unknown;
  case D3DFMT_INDEX16:
    return GpuFormat_Index16;
  case D3DFMT_INDEX32:
    return GpuFormat_Index32;
  case D3DFMT_R8G8B8:
    return GpuFormat_R8G8B8;
  case D3DFMT_A8B8G8R8:
    return GpuFormat_A8B8G8R8;
  case D3DFMT_X8R8G8B8:
    return GpuFormat_X8R8G8B8;
  case D3DFMT_A8R8G8B8:
    return GpuFormat_A8R8G8B8;
  case D3DFMT_R32F:
    return GpuFormat_R32F;
  case D3DFMT_A32B32G32R32F:
    return GpuFormat_A32B32G32R32F;
  case D3DFMT_A16B16G16R16F:
    return GpuFormat_A16B16G16R16F;
  case D3DFMT_D24S8:
    return GpuFormat_D24S8;
  case D3DFMT_D16:
    return GpuFormat_D16;
  case D3DFMT_A8:
    return GpuFormat_A8;
  case D3DFMT_L8:
    return GpuFormat_L8;
  }
  DG_UNREACHABLE();
  return GpuFormat_Unknown;
}

// Enable the following if you want to debug render call
//#define DebugRenderCall

#ifdef DebugRenderCall
static const Cstr* GetRenderStateTypeStringDx9(RenderStateType type) {
  switch (type) {
  case RenderStateType_ZEnable:
    return TXT("ZEnable");
  case RenderStateType_ZWriteEnable:
    return TXT("ZWriteEnable");
  case RenderStateType_CullMode:
    return TXT("CullMode");
  case RenderStateType_ZFunc:
    return TXT("ZFunc");
  case RenderStateType_AlphaBlendEnable:
    return TXT("AlphaBlendEnable");
  case RenderStateType_AlphaTestEnable:
    return TXT("AlphaTestEnable");
  case RenderStateType_AlphaFunc:
    return TXT("AlphaFunc");
  case RenderStateType_AlphaRef:
    return TXT("AlphaRef");
  case RenderStateType_SrcBlend:
    return TXT("SrcBlend");
  case RenderStateType_DestBlend:
    return TXT("DestBlend");
  case RenderStateType_BlendOp:
    return TXT("BlendOp");
  case RenderStateType_ScissorTestEnable:
    return TXT("ScissorTestEnable");
  default:
    DG_UNIMPLEMENTED();
    break;
  }
  return NULL;
}

static const Cstr* GetSamplerStateTypeStringDx9(SamplerStateType type) {
  switch (type) {
  case SamplerStateType_None:
    return TXT("None");
  case SamplerStateType_AddressU:
    return TXT("AddressU");
  case SamplerStateType_AddressV:
    return TXT("AddressV");
  case SamplerStateType_BorderColor:
    return TXT("BorderColor");
  case SamplerStateType_MagFilter:
    return TXT("MagFilter");
  case SamplerStateType_MinFilter:
    return TXT("MinFilter");
  case SamplerStateType_MipFilter:
    return TXT("MipFilter");
  case SamplerStateType_MipmapLodBias;
    return TXT("MipmapLodBias");
  case SamplerStateType_MaxMipLevel;
    return TXT("MaxMipLevel");
  case SamplerStateType_MaxAnisotropy:
    return TXT("MaxAnisotropy");
  case SamplerStateType_SrgbTexture:
    return TXT("SrgbTexture");
  case SamplerStateType_ElementIndex;
    return TXT("ElementIndex");
  case SamplerStateType_DmapOffset:
    return TXT("DmapOffset");
  }
}
#endif

bool RC_SetRenderState(RenderStateType type, uint32_t value) {
#ifdef DebugRenderCall
  DG_LOG_LINE(TXT("rendercall.setrenderstate: type:%s value:%d"), GetRenderStateTypeStringDx9(type), value);
#endif
  g_renderer.states.SetRenderState(type, value);
  D3DRENDERSTATETYPE type_dx9 = D3DRENDERSTATETYPE(type);
  if (FAILED(g_renderer.device_dx9_->SetRenderState(type_dx9, value))) {
    DG_LOG_LINE(TXT("error: rendercall.setrenderstate.failed:"));
    return false;
  }
  return true;
}

bool RC_PresentScreen() {
#ifdef DebugRenderCall
  DG_LOG_LINE(TXT("rendercall.presentscreen:"));
#endif
  HRESULT hr = g_renderer.device_dx9_->Present(NULL, NULL, NULL, NULL);
  if (D3DERR_DEVICELOST == hr) {
    DG_LOG_LINE(TXT("rendercall.presentscreen.lostdevice:"));
    return false;
  }
  return true;
}

bool RC_ClearScreen(ColorInt clear_color, bool is_depth_clear) {
#ifdef DebugRenderCall
  DG_LOG_LINE(TXT("rendercall.clearscreen: clearcolor:%x isdepthclear:%d"), clear_color, is_depth_clear);
#endif
  float kClearDepth = 1.0f;
  DWORD kClearStencil = 0L;
  DWORD kNumRect = 0L;
  D3DRECT* kRects = NULL;
  DWORD kFlag = D3DCLEAR_TARGET;
  if (is_depth_clear) {
    kFlag |= D3DCLEAR_ZBUFFER;
  }
  if (FAILED(g_renderer.device_dx9_->Clear(
      kNumRect,
      kRects,
      kFlag,
      clear_color,
      kClearDepth,
      kClearStencil))) {
    DG_LOG_LINE(TXT("error: rendercall.clearscreen.failed:"));
    return false;
  }
  return true;
}

bool RC_DrawIndexedPrimitive(
    DrawPrimitiveType primitive_type,
    unsigned int num_primitives) {
#ifdef DebugRenderCall
  DG_LOG_LINE(TXT("rendercall.drawindexedprimitive: primtype:%d numprims:%d"),
      primitive_type, num_primitives);
#endif
  Check(g_renderer.GetIsInBeginScene());
  INT kBaseVertexIndex = 0;
  UINT kMinVertexIndex = 0;
  UINT kStartIndex = 0;
  UINT num_vertices = 0;
  switch (primitive_type) {
  case D3DPT_LINELIST:
    num_vertices = num_primitives*3;
    break;
  case D3DPT_TRIANGLELIST:
    num_vertices = num_primitives*3;
    break;
  default:
    DG_UNREACHABLE();
  }
  if (FAILED(g_renderer.device_dx9_->DrawIndexedPrimitive(
      (D3DPRIMITIVETYPE)primitive_type,
      kBaseVertexIndex,
      kMinVertexIndex,
      num_vertices,
      kStartIndex,
      num_primitives))) {
    dg::LogErrorMsg(TXT("error: rendercall.drawindexedprimitive.failed:"));
    return false;
  }
  return true;
}

bool RC_CreateVertexShader(const Cstr* name, uint8_t* data, int data_size, VertexShaderHandle& out_vertex_shader) {
#ifdef DebugRenderCall
  DG_LOG_LINE(TXT("rendercall.createvertexshader: name:%s"), name);
#endif
  DG_UNREF(data_size);
  Check(!out_vertex_shader);
  if (FAILED(g_renderer.device_dx9_->CreateVertexShader(
      reinterpret_cast<DWORD*>(data),
      out_vertex_shader.GetRef()))) {
    DG_LOG_LINE(TXT("error: rendercall.createvertexshader.failed:"));
    return false;
  }
  return true;
}

bool RC_SetVertexShader(const Cstr* name, VertexShaderHandle& vertex_shader) {
#ifdef DebugRenderCall
  DG_LOG_LINE(TXT("rendercall.setvertexshader: name:%s ptr:%x"), name, vertex_shader.Get());
#endif
  if (FAILED(g_renderer.device_dx9_->SetVertexShader(vertex_shader))) {
    DG_LOG_LINE(TXT("error: rendercall.setvertexshader.failed: name:%s"), name);
    return false;
  }
  return true;
}

bool RC_CreatePixelShader(const Cstr* name, uint8_t* data, int data_size, PixelShaderHandle& out_pixel_shader) {
#ifdef DebugRenderCall
  DG_LOG_LINE(TXT("rendercall.createpixelshader: name:%s"), name);
#endif
  DG_UNREF(data_size);
  Check(!out_pixel_shader);
  if (FAILED(g_renderer.device_dx9_->CreatePixelShader(
      reinterpret_cast<DWORD*>(data),
      out_pixel_shader.GetRef()))) {
    DG_LOG_LINE(TXT("error: rendercall.createpixelshader.failed: name:%s"), name);
    return false;
  }
  return true;
}

bool RC_SetPixelShader(const Cstr* name, PixelShaderHandle& pixel_shader) {
#ifdef DebugRenderCall
  DG_LOG_LINE(TXT("rendercall.setpixelshader: name:%s ptr:%x"), name, pixel_shader.Get());
#endif
  if (FAILED(g_renderer.device_dx9_->SetPixelShader(pixel_shader))) {
    DG_LOG_LINE(TXT("error: rendercall.setpixelshader.failed: name:%s"), name);
    return false;
  }
  return true;
}

bool RC_GetShaderConstantStartRegister(const Cstr* name, int* out_start_register) {
#ifdef DebugRenderCall
  DG_LOG_LINE(TXT("rendercall.getshaderconstantstartregister: name:%s"),
      name);
#endif
  // For dx9, it is not needed
  Check(*out_start_register >= 0);
  return true;
}

bool RC_SetVertexShaderConstantInteger(const Cstr* name, int start_register, const int* data, int count) {
#ifdef DebugRenderCall
  DG_LOG_LINE(TXT("rendercall.setvertexshaderconstantingeter: name:%s startregister:%d count:%d"),
      name, start_register, count);
#endif
  Check(start_register >= 0);
  Check(data);
  Check(count >= 0);
  if (FAILED(g_renderer.device_dx9_->SetVertexShaderConstantI(start_register, data, count))) {
    DG_LOG_LINE(TXT("error: rendercall.setvertexshaderconstantf.failed: name:%s"), name);
    return false;
  }
  return true;
}

bool RC_SetVertexShaderConstantFloat(
    const Cstr* name, int start_register, const float* data, int vector4_count) {
#ifdef DebugRenderCall
  DG_LOG_LINE(TXT("rendercall.setvertexshaderconstantfloat: name:%s startregister:%d count:%d"),
      name, start_register, vector4_count);
#endif
  Check(start_register >= 0);
  Check(data);
  Check(vector4_count >= 0);
  if (FAILED(g_renderer.device_dx9_->SetVertexShaderConstantF(start_register, data, vector4_count))) {
    DG_LOG_LINE(TXT("error: rendercall.setvertexshaderconstantfloat.failed: name:%s"), name);
    return false;
  }
  return true;
}

bool RC_SetVertexShaderConstantMatrix(const Cstr* name, int start_register, const Matrix* matrix_data) {
#ifdef DebugRenderCall
  DG_LOG_LINE(TXT("rendercall.setvertexshaderconstantmatrix: name:%s startregister:%d"),
      name, start_register);
#endif
  return RC_SetVertexShaderConstantFloat(name, start_register, matrix_data->GetFloatsConst(), 4);
}

bool RC_SetPixelShaderConstantFloat(const Cstr* name, int start_register, const float* data, int vector4_count) {
#ifdef DebugRenderCall
  DG_LOG_LINE(TXT("rendercall.setpixelshaderconstantfloat: name:%s startregister:%d count:%d"),
      name, start_register, vector4_count);
#endif
  if (FAILED(g_renderer.device_dx9_->SetPixelShaderConstantF(start_register, data, vector4_count))) {
    DG_LOG_LINE(TXT("error: rendercall.setpixelshaderconstantfloatc.failed: name:%s"), name);
    return false;
  }
  return true;
}

bool RC_SetPixelShaderConstantMatrix(const Cstr* name, int start_register, const Matrix* data) {
#ifdef DebugRenderCall
  DG_LOG_LINE(TXT("rendercall.setpixelshaderconstantmatrix: name:%s startregister:%d count:%d"),
      name, start_register);
#endif
  return RC_SetPixelShaderConstantFloat(name, start_register, data->GetFloatsConst(), 4);
}

bool RC_CreateTexture2D(
    const Cstr* name, TextureHandle& out_texture,
    int width, int height, int mip_levels,
    GpuUsage usage, GpuFormat format, GpuPool pool) {
#ifdef DebugRenderCall
  DG_LOG_LINE(TXT("rendercall.createtexture: name:%s width:%d height:%d"), name, width, height);
#endif
  if (FAILED(g_renderer.device_dx9_->CreateTexture(
      width,
      height,
      mip_levels,
      UsageToDx9(usage),
      FormatToDx9(format),
      PoolToDx9(pool),
      out_texture.GetRef(),
      0 /* SharedHandle */))) {
    DG_LOG_LINE(TXT("error: rendercall.createtexture.failed: name:%s"), name);
    return false;
  }
  return true;
}

bool RC_CreateTexture2DFromBlob(
    const Cstr* name, TextureHandle& out_texture, BlobBase* blob,
    GpuUsage usage, GpuPool pool,
    int& out_width, int& out_height,
    int& out_mip_levels, GpuFormat& out_format) {
#ifdef DebugRenderCall
  DG_LOG_LINE(TXT("rendercall.createtexturefromblob: name:%s"), name);
#endif
  bool is_raw_texture = false;
  texture_util::RawTextureHeader header;
  D3DFORMAT internal_format = D3DFMT_UNKNOWN;
  if (blob->size() > sizeof(texture_util::RawTextureHeader)) {
    // DGTX format
    MyMemCpy(&header, blob->data_const(), sizeof(header));
    if (MyMemCmp(header.signature, texture_util::RawTextureHeader::GetSignature(), 4) == 0) {
      is_raw_texture = true;
    }
  }
  if (is_raw_texture) {
    Check(header.storage_format == GpuFormat_A8R8G8B8);
    out_width = header.width;
    out_height = header.height;
    out_format = header.storage_format;
    internal_format = FormatToDx9(out_format);
    out_mip_levels = header.num_mips;
    uint8_t* rgba = blob->data() + sizeof(header);
    int rgba_size = blob->size() - sizeof(header);
    if (FAILED(g_renderer.device_dx9_->CreateTexture(
        out_width,
        out_height,
        out_mip_levels,
        UsageToDx9(usage),
        internal_format,
        PoolToDx9(pool),
        out_texture.GetRef(),
        0 /* SharedHandle */))) {
      DG_LOG_LINE(TXT("error: rendercall.createtexture.failed: name:%s"), name);
      return false;
    }
    D3DLOCKED_RECT locked_rect;
    out_texture.Get()->LockRect(0, &locked_rect, NULL, GpuLockType_None);
    int pitch = locked_rect.Pitch;
    uint8_t* locked_data = reinterpret_cast<uint8_t*>(locked_rect.pBits);
    MyMemCpy(locked_data, rgba, rgba_size);
    out_texture.Get()->UnlockRect(0);
    return true;
  } 
  D3DXIMAGE_INFO image_info;
  if (FAILED(::D3DXGetImageInfoFromFileInMemory(
      blob->data(),
      blob->size(), &image_info))) {
    DG_LOG_LINE(TXT("error: rendercall.createtexturefromblob.failed: imageinfo.invalid"));
    return false;
  }
  out_width = image_info.Width;
  out_height = image_info.Height;
  out_format = FormatFromDx9(image_info.Format);
  internal_format = image_info.Format;
  // If the DeviceTextureHandle_ is to be used for 2d UI and has the odd size, then no need to have all miplevels
  out_mip_levels = ((out_width & 1) || (out_height & 1)) ? 1 : 0;
  DWORD kFilter = D3DX_DEFAULT;
  DWORD kMipFilter = D3DX_DEFAULT;
  if (FAILED(::D3DXCreateTextureFromFileInMemoryEx(
      g_renderer.device_dx9_,
      blob->data(),
      blob->size(),
      out_width,
      out_height,
      out_mip_levels,
      UsageToDx9(usage),
      internal_format,
      PoolToDx9(pool),
      kFilter,
      kMipFilter,
      0, // Color key
      NULL, // Source info
      NULL, // Palette
      out_texture.GetRef()))) {
    DG_LOG_LINE(TXT("error: rendercall.createtexturefromblob.failed: name:%s"), name);
    return false;
  }
  return true;
}

bool RC_TextureToBlob(
    const Cstr* name, TextureHandle& texture, const Cstr* extension, BlobBase* blob) {
#ifdef DebugRenderCall
  DG_LOG_LINE(TXT("rendercall.texturetoblob: name:%s"), name);
#endif
  LPD3DXBUFFER buff;
  D3DXIMAGE_FILEFORMAT dest_format;
  if (MyStriCmp(extension, TXT("DDS")) == 0) {
    dest_format = D3DXIFF_DDS;
  } else if ((MyStriCmp(extension, TXT("BMP")) == 0)) {
    dest_format = D3DXIFF_BMP;
  } else if ((MyStriCmp(extension, TXT("JPG")) == 0)) {
    dest_format = D3DXIFF_JPG;
  } else if ((MyStriCmp(extension, TXT("TGA")) == 0)) {
    dest_format = D3DXIFF_TGA;
  } else {
    DG_UNIMPLEMENTED();
  }
  Check(texture);
  if (FAILED(::D3DXSaveTextureToFileInMemory(&buff, dest_format, texture.Get(), 0 /* palette */))) {
    DG_LOG_LINE(TXT("error: rendercall.texturetoblob.failed: name:%s"), name);
    return false;
  }
  blob->Clear();
  blob->CopyFrom((const uint8_t*)buff->GetBufferPointer(), buff->GetBufferSize());
  DG_SAFE_RELEASE(buff);
  return true;
}

bool RC_SetTexture(const Cstr* name, int stage_index, TextureHandle& texture) {
#ifdef DebugRenderCall
  DG_LOG_LINE(TXT("rendercall.settexture: name:%s stageindex:%d"), name, stage_index);
#endif
  if (FAILED(g_renderer.device_dx9_->SetTexture(stage_index, texture.Get()))) {
    DG_LOG_LINE(TXT("error: rendercall.settexture.failed: name:%s"), name);
    return false;
  }
  g_renderer.states.textures[stage_index] = texture;
  return true;
}

bool RC_CreateIndexBuffer(
    int size_bytes,
    GpuUsage usage,
    GpuPool pool,
    IndexBufferHandle& out_index_buffer) {
#ifdef DebugRenderCall
  DG_LOG_LINE(TXT("rendercall.createindexbuffer: length:%d usage:%d format:%d pool:%d"), size_bytes, usage, format, pool);
#endif
#if defined(DG_USE_32BIT_INDEX)
  GpuFormat format = GpuFormat_Index32;
#else
  GpuFormat format = GpuFormat_Index16;
#endif
  if (FAILED(g_renderer.device_dx9_->CreateIndexBuffer(
      size_bytes,
      UsageToDx9(usage),
      FormatToDx9(format),
      PoolToDx9(pool),
      out_index_buffer.GetRef(),
      NULL // Shared handle
      ))) {
    DG_LOG_LINE(TXT("error: rendercall.createindexbuffer.failed"));
    return false;
  }
  return true;
}

bool RC_SetIndices(IndexBufferHandle& index_buffer) {
#ifdef DebugRenderCall
  DG_LOG_LINE(TXT("rendercall.setindices:"));
#endif
  if (FAILED(g_renderer.device_dx9_->SetIndices(index_buffer.Get()))) {
    DG_LOG_LINE(TXT("error: rendercall.setindices.failed"));
    return false;
  }
  return true;
}

bool RC_GetRenderTarget(int target_index, RenderTargetHandle& out_render_target) {
#ifdef DebugRenderCall
  DG_LOG_LINE(TXT("rendercall.getrendertarget:"));
#endif
  if (FAILED(g_renderer.device_dx9_->GetRenderTarget(target_index, out_render_target.GetRef()))) {
    DG_LOG_LINE(TXT("error: rendercall.getrendertarget.failed:"));
    return false;
  }
  return true;
}

bool RC_SetRenderTargetWithDepth(
    int target_index, RenderTargetHandle& render_target, SurfaceHandle& depth_surface) {
#ifdef DebugRenderCall
  DG_LOG_LINE(TXT("rendercall.setrendertarget:"));
#endif
  if (FAILED(g_renderer.device_dx9_->SetRenderTarget(target_index, render_target.Get()))) {
    DG_LOG_LINE(TXT("error: rendercall.setrendertarget.failed:"));
    return false;
  }
  if (FAILED(g_renderer.device_dx9_->SetDepthStencilSurface(depth_surface.Get()))) {
    DG_LOG_LINE(TXT("error: rendercall.setdepthstencilsurface.failed:"));
    return false;
  }
  return true;
}

bool RC_CreateDepthStencilSurface(
    int width, int height, GpuFormat depth_format,
    int multi_sample_type, int multi_sample_quality,
    SurfaceHandle& out_surface) {
#ifdef DebugRenderCall
  DG_LOG_LINE(TXT("rendercall.createdepthstencilsurface:"));
#endif
  Check(!out_surface);
  const BOOL kIsDiscard = FALSE;
  Check(multi_sample_type >= D3DMULTISAMPLE_NONE);
  Check(multi_sample_type <= D3DMULTISAMPLE_16_SAMPLES);
  D3DMULTISAMPLE_TYPE multiSampleTypeDx9 = (D3DMULTISAMPLE_TYPE)(multi_sample_type);
  DWORD multiSampleQualityDx9 = multi_sample_quality;
  if (FAILED(g_renderer.device_dx9_->CreateDepthStencilSurface(
      width,
      height,
      FormatToDx9(depth_format),
      multiSampleTypeDx9,
      multiSampleQualityDx9,
      kIsDiscard,
      out_surface.GetRef(),
      NULL))) {
    DG_LOG_LINE(TXT("error: rendercall.createdepthstencilsurface.failed:"));
    return false;
  }
  return true;
}

bool RC_GetDepthStencilSurface(SurfaceHandle& out_depth_surface) {
#ifdef DebugRenderCall
  DG_LOG_LINE(TXT("rendercall.getdepthstencilsurface:"));
#endif
  if (FAILED(g_renderer.device_dx9_->GetDepthStencilSurface(out_depth_surface.GetRef()))) {
    DG_LOG_LINE(TXT("error: rendercall.getdepthstencilsurface.failed:"));
    return false;
  }
  return true;
}

bool RC_SetSamplerState(int stage_index, SamplerStateType sampler_state, int value) {
#ifdef DebugRenderCall
  DG_LOG_LINE(TXT("rendercall.setsamplerstate: index:%d type:%s value:%d"),
      stage_index,
      GetSamplerStateTypeStringDx9(sampler_state),
      value);
#endif
  const D3DSAMPLERSTATETYPE kSamplerStateDx9 =
      static_cast<D3DSAMPLERSTATETYPE>(sampler_state);
  if (FAILED(g_renderer.device_dx9_->SetSamplerState(stage_index,
              kSamplerStateDx9, value))) {
    DG_LOG_LINE(TXT("error: rendercall.setsamplerstate.failed:"));
    return false;
  }
  return true;
}

bool RC_SetStreamSource(
    int stream_index, VertexBufferHandle& vertex_buffer, int stream_offset_in_bytes, int stride) {
#ifdef DebugRenderCall
  DG_LOG_LINE(TXT("rendercall.setstreamsource: streamindex:%d"), stream_index);
#endif
  Check(g_renderer.device_dx9_);
  if (FAILED(g_renderer.device_dx9_->SetStreamSource(
      stream_index, vertex_buffer.Get(), stream_offset_in_bytes, stride))) {
    DG_LOG_LINE(TXT("error: rendercall.setstreamsource.failed"));
    return false;
  }
  return true;
}

bool RC_CreateVertexBuffer(
    int size_bytes, GpuUsage usage,
    GpuPool pool, VertexBufferHandle& vertex_buffer) {
#ifdef DebugRenderCall
  DG_LOG_LINE(TXT("rendercall.createvertexbuffer:"));
#endif
  Check(g_renderer.device_dx9_);
  if (FAILED(g_renderer.device_dx9_->CreateVertexBuffer(
      size_bytes,
      UsageToDx9(usage),
      NULL,
      PoolToDx9(pool),
      vertex_buffer.GetRef(),
      NULL))) {
    DG_LOG_LINE(TXT("error: rendercall.createvertexbuffer.failed:"));
    return false;
  }
  return true;
}

bool RC_LockVertexBuffer(
    VertexBufferHandle& vertex_buffer,
    int offset, int size, uint8_t** out_buffer, GpuLockType lock_flag) {
#ifdef DebugRenderCall
  DG_LOG_LINE(TXT("rendercall.lockvertexbuffer:"));
#endif
  if (FAILED(vertex_buffer.Get()->Lock(
      offset, size, (void**)out_buffer, LockTypeToDx9(lock_flag)))) {
    DG_LOG_LINE(TXT("error: rendercall.lockvertexbuffer.failed:"));
    return false;
  }
  return true;
}

bool RC_UnlockVertexBuffer(VertexBufferHandle& vertex_buffer) {
#ifdef DebugRenderCall
  DG_LOG_LINE(TXT("rendercall.unlockvertexbuffer:"));
#endif
  if (FAILED(vertex_buffer.Get()->Unlock())) {
    DG_LOG_LINE(TXT("error: rendercall.unlockvertexbuffer.failed:"));
    return false;
  }
  return true;
}

bool RC_LockIndexBuffer(IndexBufferHandle& index_buffer,
    int offset, int size, uint8_t** out_buffer, GpuLockType lock_flag) {
#ifdef DebugRenderCall
  DG_LOG_LINE(TXT("rendercall.lockindexbuffer:"));
#endif
  if (FAILED(index_buffer.Get()->Lock(
      offset,
      size, (void**)out_buffer,
      LockTypeToDx9(lock_flag)))) {
    DG_LOG_LINE(TXT("error: rendercall.lockindexbuffer.failed:"));
    return false;
  }
  return true;
}

bool RC_UnlockIndexBuffer(IndexBufferHandle& index_buffer) {
#ifdef DebugRenderCall
  DG_LOG_LINE(TXT("rendercall.unlockindexbuffer:"));
#endif
  if (FAILED(index_buffer.Get()->Unlock())) {
    DG_LOG_LINE(TXT("error: rendercall.unlockindexbuffer.failed:"));
    return false;
  }
  return true;
}

bool RC_LockTexture(TextureHandle& texture,
    int level, uint8_t*& out_buffer, int& out_pitch, GpuLockType lock_flag) {
#ifdef DebugRenderCall
  DG_LOG_LINE(TXT("rendercall.locktexture:"));
#endif
  D3DLOCKED_RECT lockedRect;
  if (FAILED(texture.Get()->LockRect(
      level,
      &lockedRect,
      NULL, // Rect
      LockTypeToDx9(lock_flag)))) {
    DG_LOG_LINE(TXT("error: rendercall.locktexture.failed:"));
    return false;
  }
  out_pitch = lockedRect.Pitch;
  out_buffer = reinterpret_cast<uint8_t*>(lockedRect.pBits);
  return true;
}

bool RC_UnlockTexture(TextureHandle& texture, int level) {
#ifdef DebugRenderCall
  DG_LOG_LINE(TXT("rendercall.unlocktexture:"));
#endif
  if (FAILED(texture.Get()->UnlockRect(level))) {
    DG_LOG_LINE(TXT("error: rendercall.unlocktexture.failed:"));
    return false;
  }
  return true;
}

bool RC_CreateOffscreenPlainSurface(SurfaceHandle& out_surface,
    int width, int height, GpuFormat format) {
#ifdef DebugRenderCall
  DG_LOG_LINE(TXT("rendercall.createoffscreenplainsurface:"));
#endif
  const D3DFORMAT kFormatDx9 = FormatToDx9(format);
  // For now, this is the only supported format
  Check(kFormatDx9 == D3DFMT_A8R8G8B8);
  if (FAILED(g_renderer.device_dx9_->CreateOffscreenPlainSurface(
      width, height,
      kFormatDx9,
      D3DPOOL_SYSTEMMEM,
      out_surface.GetRef(), NULL))) {
    DG_UNREACHABLE();
    DG_LOG_LINE(TXT("error: rendercall.createoffscreenplainsurface.failed:"));
    return false;
  }
  return true;
}

bool RC_GetRenderTargetData(
    RenderTargetHandle& renderTargetPtr, SurfaceHandle& lockableSurfacePtr) {
#ifdef DebugRenderCall
  DG_LOG_LINE(TXT("rendercall.getrendertargetdata:"));
#endif
  if (FAILED(g_renderer.device_dx9_->GetRenderTargetData(
      renderTargetPtr.Get(), lockableSurfacePtr.Get()))) {
    DG_LOG_LINE(TXT("error: rendercall.getrendertargetdata.failed:"));
    DG_UNREACHABLE();
    return false;
  }
  return true;
}

bool RC_LockSurfarce(SurfaceHandle& lockableSurfacePtr, uint8_t*& out_buffer, int& out_pitch) {
#ifdef DebugRenderCall
  DG_LOG_LINE(TXT("rendercall.locksurface:"));
#endif
  D3DLOCKED_RECT lockedRect;
  if (FAILED(lockableSurfacePtr->LockRect(&lockedRect, NULL, D3DLOCK_READONLY))) {
    DG_LOG_LINE(TXT("error: rendercall.locksurface.failed:"));
    DG_UNREACHABLE();
    return false;
  }
  out_buffer = reinterpret_cast<uint8_t*>(lockedRect.pBits);
  out_pitch = lockedRect.Pitch;
  return true;
}

bool RC_UnlockSurface(SurfaceHandle& lockableSurfacePtr) {
#ifdef DebugRenderCall
  DG_LOG_LINE(TXT("rendercall.unlocksurface:"));
#endif
  if (FAILED(lockableSurfacePtr->UnlockRect())) {
    DG_LOG_LINE(TXT("error: rendercall.unlocksurface.failed:"));
    return false;
  }
  return true;
}

bool RC_CreateRenderTargetFromTexture(
    TextureHandle& texture, int level, RenderTargetHandle& out_render_target) {
#ifdef DebugRenderCall
  DG_LOG_LINE(TXT("rendercall.getrendertargetfromtexture:"));
#endif
  if (FAILED(texture.Get()->GetSurfaceLevel(level, out_render_target.GetRef()))) {
    DG_LOG_LINE(TXT("error: rendercall.texturegetsurfacelevel.failed:"));
    return false;
  }
  return true;
}

bool RC_SetViewport(Viewport& viewport) {
#ifdef DebugRenderCall
  DG_LOG_LINE(TXT("rendercall.setviewport:"));
#endif
  D3DVIEWPORT9 viewport_dx9;
  viewport_dx9.X = viewport.top_left_x;
  viewport_dx9.Y = viewport.top_left_y;
  viewport_dx9.Width = viewport.width;
  viewport_dx9.Height = viewport.height;
  viewport_dx9.MinZ = viewport.min_depth;
  viewport_dx9.MaxZ = viewport.max_depth;
  if (FAILED(g_renderer.device_dx9_->SetViewport(&viewport_dx9))) {
    DG_LOG_LINE(TXT("error: rendercall.setviewport.failed:"));
    return false;
  }
  return true;
}

static D3DVERTEXELEMENT9* VertexDeclCreateElementDx9(
    const VertexElement* elements, int element_count) {
  if (elements == 0) {
    return 0;
  }
  D3DVERTEXELEMENT9* elems_dx9 = new D3DVERTEXELEMENT9[element_count + 1];
  int offset = 0;
  int idx = 0;
  for (idx = 0; idx < element_count; ++idx) {
    elems_dx9[idx].Stream = 0;
    elems_dx9[idx].Offset = offset;
    elems_dx9[idx].Method = D3DDECLMETHOD_DEFAULT;
    const VertexElement& elem = elements[idx];
    offset += elem.GetElementSize();
    switch (elem.data_type) {
    case VertexElement::DataType_Float1:
      elems_dx9[idx].Type = D3DDECLTYPE_FLOAT1;
      break;
    case VertexElement::DataType_Float2:
      elems_dx9[idx].Type = D3DDECLTYPE_FLOAT2;
      break;
    case VertexElement::DataType_Float3:
      elems_dx9[idx].Type = D3DDECLTYPE_FLOAT3;
      break;
    case VertexElement::DataType_Float4:
      elems_dx9[idx].Type = D3DDECLTYPE_FLOAT4;
      break;
    case VertexElement::DataType_Color:
      elems_dx9[idx].Type = D3DDECLTYPE_D3DCOLOR;
      break;
    default:
      DG_UNREACHABLE();
      break;
    }
    switch (elem.usage) {
    case VertexElement::Usage_Position:
      elems_dx9[idx].Usage = D3DDECLUSAGE_POSITION;
      break;
    case VertexElement::Usage_Color:
      elems_dx9[idx].Usage = D3DDECLUSAGE_COLOR;
      break;
    case VertexElement::Usage_Normal:
      elems_dx9[idx].Usage = D3DDECLUSAGE_NORMAL;
      break;
    case VertexElement::Usage_TexCoord:
      elems_dx9[idx].Usage = D3DDECLUSAGE_TEXCOORD;
      break;
    case VertexElement::Usage_Tangent:
      elems_dx9[idx].Usage = D3DDECLUSAGE_TANGENT;
      break;
    case VertexElement::Usage_Binormal:
      elems_dx9[idx].Usage = D3DDECLUSAGE_BINORMAL;
      break;
    case VertexElement::Usage_JointIndex:
      elems_dx9[idx].Usage = D3DDECLUSAGE_BLENDINDICES;
      break;
    case VertexElement::Usage_JointWeight:
      elems_dx9[idx].Usage = D3DDECLUSAGE_BLENDWEIGHT;
      break;
    default:
      DG_UNREACHABLE();
    }
    elems_dx9[idx].UsageIndex = elem.usage_index;
  }
  elems_dx9[idx].Stream = 0xFF;
  elems_dx9[idx].Offset = 0;
  elems_dx9[idx].Type = D3DDECLTYPE_UNUSED;
  elems_dx9[idx].Method = 0;
  elems_dx9[idx].Usage = 0;
  elems_dx9[idx].UsageIndex = 0;
  return elems_dx9;
}

bool RC_CreateVertexDeclaration(
    const struct VertexElement* elements, int elements_count,
    VertexLayoutHandle& out_vertex_decl) {
#ifdef DebugRenderCall
  DG_LOG_LINE(TXT("rendercall.createvertexdeclaration:"));
#endif
  Check(elements);
  D3DVERTEXELEMENT9* elem_dx9 = VertexDeclCreateElementDx9(elements, elements_count);
  if (FAILED(g_renderer.device_dx9_->CreateVertexDeclaration(
      elem_dx9,
      out_vertex_decl.GetRef()))) {
    DG_LOG_LINE(TXT("error: rendercall.createvertexdeclaration.failed:"));
    DG_SAFE_DELETE_ARRAY(elem_dx9);
    return false;
  }
  DG_SAFE_DELETE_ARRAY(elem_dx9);
  return true;
}

bool RC_SetVertexDeclaration(VertexLayoutHandle& vertex_decl) {
#ifdef DebugRenderCall
  DG_LOG_LINE(TXT("rendercall.setcurrentvertexdeclaration:"));
#endif
  if (FAILED(g_renderer.device_dx9_->SetVertexDeclaration(vertex_decl.Get()))) {
    DG_LOG_LINE(TXT("error: rendercall.setcurrentvertexdeclaration.failed:"));
    return false;
  }
  return true;
}

bool RC_SetShaderProgram(const Cstr* name,
    ShaderProgramHandle& shader_program_handle,
    VertexShaderHandle& vertex_shader,
    PixelShaderHandle& pixel_shader) {
#ifdef DebugRenderCall
  DG_LOG_LINE(TXT("rendercall.setshaderprogram:"));
#endif
  RC_SetVertexShader(name, vertex_shader);
  g_renderer.states.vertex_shader = vertex_shader;
  RC_SetPixelShader(name, pixel_shader);
  g_renderer.states.pixel_shader = pixel_shader;
  g_renderer.states.shader_program = shader_program_handle;
  return true;
}

bool RC_CreateShaderProgram(
    ShaderProgramHandle& out_shader_program,
    VertexShaderHandle& vertex_shader,
    PixelShaderHandle& pixel_shader) {
#ifdef DebugRenderCall
  DG_LOG_LINE(TXT("rendercall.createshaderprogram:"));
#endif
  return true;
}

bool RC_SetScissorRect(int x, int y, int width, int height) {
#ifdef DebugRenderCall
  DG_LOG_LINE(TXT("rendercall.setscissorrect:"));
#endif
  ::RECT rect;
  rect.left = x;
  rect.right = x + width;
  rect.top = y;
  rect.bottom = y + height;
  if (FAILED(g_renderer.device_dx9_->SetScissorRect(&rect))) {
    DG_LOG_LINE(TXT("error: rendercall.texturegetsurfacelevel.failed:"));
    return false;
  }
  return true;
}

} // namespace dg
