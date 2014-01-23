// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"
#include "../externals/stb_image/stb_image.c"

#if defined(DG_TARGET_ANDROID)
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#endif // #if defined(DG_TARGET_ANDROID)

#if defined(DG_TARGET_WIN32)
#include "../externals/glloadgen/gl_core_2_0.c"
#include "../externals/glloadgen/wgl_ext.c"
#pragma comment(lib, "OpenGL32.lib")
#endif // #if defined(DG_TARGET_ANDROID)

//#define DG_LOG_RENDER_CALL(...) DG_LOG_LINE(__VA_ARGS__)
#define DG_LOG_RENDER_CALL(...) 

// Comment out the following line to disable OpenGL error check
#define DG_IGNORE_GL_ERROR

#if defined(DG_IGNORE_GL_ERROR)
// OpenGL error check for the current expression
#define DG_CHECK_GL_ERROR(expr) expr
// OpenGL error check for the last expression
#define DG_CHECK_LAST_GL_ERROR()
#else
// OpenGL error check for the current expression
#define DG_CHECK_GL_ERROR(expr) { expr; GLenum error_no = glGetError(); \
  if (error_no != GL_NO_ERROR) { LogGlError(error_no); DG_UNREACHABLE(); } }
// OpenGL error check for the last expression
#define DG_CHECK_LAST_GL_ERROR() { GLenum error_no = glGetError(); \
  if (error_no != GL_NO_ERROR) { LogGlError(error_no); DG_UNREACHABLE(); } }
#endif // End of defined(DG_IGNORE_GL_ERROR)

namespace dg {

struct VertexDeclGl2 {
  // Starts from 0. -1 means the end of components
  int index;
  // Number of components per vertex
  GLint num_components;
  // GL_FLOAT or ?
  GLenum data_type;
  // Whether it is to be normalized or not
  GLboolean is_normalized;
  // Byte offsets between the current and the next attribute
  GLsizei stride_bytes;
  // Offset to the first vertex attribute
  const GLvoid* start_offset;
};

struct LockDescGl2 {
  int offset;
  int size;
  uint8_t* buffer;
  GpuLockType flag;

