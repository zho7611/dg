// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

enum GpuUsage {
  GpuUsage_None = 0,
  GpuUsage_WriteOnly,
  GpuUsage_SoftwareProcessing,
  GpuUsage_Dynamic,
  GpuUsage_RenderTarget,
  GpuUsage_DepthStencil,
  GpuUsage_Max,
};

enum GpuPool {
  GpuPool_Managed,
  GpuPool_Default,
  GpuPool_Max,
};

enum GpuFormat {
  GpuFormat_Unknown,
  GpuFormat_Index16,
  GpuFormat_Index32,
  GpuFormat_R8G8B8,
  GpuFormat_A8R8G8B8,
  GpuFormat_X8R8G8B8,
  GpuFormat_A8B8G8R8,
  GpuFormat_R32F,
  GpuFormat_A16B16G16R16F,
  GpuFormat_A32B32G32R32F,
  GpuFormat_D24S8,
  GpuFormat_D16,
  GpuFormat_A8,
  GpuFormat_L8,
  GpuFormat_Max,
};

enum GpuLockType {
  GpuLockType_None = 0,
  GpuLockType_Discard,
  GpuLockType_NoDirtyUpdate,
  GpuLockType_NoSysLock,
  GpuLockType_NoOverwrite,
  GpuLockType_NoWait,
  GpuLockType_ReadOnly,
  GpuLockType_Max,
};

enum DrawPrimitiveType {
  DrawPrimitiveType_None = 0,
  DrawPrimitiveType_PointList = 1,
  DrawPrimitiveType_LineList = 2,
  DrawPrimitiveType_LineStrip = 3,
  DrawPrimitiveType_TriangleList = 4,
  DrawPrimitiveType_TriangleStrip = 5,
  DrawPrimitiveType_TriangleFan = 6,
  DrawPrimitiveType_Max,
};

// Renderer [[
bool RC_ClearScreen(ColorInt clear_color, bool is_depth_clear);
bool RC_PresentScreen();
bool RC_SetViewport(struct Viewport& viewport);
bool RC_DrawIndexedPrimitive(DrawPrimitiveType primitive_type, unsigned int num_primitives);
// ]] renderer
// Vertex buffer, index buffer [[
bool RC_CreateVertexDeclaration(const struct VertexElement* elements, int elements_count, VertexLayoutHandle& out_vertex_decl);
bool RC_SetVertexDeclaration(VertexLayoutHandle& vertex_decl);
bool RC_CreateVertexBuffer(int size_bytes, GpuUsage usage, GpuPool pool, VertexBufferHandle& vertex_buffer);
bool RC_LockVertexBuffer(VertexBufferHandle& vertex_buffer, int offset, int size, uint8_t** out_buffer, GpuLockType lock_flag);
bool RC_UnlockVertexBuffer(VertexBufferHandle& vertex_buffer);
bool RC_SetStreamSource(int stream_index, VertexBufferHandle& vertex_buffer, int stream_offset_in_bytes, int stride);
bool RC_CreateIndexBuffer(int size_bytes, GpuUsage usage, GpuPool pool, IndexBufferHandle& out_index_buffer);
bool RC_SetIndices(IndexBufferHandle& index_buffer);
bool RC_LockIndexBuffer(IndexBufferHandle& index_buffer, int offset, int size, uint8_t** out_buffer, GpuLockType lock_flag);
bool RC_UnlockIndexBuffer(IndexBufferHandle& index_buffer);
// ]] vertex buffer, index buffer
// Shaders [[
bool RC_CreateShaderProgram(ShaderProgramHandle& out_shader_program, VertexShaderHandle& vertex_shader, PixelShaderHandle& pixel_shader);
bool RC_SetShaderProgram(const Cstr* name, ShaderProgramHandle& shader_program_handle, VertexShaderHandle& vertex_shader, PixelShaderHandle& pixel_shader);
bool RC_CreateVertexShader(const Cstr* name, uint8_t* data, int data_size, VertexShaderHandle& out_vertex_shader);
bool RC_SetVertexShader(const Cstr* name, VertexShaderHandle& vertex_shader);
bool RC_SetVertexShaderConstantInteger(const Cstr* name, int start_register, const int* constant_data, int vector4_count);
bool RC_SetVertexShaderConstantFloat(const Cstr* name, int start_register, const float* constant_data, int vector4_count);
bool RC_SetVertexShaderConstantMatrix(const Cstr* name, int start_register, const Matrix* matrix_data);
bool RC_CreatePixelShader(const Cstr* name, uint8_t* data, int data_size, PixelShaderHandle& out_pixel_shader);
bool RC_SetPixelShader(const Cstr* name, PixelShaderHandle& pixel_shader);
bool RC_GetShaderConstantStartRegister(const Cstr* name, int* out_start_register);
bool RC_SetPixelShaderConstantFloat(const Cstr* name, int start_register, const float* constant_data, int vector4_count);
bool RC_SetPixelShaderConstantMatrix(const Cstr* name, int start_register, const Matrix* matrix_data);
// ]] shaders
// Render states [[
bool RC_SetRenderState(RenderStateType type, uint32_t value);
bool RC_SetSamplerState(int stage_index, SamplerStateType sampler_state, int value);
// ]] render states
// Textures [[
// Mip_levels : number of mip levels to generate. if mip_levels = 0, all mip levels will be generated
bool RC_CreateTexture2D(const Cstr* name, TextureHandle& out_texture, int width, int height, int mip_levels, GpuUsage usage, GpuFormat format, GpuPool pool);
bool RC_CreateTexture2DFromBlob(const Cstr* name, TextureHandle& out_texture, BlobBase* blob, GpuUsage usage, GpuPool pool, int& out_width, int& out_height, int& out_mip_levels, GpuFormat& out_format);
bool RC_TextureToBlob(const Cstr* name, TextureHandle& texture, const Cstr* extension, BlobBase* blob);
bool RC_LockTexture(TextureHandle& texture, int level, uint8_t*& out_buffer, int& out_pitch, GpuLockType lock_flag);
bool RC_UnlockTexture(TextureHandle& texture, int level);
bool RC_SetTexture(const Cstr* name, int stage_index, TextureHandle& texture);
// ]] textures
// Render target, surfaces [[
bool RC_CreateRenderTargetFromTexture(TextureHandle& texture, int level, RenderTargetHandle& out_render_target);
// Get the render target that is currently binded for the device
bool RC_GetRenderTarget(int target_index, RenderTargetHandle& out_render_target);
bool RC_SetRenderTargetWithDepth(int target_index, RenderTargetHandle& render_target, SurfaceHandle& depth_surface);
bool RC_GetRenderTargetData(RenderTargetHandle& render_target, SurfaceHandle& lockable_surface);
bool RC_CreateDepthStencilSurface(int width, int height, GpuFormat format, int multi_sample_type, int multi_sample_quality, SurfaceHandle& out_surface);
bool RC_GetDepthStencilSurface(SurfaceHandle& out_depth_surface);
bool RC_CreateOffscreenPlainSurface(SurfaceHandle& out_surface, int width, int height, GpuFormat format);
bool RC_LockSurfarce(SurfaceHandle& lockable_surface, uint8_t*& out_buffer, int& out_pitch);
bool RC_UnlockSurface(SurfaceHandle& lockable_surface);
// ]] render target, surfaces
// X, y : upper-left (0, 0) is default. dx9 convention.
// Range check should be done by the caller
bool RC_SetScissorRect(int x, int y, int width, int height);
} // namespace dg
