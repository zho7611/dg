// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

bool RC_SetRenderState(RenderStateType type, uint32_t value) {
  return true;
}

bool RC_PresentScreen() {
  return true;
}

bool RC_ClearScreen(ColorInt clear_color, bool is_depth_clear) {
  return true;
}

bool RC_DrawIndexedPrimitive(
    DrawPrimitiveType primitive_type,
    unsigned int num_primitives) {
  return true;
}

bool RC_CreateVertexShader(const Cstr* name, uint8_t* data, VertexShaderHandle& out_vertex_shader) {
  return true;
}

bool RC_SetVertexShader(const Cstr* name, VertexShaderHandle& vertex_shader) {
  return true;
}

bool RC_CreatePixelShader(const Cstr* name, uint8_t* data, PixelShaderHandle& out_pixel_shader) {
  return true;
}

bool RC_SetPixelShader(const Cstr* name, PixelShaderHandle& pixel_shader) {
  return true;
}

bool RC_SetVertexShaderConstantFloat(const Cstr* name, int start_register, const float* data, int count) {
  return true;
}

bool RC_SetPixelShaderConstantFloat(const Cstr* name, int start_register, const float* data, int count) {
  return true;
}

bool RC_CreateTexture2D(
    const Cstr* name, TextureHandle& out_texture,
    int width, int height, int mip_levels,
    GpuUsage usage, GpuFormat format, GpuPool pool) {
  return true;
}

bool RC_CreateTexture2DFromBlob(
    const Cstr* name, TextureHandle& out_texture,
    BlobBase* blob,
    GpuUsage usage, GpuPool pool,
    int& out_width, int& out_height,
    int& out_mip_levels,
    GpuFormat& out_format) {
  return true;
}

bool RC_TextureToBlob(
    const Cstr* name, TextureHandle& texture,
    const Cstr* extension, BlobBase* blob) {
  return true;
}

bool RC_SetTexture(
    const Cstr* name, int stage_index, TextureHandle& texture) {
  return true;
}

bool RC_CreateIndexBuffer(
    int length, GpuUsage usage,
    GpuPool pool, IndexBufferHandle& out_index_buffer) {
  return true;
}

bool RC_SetIndices(IndexBufferHandle& index_buffer) {
  return true;
}

bool RC_GetRenderTarget(int target_index, RenderTargetHandle& out_render_target) {
  return true;
}

bool RC_SetRenderTargetWithDepth(
    int target_index, RenderTargetHandle& render_target, SurfaceHandle& depth_surface) {
  return true;
}

bool RC_CreateDepthStencilSurface(
    int width, int height,
    GpuFormat depth_format, int multi_sample_type, int multi_sample_quality,
    SurfaceHandle& out_surface) {
  return true;
}

bool RC_GetDepthStencilSurface(SurfaceHandle& out_depth_surface) {
  return true;
}

bool RC_SetSamplerState(int stage_index, SamplerStateType sampler_state, int value) {
  return true;
}

bool RC_SetStreamSource(
    int stream_index,
    VertexBufferHandle& buffer,
    int stream_offset_in_bytes, int stride) {
  return true;
}

bool RC_CreateVertexBuffer(
    int size_bytes, GpuUsage usage, GpuPool pool,
    VertexBufferHandle& buffer) {
  return true;
}

bool RC_LockVertexBuffer(
    VertexBufferHandle& buffer, int offset, int size, uint8_t** out_buffer,
    GpuLockType lock_flag) {
  return true;
}

bool RC_UnlockVertexBuffer(VertexBufferHandle& buffer) {
  return true;
}

bool RC_LockIndexBuffer(
    IndexBufferHandle& buffer, int offset, int size, uint8_t** out_buffer,
    GpuLockType lock_flag) {
  return true;
}

bool RC_UnlockIndexBuffer(IndexBufferHandle& buffer) {
  return true;
}

bool RC_LockTexture(
    TextureHandle& texture, int level, uint8_t*& out_buffer,
    int& out_pitch, GpuLockType lock_flag) {
  return true;
}

bool RC_UnlockTexture(TextureHandle& texture, int level) {
  return true;
}

bool RC_CreateOffscreenPlainSurface(
    SurfaceHandle& out_surface, int width, int height, GpuFormat format) {
  return true;
}

bool RC_GetRenderTargetData(
    SurfaceHandle& render_target, SurfaceHandle& lockable_surface) {
  return true;
}

bool RC_LockSurfarce(
    SurfaceHandle& lockable_surface,
    uint8_t*& out_buffer, int& out_pitch) {
  return true;
}

bool RC_UnlockSurface(SurfaceHandle& lockable_surface) {
  return true;
}

bool RC_CreateRenderTargetFromTexture(
    TextureHandle& texture, int level, SurfaceHandle& out_surface) {
  return true;
}

bool RC_SetViewport(Viewport& viewport) {
  return true;
}

} // namespace dg