  void Clear() {
    offset = 0;
    size = 0;
    buffer = NULL;
    flag = GpuLockType_None;
  }
};

struct BufferGl2 {
  // Total size of the buffer in bytes
  int size_bytes;
  // Multiple lock is not allowed for now
  LockDescGl2 lock_desc;
};

struct TextureBufferGl2 {
  int width;
  int height;
  int pitch;
  int num_mips;
  GpuFormat format;
  GLint target;
  LockDescGl2 lock_desc;
};

struct RenderTargetGl2 {
  TextureHandle texture;
};

struct UniformVariableGl2 {
  GLint location;
  GLenum data_type;
  String variable_name;
  int texture_unit;
  GLint target;
};

typedef FixedArray<UniformVariableGl2> UniformVariablesGl2;

static void LogGlError(GLenum error_num) {
  switch (error_num) {
  case GL_NO_ERROR:
    DG_LOG_LINE(TXT("error: gl-error: no-error"));
    break;
  case GL_INVALID_ENUM:
    DG_LOG_LINE(TXT("error: gl-error: invalid-enum"));
    break;
  case GL_INVALID_VALUE:
    DG_LOG_LINE(TXT("error: gl-error: invalid-value"));
    break;
  case GL_INVALID_OPERATION:
    DG_LOG_LINE(TXT("error: gl-error: invalid-operation"));
    break;
  case GL_OUT_OF_MEMORY:
    DG_LOG_LINE(TXT("error: gl-error: out-of-memory"));
    break;
  case GL_INVALID_FRAMEBUFFER_OPERATION:
    DG_LOG_LINE(TXT("error: gl-error: invalid-framebuffer-operation"));
    break;
  default:
    DG_LOG_LINE(TXT("error: gl-error: unknown-error: error-code:%x"), error_num);
    break;
  }
}

static GLenum CompareGl2(RenderStateCompareType value) {
  switch (value) {
  case RenderStateCompareType_Never:
    return GL_NEVER;
  case RenderStateCompareType_Less:
    return GL_LESS;
  case RenderStateCompareType_Equal:
    return GL_EQUAL;
  case RenderStateCompareType_LessEqual:
    return GL_LEQUAL;
  case RenderStateCompareType_Greater:
    return GL_GREATER;
  case RenderStateCompareType_NotEqual:
    return GL_NOTEQUAL;
  case RenderStateCompareType_GreaterEqual:
    return GL_GEQUAL;
  case RenderStateCompareType_Always:
    return GL_ALWAYS;
  }
  DG_UNIMPLEMENTED();
  return 0;
}

static GLenum BlendTypeGl2(RenderStateBlendType blend_type) {
  switch (blend_type) {
  case RenderStateBlendType_None:
    break;
  case RenderStateBlendType_Zero:
    return GL_ZERO;
  case RenderStateBlendType_One:
    return GL_ONE;
  case RenderStateBlendType_SrcColor:
    return GL_SRC_COLOR;
  case RenderStateBlendType_DestColor:
    return GL_DST_COLOR;
  case RenderStateBlendType_SrcAlpha:
    return GL_SRC_ALPHA;
  case RenderStateBlendType_DestAlpha:
    return GL_DST_ALPHA;
  case RenderStateBlendType_InvSrcColor:
    return GL_ONE_MINUS_SRC_COLOR;
  case RenderStateBlendType_InvDestColor:
    return GL_ONE_MINUS_DST_COLOR;
  case RenderStateBlendType_InvSrcAlpha:
    return GL_ONE_MINUS_SRC_ALPHA;
  case RenderStateBlendType_InvDestAlpha:
    return GL_ONE_MINUS_DST_ALPHA;
  default:
    DG_UNIMPLEMENTED();
  }
  return GL_ZERO;
}

static GLenum PrimitiveModeGl2(DrawPrimitiveType prim_type) {
  switch (prim_type) {
  case DrawPrimitiveType_PointList:
    return GL_POINTS;
  case DrawPrimitiveType_LineList:
    return GL_LINES;
  case DrawPrimitiveType_LineStrip:
    return GL_LINE_STRIP;
  case DrawPrimitiveType_TriangleList:
    return GL_TRIANGLES;
  case DrawPrimitiveType_TriangleStrip:
    return GL_TRIANGLE_STRIP;
  case DrawPrimitiveType_TriangleFan:
    return GL_TRIANGLE_FAN;
  }
  DG_UNIMPLEMENTED();
  return 0;
}

static GLint TextureAddressGl2(int address_type) {
  switch (address_type) {
  case TextureAddressType_Wrap:
    return GL_REPEAT;
  case TextureAddressType_Mirror:
    return GL_MIRRORED_REPEAT;
  case TextureAddressType_Clamp:
    return GL_CLAMP_TO_EDGE;
  case TextureAddressType_Border:
  case TextureAddressType_MirrorOnce:
    DG_UNIMPLEMENTED();
  }
  return GL_REPEAT;
}

static GLint TextureMagFilterGl2(int filter_type) {
  switch (filter_type) {
  case TextureFilterType_Point:
    return GL_NEAREST;
  case TextureFilterType_Linear:
    return GL_LINEAR;
  }
  return GL_LINEAR;
}

static GLint TextureMinFilterGl2(int filter_type) {
  switch (filter_type) {
  case TextureFilterType_Point:
    return GL_NEAREST;
  case TextureFilterType_Linear:
    return GL_LINEAR_MIPMAP_LINEAR;
  }
  return GL_LINEAR;
}

static GLenum GetTextureFormatGl2(GpuFormat format) {
  switch (format) {
  case GpuFormat_A8:
  case GpuFormat_R32F:
    // GL_LUMINANCE will have RGBA(L, L, L, 1) if the luminance value is L
    // GL_INTENSITY will have RGBA(L, L, L, L)
    return GL_RED;
  case GpuFormat_A32B32G32R32F:
    return GL_RGBA;
  case GpuFormat_A8R8G8B8:
    return GL_RGBA;
  case GpuFormat_R8G8B8:
    return GL_RGB;
  }
  DG_UNIMPLEMENTED();
  return 0;
}

static GLenum GetInternalTextureFormatGl2(GpuFormat format) {
  switch (format) {
  case GpuFormat_A8:
  case GpuFormat_R32F:
    // Return GL_LUMINANCE;
    return GL_RED;
  case GpuFormat_A32B32G32R32F:
    return GL_RGBA32F;
  case GpuFormat_A8R8G8B8:
    return GL_RGBA;
  case GpuFormat_R8G8B8:
    return GL_RGB;
  }
  DG_UNIMPLEMENTED();
  return 0;
}

static GLenum PixelTypeGl2(GpuFormat format) {
  switch (format) {
  case GpuFormat_R32F:
    // #todo GLES2 doesn't support GL_FLOAT texture
    return GL_FLOAT;
  case GpuFormat_A8:
    return GL_UNSIGNED_BYTE;
  case GpuFormat_A32B32G32R32F:
    return GL_FLOAT;
  case GpuFormat_A8R8G8B8:
    return GL_UNSIGNED_BYTE;
  case GpuFormat_R8G8B8:
    return GL_UNSIGNED_BYTE;
  }
  DG_UNIMPLEMENTED();
  return 0;
}

static int GetGpuFormatSize(GpuFormat format) {
  switch (format) {
  case GpuFormat_Unknown:
    return 0;
  case GpuFormat_Index16:
    return 2;
  case GpuFormat_Index32:
    return 4;
  case GpuFormat_R8G8B8:
    return 3;
  case GpuFormat_A8R8G8B8:
  case GpuFormat_X8R8G8B8:
  case GpuFormat_A8B8G8R8:
  case GpuFormat_R32F:
    return 4;
  case GpuFormat_A32B32G32R32F:
    return 16;
  case GpuFormat_A16B16G16R16F:
    return 8;
  case GpuFormat_D24S8:
    return 4;
  case GpuFormat_D16:
    return 2;
  case GpuFormat_A8:
    return 1;
  case GpuFormat_L8:
    return 1;
  }
  return 0;
}


static GLenum GetUsageGl2(GpuUsage usage) {
  GLenum usage_gl2 = GL_STATIC_DRAW;
  switch (usage) {
  case GpuUsage_Dynamic:
    usage_gl2 = GL_DYNAMIC_DRAW;
    break;
  case GpuUsage_None:
  case GpuUsage_WriteOnly:
    usage_gl2 = GL_STATIC_DRAW;
    break;
  default:
    DG_UNIMPLEMENTED();
    usage_gl2 = GL_STATIC_DRAW;
    break;
  }
  return usage_gl2;
}

static GLenum BlendOpGl2(RenderStateBlendOpType value) {
  switch (value) {
  case RenderStateBlendOpType_Add:
    return GL_FUNC_ADD;
  case RenderStateBlendOpType_Subtract:
    return GL_FUNC_SUBTRACT;
  case RenderStateBlendOpType_RevSubtract:
    return GL_FUNC_REVERSE_SUBTRACT;
  case RenderStateBlendOpType_Min:
  case RenderStateBlendOpType_Max:
    // Min/max are not supported in GLES2
    return GL_FUNC_ADD;
  default:
    DG_UNIMPLEMENTED();
    break;
  }
  return GL_FUNC_ADD;
}

static bool CreateBufferGl2(GLenum buffer_type, GpuUsage usage, int size_bytes, IndexBufferHandle& out_buffer_handle) {
  DG_CHECK_GL_ERROR(glGenBuffers(1, out_buffer_handle.GetRef()));
  BufferGl2* custom_buffer = new BufferGl2;
  MyMemSet(custom_buffer, 0, sizeof(*custom_buffer));
  out_buffer_handle.custom_ = (void*)custom_buffer;
  custom_buffer->size_bytes = size_bytes;
  DG_CHECK_GL_ERROR(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, out_buffer_handle.Get()));
  DG_CHECK_GL_ERROR(glBufferData(GL_ELEMENT_ARRAY_BUFFER, size_bytes, NULL, GetUsageGl2(usage)));
  DG_CHECK_GL_ERROR(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
  if (!custom_buffer) return false;
  return true;
}

static bool LockBufferGl(
    GLenum buffer_type, BufferGl2* buffer_gl2, GLuint buffer_handle,
    int offset, int size, uint8_t** out_buffer, GpuLockType lock_flag) {
  Check(buffer_gl2);
  Check(offset == 0); // For now 0 offset only
  Check(size <= buffer_gl2->size_bytes);
  buffer_gl2->lock_desc.offset = offset;
  buffer_gl2->lock_desc.size = size;
  buffer_gl2->lock_desc.flag = lock_flag;
  if (lock_flag == GpuLockType_ReadOnly) {
    DG_CHECK_GL_ERROR(glBindBuffer(buffer_type, buffer_handle));
    {
      buffer_gl2->lock_desc.buffer = *out_buffer = (uint8_t*)glMapBufferRange(buffer_type, offset, size, GL_READ_ONLY);
      Check(*out_buffer);
      DG_CHECK_LAST_GL_ERROR();
    }
    DG_CHECK_GL_ERROR(glBindBuffer(buffer_type, 0));
  } else {
    buffer_gl2->lock_desc.buffer = *out_buffer = new uint8_t[size];
  }
  return true;
}

static bool UnlockBufferGl(GLenum buffer_type, BufferGl2* buffer, GLint buffer_handle) {
  Check(buffer);
  if (buffer->lock_desc.flag == GpuLockType_ReadOnly) {
    DG_CHECK_GL_ERROR(glUnmapBuffer(buffer_type));
    buffer->lock_desc.buffer = NULL;
  } else {
    DG_CHECK_GL_ERROR(glBindBuffer(buffer_type, buffer_handle));
    DG_CHECK_GL_ERROR(glBufferSubData(buffer_type,
        buffer->lock_desc.offset, buffer->lock_desc.size,
        buffer->lock_desc.buffer));
    DG_CHECK_GL_ERROR(glBindBuffer(buffer_type, 0));
    DG_SAFE_DELETE(buffer->lock_desc.buffer);
  }
  return true;
}

bool RC_SetRenderState(RenderStateType type, uint32_t value) {
  DG_LOG_RENDER_CALL(TXT("rc_setrendertsate: type:%d value:%d"), type, value);
  g_renderer.states.SetRenderState(type, value);
  switch (type) {
  case RenderStateType_AlphaTestEnable:
    // In OpenGL ES 2, there is no alpha-testing render state
    // Use "discard" operation in shader
    // @see #no-alpha-testing-support
    {
      static bool is_logged = false;
      if (!is_logged) {
        DG_LOG_LINE(TXT("warning: #no-alpha-testing-support"));
        is_logged = true;
      }
    }
    break;
  case RenderStateType_AlphaBlendEnable:
    (value) ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
    break;
  case RenderStateType_ZEnable:
    (value) ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
    break;
  case RenderStateType_ZWriteEnable:
    (value) ? glDepthMask(GL_TRUE) : glDepthMask(GL_FALSE);
    break;
  case RenderStateType_ScissorTestEnable:
    (value) ? glEnable(GL_SCISSOR_TEST) : glDisable(GL_SCISSOR_TEST);
    break;
  case RenderStateType_CullMode:
    glFrontFace(GL_CW); // CCW is OpenGL default
    if (value == CullModeType_None) {
      glDisable(GL_CULL_FACE);
    } else if (value == CullModeType_CCW) {
      glEnable(GL_CULL_FACE);
      glCullFace(GL_BACK);
    } else if (value == CullModeType_CW) {
      glEnable(GL_CULL_FACE);
      glCullFace(GL_FRONT);
    }
    break;
  case RenderStateType_AlphaRef:
    // @see #no-alpha-testing-support
    {
      static bool is_logged = false;
      if (!is_logged) {
        DG_LOG_LINE(TXT("warning: #no-alpha-testing-support"));
        is_logged = true;
      }
    }
    break;
  case RenderStateType_ZFunc:
    glDepthFunc(CompareGl2((RenderStateCompareType)value));
    break;
  case RenderStateType_AlphaFunc:
    // Do nothing here
    break;
  case RenderStateType_SrcBlend:
  case RenderStateType_DestBlend:
    {
      GLenum src_factor = BlendTypeGl2(g_renderer.states.GetRenderState<RenderStateBlendType>(RenderStateType_SrcBlend));
      GLenum dest_factor = BlendTypeGl2(g_renderer.states.GetRenderState<RenderStateBlendType>(RenderStateType_DestBlend));
      DG_CHECK_GL_ERROR(glBlendFunc(src_factor, dest_factor));
      break;
    }
  case RenderStateType_BlendOp:
    {
      GLenum mode = BlendOpGl2((RenderStateBlendOpType)value);
      DG_CHECK_GL_ERROR(glBlendEquation(mode));
      break;
    }
  case RenderStateType_FillMode:
    break;
  default:
    DG_LOG_LINE(TXT("rc_setrenderstate: not-supported-state:%d"), type); 
    return false;
  }
  return true;
}

bool RC_PresentScreen() {
  DG_LOG_RENDER_CALL(TXT("rc_presentscreen:"));
#if defined(DG_TARGET_ANDROID)
  eglSwapBuffers(g_renderer.display_handle_gl2_, g_renderer.window_surface_gl2_);
#elif defined(DG_TARGET_WIN32)
  wglSwapIntervalEXT(0);
  SwapBuffers(::GetDC(Application::GetWindowHandle()));
#endif
  return true;
}

bool RC_ClearScreen(ColorInt clear_color, bool is_depth_clear) {
  DG_LOG_RENDER_CALL(TXT("RC_ClearScreen: clearcolor:%d isdepthclear:%d"), clear_color, is_depth_clear);
  ColorRGBA clear_color_float;
  clear_color_float.FromColorInt(clear_color);
  DG_CHECK_GL_ERROR(glClearColor(clear_color_float.r, clear_color_float.g, clear_color_float.b, .0f));
  GLbitfield mask = GL_COLOR_BUFFER_BIT;
  if (is_depth_clear) {
    mask |= GL_DEPTH_BUFFER_BIT;
    DG_CHECK_GL_ERROR(glDepthMask(GL_TRUE));
  }
  DG_CHECK_GL_ERROR(glClear(mask));
  return true;
}

bool RC_DrawIndexedPrimitive(
    DrawPrimitiveType primitive_type,
    unsigned int num_primitives) {
  DG_LOG_RENDER_CALL(TXT("rc_drawindexedprimitive: primtype:%d numprims:%d"), primitive_type, num_primitives);
  Check(num_primitives > 0);
#if !defined(DG_USE_32BIT_INDEX)
  Check(num_primitives*3 < 65535);
#endif
  GLenum prim_mode = GL_TRIANGLES;
  GLsizei count = 0;
  switch (primitive_type) {
  case DrawPrimitiveType_LineList:
    prim_mode = GL_LINES;
    count = num_primitives*2;
    break;
  case DrawPrimitiveType_TriangleList:
    prim_mode = GL_TRIANGLES;
    count = num_primitives*3;
    break;
  default:
    DG_UNIMPLEMENTED();
    break;
  }
#if defined(DG_USE_32BIT_INDEX)
  GLenum index_type = GL_UNSIGNED_INT;
#else
  GLenum index_type = GL_UNSIGNED_SHORT;
#endif
  DG_CHECK_GL_ERROR(glDrawElements(prim_mode, count, index_type, (GLvoid*)0));
  return true;
}

bool RC_CreateShaderProgram(
    ShaderProgramHandle& out_shader_program,
    VertexShaderHandle& vertex_shader,
    PixelShaderHandle& pixel_shader) {
  out_shader_program = glCreateProgram();
  GLuint program_id = out_shader_program.Get();
  DG_LOG_RENDER_CALL(TXT("rc_createshaderprogram: shaderprogram:%d vertexshader:%d pixelshader:%d"),
      program_id, vertex_shader.Get(), pixel_shader.Get());
  DG_CHECK_GL_ERROR(glAttachShader(program_id, vertex_shader.Get()));
  DG_CHECK_GL_ERROR(glAttachShader(program_id, pixel_shader.Get()));
  DG_CHECK_GL_ERROR(glLinkProgram(program_id));
  DG_CHECK_GL_ERROR(glUseProgram(program_id));
  GLint num_active_params = -1;
  int texture_unit = 0;
  DG_CHECK_GL_ERROR(glGetProgramiv(program_id, GL_ACTIVE_UNIFORMS, &num_active_params));
  UniformVariablesGl2* variables = new UniformVariablesGl2(num_active_params);
  for (GLint uniform_idx = 0; uniform_idx < num_active_params; ++uniform_idx) {
    const char* param_name = NULL;
    int name_num_chars = -1;
    int variable_size = -1;
    GLenum variable_data_type = GL_ZERO;
    char uniform_name[255];
    DG_CHECK_GL_ERROR(glGetActiveUniform(
        program_id, uniform_idx,
        sizeof(uniform_name)-1, &name_num_chars,
        &variable_size,
        &variable_data_type,
        uniform_name));
    uniform_name[name_num_chars] = 0;
    GLint location = glGetUniformLocation(program_id, uniform_name);
    DG_CHECK_LAST_GL_ERROR();
    UniformVariableGl2 var;
    var.data_type = variable_data_type;
    var.location = location;
    var.texture_unit = -1;
    var.variable_name = String(uniform_name);
    int idx = var.variable_name.IndexOf(TXT('['));
    if (idx >= 0) {
      // In this case, the uniform name ends with [0]
      var.variable_name = var.variable_name.Left(idx);
    }
    if ((var.data_type == GL_SAMPLER_1D) || (var.data_type == GL_SAMPLER_2D) || (var.data_type == GL_SAMPLER_3D)) {
      var.texture_unit = texture_unit++;
      switch (var.data_type) {
        case GL_SAMPLER_1D:
          var.target = GL_TEXTURE_1D;
          break;
        case GL_SAMPLER_2D:
          var.target = GL_TEXTURE_2D;
          break;
        case GL_SAMPLER_3D:
          var.target = GL_TEXTURE_3D;
          break;
      }
    }
    variables->Set(uniform_idx, var);
    //DG_LOG_LINE(TXT("uniform: name:%s location:%d"), variable.variable_name.GetConst(), location);
  }
  out_shader_program.custom_ = reinterpret_cast<void*>(variables);
  return true;
}

bool RC_SetShaderProgram(
    const Cstr* name,
    ShaderProgramHandle& shader_program_handle,
    VertexShaderHandle& vertex_shader,
    PixelShaderHandle& pixel_shader) {
  DG_LOG_RENDER_CALL(TXT("rc_setshaderprogram: name:%s shaderprogram:%d vertexshader:%d pixelshader:%d"),
      name, shader_program_handle.Get(), vertex_shader.Get(), pixel_shader.Get());
  DG_CHECK_GL_ERROR(glUseProgram(shader_program_handle.Get()));
  g_renderer.states.vertex_shader = vertex_shader;
  g_renderer.states.pixel_shader = pixel_shader;
  g_renderer.states.shader_program = shader_program_handle;
  UniformVariablesGl2* vars = (UniformVariablesGl2*)shader_program_handle.custom_;
  if (vars) {
    for (int idx = 0; idx < vars->Count(); ++idx) {
      UniformVariableGl2* var = &vars->Get(idx);
      if (var && (var->texture_unit >= 0)) {
        // If it is a texture sampler, bind with uniform location
        DG_CHECK_GL_ERROR(glUniform1i(var->location, var->texture_unit));
      }
    }
  }
  return true;
}

template<>
void ReleaseGpuResource(ShaderProgramHandle* resource) {
  DG_LOG_RENDER_CALL(TXT("rendercallgl2.ReleaseGpuResource: shaderprogram:%d"), resource->Get());
  DG_CHECK_GL_ERROR(glDeleteProgram(resource->Get()));
  resource->handle_ = NULL;
  UniformVariablesGl2* variables = (UniformVariablesGl2*)resource->custom_;
  if (variables) {
    delete variables;
    resource->custom_ = NULL;
  }
}

template<>
void ReleaseGpuResource(DeviceShaderHandle* resource) {
  DG_LOG_RENDER_CALL(TXT("rendercallgl2.ReleaseGpuResource: shader:%x"), resource);
  DG_CHECK_GL_ERROR(glDeleteShader(resource->Get()));
  resource->handle_ = NULL;
}

template<>
void ReleaseGpuResource(RenderTargetHandle* resource) {
  DG_LOG_RENDER_CALL(TXT("rendercallgl2.ReleaseGpuResource: rendertarget:%x"), resource);
  DG_SAFE_DELETE(resource->custom_);
  DG_CHECK_GL_ERROR(glDeleteFramebuffers(1, resource->GetRef()));
  resource->handle_ = NULL;
}

template<>
void ReleaseGpuResource(TextureHandle* resource) {
  DG_LOG_RENDER_CALL(TXT("rendercallgl2.ReleaseGpuResource: texture:%d"), resource->Get());
  // Only bound textures can be deleted
  if (resource->Get()) {
    DG_CHECK_GL_ERROR(glDeleteTextures(1, resource->GetRef()));
    TextureBufferGl2* buff = (TextureBufferGl2*)resource->custom_;
    if (buff) {
      delete buff;
    }
    resource->handle_ = NULL;
  }
}

template<>
void ReleaseGpuResource(GpuResourceHandleGl<GpuResourceType_Buffer>* resource) {
  DG_LOG_RENDER_CALL(TXT("rendercallgl2.ReleaseGpuResource: buffer:%x"), resource);
  DG_SAFE_DELETE(resource->custom_);
  if (*resource) {
    DG_CHECK_GL_ERROR(glDeleteBuffers(1, resource->GetRef()));
  }
  resource->handle_ = NULL;
}

template<>
void ReleaseGpuResource(SurfaceHandle* resource) {
  DG_LOG_RENDER_CALL(TXT("rendercallgl2.ReleaseGpuResource: surface:%x"), resource);
  Check(resource->custom_ == 0);
  if (*resource) {
    DG_CHECK_GL_ERROR(glDeleteRenderbuffers(1, resource->GetRef()));
  } else {
    DG_LOG_LINE(TXT("warning: invalid-resource"));
  }
  resource->handle_ = NULL;
}

template<>
void ReleaseGpuResource(VertexLayoutHandle* resource) {
  DG_LOG_RENDER_CALL(TXT("rendercallgl2.ReleaseGpuResource: vertexdeclaration:%x"), resource);
  Check(resource);
  if (resource->handle_) {
    VertexDeclGl2* decls = (VertexDeclGl2*)resource->handle_;
    delete decls;
  } else {
    DG_LOG_LINE(TXT("warning: invalid-resource"));
  }
  resource->handle_ = NULL;
}

static void CheckCompileErrorGl2(const Cstr* shader_name, GLuint shader) {
  GLint result;
  DG_CHECK_GL_ERROR(glGetShaderiv(shader, GL_COMPILE_STATUS, &result));
  if (result != GL_TRUE) {
    AnsiStr msg[1024];
    GLsizei length;
    DG_CHECK_GL_ERROR(glGetShaderInfoLog(shader, 1024, &length, msg));
    Cstr msg2[1024];
    string_util::AnsiToUnicode(msg, -1, msg2, 1024);
    DG_LOG_LINE(TXT("error: rendercallgl2.checkcompileerror.failed: shadername:%s msg:\n<<<\n%s>>>"), shader_name, msg2);
    DG_UNREACHABLE();
  }
}

bool RC_CreateVertexShader(const Cstr* name, uint8_t* data, int data_size,
    VertexShaderHandle& out_vertex_shader) {
  out_vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  DG_LOG_RENDER_CALL(TXT("rendercallgl2.createvertexshader: name:%s id:%d"),
      name, out_vertex_shader.Get());
  if (!data) return false;
  DG_CHECK_GL_ERROR(glShaderSource(out_vertex_shader.Get(), 1, (const char**)&data, &data_size));
  DG_CHECK_GL_ERROR(glCompileShader(out_vertex_shader.Get()));
  CheckCompileErrorGl2(name, out_vertex_shader.Get());
  return true;
}

bool RC_SetVertexShader(const Cstr* name, VertexShaderHandle& vertex_shader) {
  DG_LOG_RENDER_CALL(TXT("rc_setvertexshader: name:%s shader:%x"), name, vertex_shader.Get());
  return true;
}

bool RC_CreatePixelShader(const Cstr* name, uint8_t* data, int data_size,
    PixelShaderHandle& out_pixel_shader) {
  out_pixel_shader = glCreateShader(GL_FRAGMENT_SHADER);
  DG_LOG_RENDER_CALL(TXT("rc_createpixelshader: name:%s id:%d"),
      name, out_pixel_shader.Get());
  if (!data) return false;
  if (0) {
    String code((const AnsiStr*)data, 0, data_size);
    const Cstr* code_str = code.GetConst();
    DG_LOG_LINE(TXT("shader: id:%d name:%s\n%s\n"),
        out_pixel_shader.Get(), name, code_str);
  }
  DG_CHECK_GL_ERROR(glShaderSource(out_pixel_shader.Get(), 1, (const char**)&data, &data_size));
  DG_CHECK_GL_ERROR(glCompileShader(out_pixel_shader.Get()));
  CheckCompileErrorGl2(name, out_pixel_shader.Get());
  return true;
}

bool RC_SetPixelShader(const Cstr* name, PixelShaderHandle& pixel_shader) {
  DG_LOG_RENDER_CALL(TXT("rc_setpixelshader: name:%s shader:%x"), name, pixel_shader.Get());
  return true;
}

bool RC_GetShaderConstantStartRegister(const Cstr* name, int* out_start_register) {
  DG_LOG_RENDER_CALL(TXT("rc_getshaderconstantstartregister: name:%s"), name);
  UniformVariablesGl2* variables = (UniformVariablesGl2*)g_renderer.states.shader_program.custom_;
  Check(out_start_register);
  if (!variables) return false;
  UniformVariableGl2* var;
  *out_start_register = -1;
  int num_vars = variables->Count();
  for (int idx = 0; idx < num_vars; ++idx) {
    // Find the variable with name
    var = &variables->Get(idx);
    if (var->variable_name == name) {
      Check(var->location >= 0);
      if (var->texture_unit >= 0) {
        *out_start_register = var->texture_unit;
      } else {
        *out_start_register = var->location;
      }
      break;
    }
  }
  if (*out_start_register == -1) {
    //DG_LOG_LINE(TXT("uniform-not-used: %s "), name, *out_start_register);
  }
  //DG_LOG_LINE(TXT("uniform: %s=%d"), name, *out_start_register);
  return true;
}

bool RC_SetVertexShaderConstantFloat(
    const Cstr* name, int start_register, const float* data, int vector4_count) {
  DG_LOG_RENDER_CALL(TXT("rc_setvertexshaderconstantfloat: name:%s startregister:%d data:%x vercor4count:%d"),
      name, start_register, data, vector4_count);
  Check(start_register >= 0);
  Check(data);
  Check(vector4_count > 0);
  RC_SetPixelShaderConstantFloat(name, start_register, data, vector4_count);
  return true;
}

bool RC_SetVertexShaderConstantMatrix(const Cstr* name, int start_register, const Matrix* data) {
  DG_LOG_RENDER_CALL(TXT("rc_setvertexshaderconstantmatrix: name:%s startregister:%d data:%x"),
      name, start_register, data);
  Check(start_register >= 0);
  Check(data);
  GLboolean is_transpose = GL_TRUE;
  DG_CHECK_GL_ERROR(glUniformMatrix4fv(start_register, 1, is_transpose, data->GetFloatsConst()));
  return true;
}

bool RC_SetPixelShaderConstantFloat(
    const Cstr* name, int start_register, const float* data, int vector4_count) {
  DG_LOG_RENDER_CALL(TXT("rc_setpixelshaderconstantfloat: name:%s startregister:%d data:%x vector4count:%d"),
      name, start_register, data, vector4_count);
  Check(start_register >= 0);
  Check(data);
  DG_CHECK_GL_ERROR(glUniform4fv(start_register, vector4_count, data));
  return true;
}

bool RC_SetPixelShaderConstantMatrix(const Cstr* name, int start_register, const Matrix* data) {
  DG_LOG_RENDER_CALL(TXT("rc_setpixelshaderconstantmatrix: name:%s startregister:%d data:%x"),
      name, start_register, data);
  Check(start_register >= 0);
  Check(data);
  GLboolean is_transpose = GL_TRUE;
  DG_CHECK_GL_ERROR(glUniformMatrix4fv(start_register, 1, is_transpose, data->GetFloatsConst()));
  return true;
}

bool RC_CreateTexture2D(const Cstr* name, TextureHandle& out_texture,
    int width, int height, int mip_levels,
    GpuUsage usage, GpuFormat format, GpuPool pool) {
  DG_CHECK_GL_ERROR(glGenTextures(1, out_texture.GetRef()));
  TextureBufferGl2* custom = new TextureBufferGl2;
  custom->target = GL_TEXTURE_2D;
  custom->width = width;
  custom->height = height;
  custom->format = format;
  custom->num_mips = mip_levels;
  custom->pitch = (width*GetGpuFormatSize(format) + 3) / 4 * 4;
  const int kSizeBytes = custom->pitch*height;
  unsigned char* rgb = new unsigned char[kSizeBytes];
  MyMemSet(rgb, 0, kSizeBytes);
  const GLenum kInternalTextureFormat = GetInternalTextureFormatGl2(format);
  const GLenum kTextureFormat = GetTextureFormatGl2(format);
  const GLenum kPixelType = PixelTypeGl2(format);
  DG_CHECK_GL_ERROR(glBindTexture(GL_TEXTURE_2D, out_texture.Get()));
  GLint level = 0;
  DG_CHECK_GL_ERROR(glTexImage2D(GL_TEXTURE_2D, level, kInternalTextureFormat,
      width, height, 0, // Border
      kTextureFormat, kPixelType, rgb));
  if (mip_levels == 0) {
    if (glGenerateMipmap) DG_CHECK_GL_ERROR(glGenerateMipmap(GL_TEXTURE_2D));
  }
  DG_CHECK_GL_ERROR(glBindTexture(GL_TEXTURE_2D, 0));
  delete [] rgb;
  out_texture.custom_ = (void*)custom;
  DG_LOG_RENDER_CALL(TXT("rc_createtexture2d: name:%s width:%d height:%d miplevels:%d usage:%d format:%d pool:%d outtexture:%x"),
      name, width, height, mip_levels, usage, format, pool, out_texture.Get());
  return true;
}

bool RC_CreateTexture2DFromBlob(
    const Cstr* name,
    TextureHandle& out_texture,
    BlobBase* blob,
    GpuUsage usage,
    GpuPool pool,
    int& out_width,
    int& out_height,
    int& out_mip_levels,
    GpuFormat& out_format) {
  DG_LOG_RENDER_CALL(TXT("rc_createtexture2dfromblob: name:%s blob:%x usage:%d pool:%d"),
      name, usage, pool);
  // Load .tga file
  DG_CHECK_GL_ERROR(glGenTextures(1, out_texture.GetRef()));
  DG_CHECK_GL_ERROR(glBindTexture(GL_TEXTURE_2D, out_texture.Get()));
  GLint internal_format = GL_RGBA;
  out_format = GpuFormat_A8R8G8B8;
  out_mip_levels = 0;
  int num_components = 0;
  unsigned char* rgba = NULL;
  int width = 0, height = 0;
  int is_stbi = false;
  if (stbi_info_from_memory(blob->data_const(), blob->size(), &width, &height, &num_components) == 1) {
    // Load using stbi library for the general image file formats (e.g. jpg, png, gif, psd)
    is_stbi = true;
    out_width = width;
    out_height = height;
    const int kRequiredComponents = 4;
    rgba = stbi_load_from_memory(blob->data_const(), blob->size(),
        &out_width, &out_height, &num_components, kRequiredComponents);
  } else {
    // DGTX format
    texture_util::RawTextureHeader header;
    MyMemCpy(&header, blob->data_const(), sizeof(header));
    Check(MyMemCmp(header.signature, texture_util::RawTextureHeader::GetSignature(), 4) == 0);
    Check(header.storage_format == GpuFormat_A8B8G8R8);
    out_width = header.width;
    out_height = header.height;
    out_mip_levels = header.num_mips;
    rgba = blob->data() + sizeof(header);
  }
  // Update texture
  DG_CHECK_GL_ERROR(glTexImage2D(GL_TEXTURE_2D,
      0, // Level
      internal_format, out_width, out_height,
      0, // Border
      GL_RGBA, GL_UNSIGNED_BYTE, rgba));
  if (out_mip_levels == 0) {
    if (glGenerateMipmap) DG_CHECK_GL_ERROR(glGenerateMipmap(GL_TEXTURE_2D));
  }
  if (is_stbi) stbi_image_free(rgba);
  rgba = 0;
  TextureBufferGl2* custom = new TextureBufferGl2;
  custom->target = GL_TEXTURE_2D;
  custom->width = out_width;
  custom->height = out_height;
  custom->format = out_format;
  custom->num_mips = out_mip_levels;
  custom->pitch = (out_width * GetGpuFormatSize(out_format) + 3) / 4 * 4;
  out_texture.custom_ = (void*)custom;
  return true;
}

bool RC_TextureToBlob(
    const Cstr* name,
    TextureHandle& texture,
    const Cstr* extension,
    BlobBase* blob) {
  DG_LOG_RENDER_CALL(TXT("rc_texturetoblob: name:%s texture:%x extension:%s blob:%x"),
      name, texture.Get(), extension, blob);
  TextureBufferGl2* buff = (TextureBufferGl2*)texture.custom_;
  Check(buff);
  if (buff == 0) {
    DG_LOG_LINE(TXT("rc_texturetoblob.error: no-internal-texture-data"));
    return false;
  }
  int level = 0;
  uint8_t* buffer = 0;
  int pitch = 0;
  texture_util::RawTextureHeader header;
  header.Init(buff->width, buff->height);
  RC_LockTexture(texture, level, buffer, pitch, GpuLockType_ReadOnly);
  header.CopyToBlob(blob, buffer, pitch*header.height);
  RC_UnlockTexture(texture, level);
  return true;
}

static void SetSamplerStateGl(int sampler_target, int sampler_idx, int sampler_state, int value) {
  Check(sampler_target == GL_TEXTURE_2D);
  switch (sampler_state) {
  case SamplerStateType_AddressU:
    DG_CHECK_GL_ERROR(glTexParameteri(sampler_target, GL_TEXTURE_WRAP_S, TextureAddressGl2(value)));
    break;
  case SamplerStateType_AddressV:
    DG_CHECK_GL_ERROR(glTexParameteri(sampler_target, GL_TEXTURE_WRAP_T, TextureAddressGl2(value)));
    break;
  case SamplerStateType_AddressW:
    DG_CHECK_GL_ERROR(glTexParameteri(sampler_target, GL_TEXTURE_WRAP_R, TextureAddressGl2(value)));
    break;
  case SamplerStateType_MipFilter:
    // Mipfilter not supported for now
    break;
  case SamplerStateType_MinFilter:
    DG_CHECK_GL_ERROR(glTexParameteri(sampler_target, GL_TEXTURE_MIN_FILTER, TextureMinFilterGl2(value)));
    break;
  case SamplerStateType_MagFilter:
    DG_CHECK_GL_ERROR(glTexParameteri(sampler_target, GL_TEXTURE_MAG_FILTER, TextureMagFilterGl2(value)));
    break;
  case SamplerStateType_SrgbTexture:
    {
      static bool is_logged = false;
      if (is_logged) {
        DG_LOG_LINE(TXT("warning: srgbtexture not supported"));
        is_logged = true;
      }
    }
    break;
  }
}

bool RC_SetTexture(const Cstr* name, int stage_index, TextureHandle& texture) {
  Check(stage_index >= 0);
  Check(stage_index < RenderStates::kNumTextureSamplerUnits);
  g_renderer.states.textures[stage_index] = texture;
  if (texture) {
    TextureBufferGl2* tex_gl = (TextureBufferGl2*)texture.custom_;
    GLint target = GL_TEXTURE_2D;
    Check(tex_gl);
    target = tex_gl->target;
    DG_CHECK_GL_ERROR(glActiveTexture(GL_TEXTURE0 + stage_index));
    DG_CHECK_GL_ERROR(glBindTexture(target, texture.Get()));
    for (int type_idx = 0; type_idx < SamplerStateType_Max; ++type_idx) {
      int value = g_renderer.states.sampler_states[stage_index][type_idx];
      // #todo states sampler state. setting this every frame is too slow
      SetSamplerStateGl(target, stage_index, type_idx, value);
    }
  }
  DG_LOG_RENDER_CALL(TXT("rc_settexture: name:%s stage_index:%d texture:%d"),
      name, stage_index, texture.Get());
  return true;
}

bool RC_CreateIndexBuffer(
    int size_bytes, GpuUsage usage,
    GpuPool pool, IndexBufferHandle& out_index_buffer) {
  DG_LOG_RENDER_CALL(TXT("rc_createindexbuffer: sizebytes:%d usage:%d pool:%d"),
      size_bytes, usage, pool);
  if (!CreateBufferGl2(GL_ARRAY_BUFFER, usage, size_bytes, out_index_buffer)) {
    return false;
  }
  return true;
}

bool RC_SetIndices(IndexBufferHandle& index_buffer) {
  DG_LOG_RENDER_CALL(TXT("rc_setindices: indexbuffer:%x"), index_buffer.Get());
  DG_CHECK_GL_ERROR(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer.Get()));
  return true;
}

bool RC_GetRenderTarget(int target_index, RenderTargetHandle& out_render_target) {
  DG_LOG_RENDER_CALL(TXT("rc_getrendertarget: targetindex:%d"), target_index);
  // OpenGL default render target handle is 0
  out_render_target = NULL;
  return true;
}

static bool CheckFrameBufferStatusSlowDoNotCallEveryFrame() {
  GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  switch(status) {
    case GL_FRAMEBUFFER_COMPLETE:
      return true;
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
      DG_LOG_LINE(TXT("checkframebufferstatus: incomplete-attachment"));
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
      DG_LOG_LINE(TXT("checkframebufferstatus: incomplete-missing-attachment"));
      break;
#if defined(DG_TARGET_ANDROID)
    case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
      DG_LOG_LINE(TXT("checkframebufferstatus: incomplete-dimensions"));
      break;
#endif
    case GL_FRAMEBUFFER_UNSUPPORTED:
      DG_LOG_LINE(TXT("checkframebufferstatus: unsupported"));
      break;
    default:
      DG_LOG_LINE(TXT("checkframebufferstatus: unknown"));
      break;
  }
  return false;
}

bool RC_SetRenderTargetWithDepth(
    int target_index,
    RenderTargetHandle& render_target,
    SurfaceHandle& depth_surface) {
  DG_LOG_RENDER_CALL(TXT("rc_setrendertargetwithdepth: targetindex:%d target:%x depth:%x"),
      target_index, render_target.Get(), depth_surface.Get());
  DG_CHECK_GL_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, render_target.Get()));
  RenderTargetGl2* target_gl = reinterpret_cast<RenderTargetGl2*>(render_target.custom_);
  if (target_gl) {
    TextureBufferGl2* tex_gl = (TextureBufferGl2*)target_gl->texture.custom_;
    DG_CHECK_GL_ERROR(glBindTexture(tex_gl->target, target_gl->texture.Get()));
    int level = 0;
    DG_CHECK_GL_ERROR(glFramebufferTexture2D(GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, target_gl->texture.Get(), level));
    DG_CHECK_GL_ERROR(glBindTexture(tex_gl->target, 0));
  }
  if (depth_surface) {
    DG_CHECK_GL_ERROR(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_surface.Get()));
  }
  if (g_renderer.frame_id() == 0) {
    Check(CheckFrameBufferStatusSlowDoNotCallEveryFrame());
  }
  return true;
}

bool RC_CreateDepthStencilSurface(
    int width,
    int height,
    GpuFormat depth_format,
    int multi_sample_type,
    int multi_sample_quality,
    SurfaceHandle& out_surface) {
  DG_LOG_RENDER_CALL(TXT("rc_createdepthstencilsurface: width:%d height:%d depthformat:%d multisampletype:%d multisamplequality:%d"),
      width, height, depth_format, multi_sample_type, multi_sample_quality);
  DG_CHECK_GL_ERROR(glGenRenderbuffers(1, out_surface.GetRef()));
  DG_CHECK_GL_ERROR(glBindRenderbuffer(GL_RENDERBUFFER, out_surface.Get()));
  // #todo check first if it supports D24S8 (GL_DEPTH24_STENCIL8_OES)
  //GLint params;
  //glGetRenderbufferParameterivEXT(GL_RENDERBUFFER, GL_RENDERBUFFER_INTERNAL_FORMAT, &params);
  if (depth_format == GpuFormat_D24S8) {
    //DG_CHECK_GL_ERROR(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8_EXT, width, height));
    DG_CHECK_GL_ERROR(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height));
  } else {
    DG_CHECK_GL_ERROR(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height));
  }
  DG_CHECK_GL_ERROR(glBindRenderbuffer(GL_RENDERBUFFER, 0));
  return true;
}

bool RC_GetDepthStencilSurface(SurfaceHandle& out_depth_surface) {
  DG_LOG_RENDER_CALL(TXT("rc_getdepthstencilsurface:"));
  out_depth_surface = NULL;
  return true;
}

bool RC_SetSamplerState(int stage_index, SamplerStateType sampler_state, int value) {
  DG_LOG_RENDER_CALL(TXT("rc_setsamplerstate: stageindex:%d samplerstate:%d value:%d"),
      stage_index, sampler_state, value);
  g_renderer.states.SetSamplerState(stage_index, sampler_state, value);
  return true;
}

bool RC_SetStreamSource(
    int stream_index, VertexBufferHandle& vertex_buffer,
    int stream_offset_in_bytes, int stride) {
  DG_LOG_RENDER_CALL(TXT("rc_setstreamsource: streamindex:%d vertexbuffer:%x streamoffsetinbytes:%d stride:%d"),
      stream_index, vertex_buffer.Get(), stream_offset_in_bytes, stride);
  DG_CHECK_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer.Get()));
  return true;
}

bool RC_CreateVertexBuffer(
    int size_bytes,
    GpuUsage usage,
    GpuPool pool,
    VertexBufferHandle& out_vertex_shader) {
  DG_LOG_RENDER_CALL(TXT("rc_createvertexbuffer: size_bytes:%d usage:%d pool:%d"),
      size_bytes, usage, pool);
  if (!CreateBufferGl2(GL_ARRAY_BUFFER, usage, size_bytes, out_vertex_shader)) {
    return false;
  }
  return true;
}

bool RC_LockVertexBuffer(
    VertexBufferHandle& vertex_buffer,
    int offset,
    int size,
    uint8_t** out_buffer,
    GpuLockType lock_flag) {
  DG_LOG_RENDER_CALL(TXT("rc_lockvertexbuffer: vertexbuffer:%x size:%d lockflag:%d"),
      vertex_buffer.Get(), size, lock_flag);
  return LockBufferGl(GL_ARRAY_BUFFER,
      (BufferGl2*)vertex_buffer.custom_,
      vertex_buffer.Get(), offset, size, out_buffer, lock_flag);
}

bool RC_UnlockVertexBuffer(VertexBufferHandle& vertex_buffer) {
  DG_LOG_RENDER_CALL(TXT("rc_unlockvertexbuffer: vertexbuffer:%x"), vertex_buffer.Get());
  return UnlockBufferGl(GL_ARRAY_BUFFER,
      (BufferGl2*)vertex_buffer.custom_,
      vertex_buffer.Get());
}

bool RC_LockIndexBuffer(
    IndexBufferHandle& index_buffer,
    int offset,
    int size,
    uint8_t** out_buffer,
    GpuLockType lock_flag) {
  DG_LOG_RENDER_CALL(TXT("rc_lockindexbuffer: offset: %dsize:%d lockflag:%d"),
      index_buffer.Get(), offset, size, lock_flag);
  return LockBufferGl(GL_ELEMENT_ARRAY_BUFFER,
      (BufferGl2*)index_buffer.custom_,
      index_buffer.Get(), offset, size, out_buffer, lock_flag);
}

bool RC_UnlockIndexBuffer(IndexBufferHandle& index_buffer) {
  DG_LOG_RENDER_CALL(TXT("rc_unlockindexbuffer: indexbuffer:%x"), index_buffer.Get());
  return UnlockBufferGl(GL_ELEMENT_ARRAY_BUFFER,
      (BufferGl2*)index_buffer.custom_,
      index_buffer.Get());
}

bool RC_LockTexture(
    TextureHandle& texture,
    int level,
    uint8_t*& out_buffer,
    int& out_pitch,
    GpuLockType lock_flag) {
  DG_LOG_RENDER_CALL(TXT("rc_locktexture: texture:%x level:%d lockflag:%d"),
      texture.Get(), level, lock_flag);
  TextureBufferGl2* texture_gl2 = (TextureBufferGl2*)texture.custom_;
  Check(texture_gl2);
  out_pitch = texture_gl2->pitch;
  const int kSizeBytes = texture_gl2->pitch * texture_gl2->height;
  Check(kSizeBytes > 0);
  texture_gl2->lock_desc.size = kSizeBytes;
  texture_gl2->lock_desc.offset = 0;
  texture_gl2->lock_desc.flag = lock_flag;
  texture_gl2->lock_desc.buffer = out_buffer = new uint8_t[kSizeBytes];
  if (lock_flag == GpuLockType_ReadOnly) {
    glBindTexture(texture_gl2->target, texture.Get());
    glGetTexImage(texture_gl2->target,
      0, // level
      GL_RGBA,
      GL_UNSIGNED_BYTE,
      out_buffer);
    if (texture_gl2->width*4 == texture_gl2->pitch) {
      MyMemCpy(out_buffer, out_buffer, kSizeBytes);
    } else {
      DG_UNIMPLEMENTED();
    }
  } else {
    // For discard, do nothing here
  }
  return true;
}

bool RC_UnlockTexture(TextureHandle& texture, int level) {
  DG_LOG_RENDER_CALL(TXT("rc_unlocktexture: texture:%x level:%d"),
      texture.Get(), level);
  TextureBufferGl2* texture_gl2 = (TextureBufferGl2*)texture.custom_;
  Check(level == 0);
  Check(texture_gl2);
  if (texture_gl2->lock_desc.flag == GpuLockType_ReadOnly) {
    DG_SAFE_DELETE(texture_gl2->lock_desc.buffer);
  } else {
    Check(texture_gl2->lock_desc.size == (texture_gl2->width * texture_gl2->height * GetGpuFormatSize(texture_gl2->format)));
    GLenum format = GetTextureFormatGl2(texture_gl2->format);
    GLenum internal_format = GetInternalTextureFormatGl2(texture_gl2->format);
    DG_CHECK_GL_ERROR(glBindTexture(texture_gl2->target, texture.Get()));
    DG_CHECK_GL_ERROR(glTexImage2D(texture_gl2->target, level, internal_format,
        texture_gl2->width, texture_gl2->height,
        0, // Border
        format, GL_UNSIGNED_BYTE,
        texture_gl2->lock_desc.buffer));
    DG_SAFE_DELETE(texture_gl2->lock_desc.buffer);
  }
  texture_gl2->lock_desc.Clear();
  return true;
}

bool RC_CreateOffscreenPlainSurface(
    SurfaceHandle& out_surface, int width, int height, GpuFormat format) {
  DG_LOG_RENDER_CALL(TXT("rc_createoffscreenplainsurface: width:%d height:%d format:%d"),
      width, height, format);
  DG_UNIMPLEMENTED_SKIP();
  return true;
}

bool RC_GetRenderTargetData(RenderTargetHandle& render_target, SurfaceHandle& lockable_surface) {
  DG_LOG_RENDER_CALL(TXT("rc_getrendertargetdata: rendertarget:%x lockablesurface:%x"),
      render_target.Get(), lockable_surface.Get());
  DG_UNIMPLEMENTED_SKIP();
  return true;
}

bool RC_LockSurfarce(SurfaceHandle& lockable_surface, uint8_t*& out_buffer, int& out_pitch) {
  DG_LOG_RENDER_CALL(TXT("rc_locksurfarce: lockablesurface:%x"), lockable_surface.Get());
  DG_UNIMPLEMENTED_SKIP();
  return true;
}

bool RC_UnlockSurface(SurfaceHandle& lockable_surface) {
  DG_LOG_RENDER_CALL(TXT("rc_unlocksurface: lockablesurface:%x"), lockable_surface.Get());
  DG_UNIMPLEMENTED_SKIP();
  return true;
}

bool RC_CreateRenderTargetFromTexture(
    TextureHandle& texture, int level, RenderTargetHandle& out_render_target) {
  DG_CHECK_GL_ERROR(glGenFramebuffers(1, out_render_target.GetRef()));
  Check(out_render_target.custom_ == 0);
  RenderTargetGl2* custom = new RenderTargetGl2;
  custom->texture = texture;
  out_render_target.custom_ = custom;
  DG_LOG_RENDER_CALL(TXT("rc_createrendertargetfromtexture: texture:%x level:%d outrendertarget:%x"),
      texture.Get(), level, out_render_target.Get());
  return true;
}

bool RC_SetViewport(Viewport& viewport) {
  DG_LOG_RENDER_CALL(TXT("rc_setviewport: top_left_x:%d top_left_y:%d width:%d height:%d min_depth:%d max_depth:%d"),
      viewport.top_left_x, viewport.top_left_y, viewport.width, viewport.height, viewport.min_depth, viewport.max_depth);
  Check(viewport.top_left_y == 0); // Should be flipped for gl
  DG_CHECK_GL_ERROR(glViewport(viewport.top_left_x, viewport.top_left_y, viewport.width, viewport.height));
  if (glDepthRange) {
    DG_CHECK_GL_ERROR(glDepthRange(viewport.min_depth, viewport.max_depth));
  } else {
    return false;
  }
  return true;
}

static VertexDeclGl2* VertexDeclCreateElementGl2(
    const VertexElement* elements, int element_count) {
  if (elements == 0) {
    return 0;
  }
  VertexDeclGl2* elems_gl2 = new VertexDeclGl2[element_count + 1];
  VertexDeclGl2* elem_gl2 = NULL;
  int offset = 0;
  int elem_idx = 0;
  const bool is_logging = false;
  if (is_logging) DG_LOG(TXT("vertexdeclcreateelementgl2: num_elems = %d\n"), element_count);
  MyMemSet(elems_gl2, 0, sizeof(VertexDeclGl2)*(element_count+1));
  for (elem_idx = 0; elem_idx < element_count; ++elem_idx) {
    const VertexElement& elem = elements[elem_idx];
    elem_gl2 = &elems_gl2[elem_idx];
    elems_gl2[elem_idx].index = elem_idx;
    elem_gl2->start_offset = (const GLvoid*)offset;
    switch (elem.data_type) {
    case VertexElement::DataType_Float1:
      elem_gl2->num_components = 1;
      elem_gl2->data_type = GL_FLOAT;
      elem_gl2->is_normalized = GL_FALSE;
      break;
    case VertexElement::DataType_Float2:
      elem_gl2->num_components = 2;
      elem_gl2->data_type = GL_FLOAT;
      elem_gl2->is_normalized = GL_FALSE;
      break;
    case VertexElement::DataType_Float3:
      elem_gl2->num_components = 3;
      elem_gl2->data_type = GL_FLOAT;
      elem_gl2->is_normalized = GL_FALSE;
      break;
    case VertexElement::DataType_Float4:
      elem_gl2->num_components = 4;
      elem_gl2->data_type = GL_FLOAT;
      elem_gl2->is_normalized = GL_FALSE;
      break;
    case VertexElement::DataType_Color:
      elem_gl2->num_components = 4;
      elem_gl2->data_type = GL_UNSIGNED_BYTE;
      elem_gl2->is_normalized = GL_TRUE;
      break;
    default:
      DG_UNREACHABLE();
      break;
    }
    if (is_logging) {
      DG_LOG(TXT("\tidx:%d type:%d num_comp:%d is_norm:%d offset:%d\n"),
          elem_idx, elem.data_type, elem_gl2->num_components, elem_gl2->is_normalized, offset);
    }
    offset += elem.GetElementSize();
    switch (elem.usage) {
    case VertexElement::Usage_Position:
      break;
    case VertexElement::Usage_Color:
      break;
    case VertexElement::Usage_Normal:
      break;
    case VertexElement::Usage_TexCoord:
      break;
    case VertexElement::Usage_Tangent:
      break;
    case VertexElement::Usage_Binormal:
      break;
    case VertexElement::Usage_JointIndex:
      break;
    case VertexElement::Usage_JointWeight:
      break;
    default:
      DG_UNREACHABLE();
    }
  }
  // Set the stride after evaluating all elements
  for (elem_idx = 0; elem_idx < element_count; ++elem_idx) {
    elems_gl2[elem_idx].stride_bytes = offset;
  }
  elems_gl2[elem_idx].index = -1;
  return elems_gl2;
}

bool RC_CreateVertexDeclaration(
    const struct VertexElement* elements, int elements_count,
    VertexLayoutHandle& out_vertex_decl) {
  DG_LOG_RENDER_CALL(TXT("rc_createvertexdeclaration: elements:%x elementscount:%d"),
      elements, elements_count);
  VertexDeclGl2* elems_gl2 = VertexDeclCreateElementGl2(elements, elements_count);
  out_vertex_decl.handle_ = (VertexDeclGl2*)elems_gl2;
  return true;
}

bool RC_SetVertexDeclaration(VertexLayoutHandle& vertex_decl) {
  DG_LOG_RENDER_CALL(TXT("rc_setvertexdeclaration: ptr:%x"), vertex_decl.Get());
  g_renderer.states.vertex_decl = vertex_decl;
  // Attribute array limit = 16 for OGLES2
  const int kMaxVertexArrayIndex = 16;
  static int s_attrib_state[kMaxVertexArrayIndex] = { 0, }; // 1 : enabled, 0 : disabled
  VertexDeclGl2* decls = (VertexDeclGl2*)vertex_decl.Get();
  if (!decls) {
    return false;
  }
  int offset = 0;
  bool is_valid = true;
  static int s_max_vertex_array_index = 0;
  if (s_max_vertex_array_index == 0) {
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &s_max_vertex_array_index);
  }
  Check(s_max_vertex_array_index >= kMaxVertexArrayIndex);
  for (int idx = 0; idx < kMaxVertexArrayIndex; ++idx) {
    if (is_valid && (decls[idx].index < 0)) is_valid = false;
    if (is_valid) {
      Check(idx == decls[idx].index);
      Check(decls[idx].num_components > 1);
      DG_CHECK_GL_ERROR(glVertexAttribPointer(
          decls[idx].index,
          decls[idx].num_components,
          decls[idx].data_type,
          decls[idx].is_normalized,
          decls[idx].stride_bytes,
          decls[idx].start_offset));
      DG_CHECK_GL_ERROR(glEnableVertexAttribArray(idx));
      s_attrib_state[idx] = 1;
      // #todo make sure the glBindAttribLocation() is called correctly in shader compilation process
    } else {
      if (s_attrib_state[idx]) {
        DG_CHECK_GL_ERROR(glDisableVertexAttribArray(idx));
      } // Else, already disabled
      s_attrib_state[idx] = 0;
    }
  }
  return true;
}

bool RC_SetScissorRect(int x, int y, int width, int height) {
  DG_LOG_RENDER_CALL(TXT("RC_SetScissorRect: %d %d %d %d"), x, y, width, height);
  int target_height = 0;
  if (g_renderer.render_target) target_height = g_renderer.render_target->size().y;
  int lower_y = target_height - y - height;
  DG_CHECK_GL_ERROR(glScissor(x, lower_y, width, lower_y + height));
  return true;
}

} // namespace dg
