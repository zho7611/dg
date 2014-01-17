// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

void RenderStates::Apply() {
  RC_SetRenderState(RenderStateType_ZWriteEnable, render_states[RenderStateType_ZWriteEnable]);
  RC_SetRenderState(RenderStateType_ZEnable, render_states[RenderStateType_ZEnable]);
  RC_SetRenderState(RenderStateType_AlphaBlendEnable, render_states[RenderStateType_AlphaBlendEnable]);
  RC_SetRenderState(RenderStateType_AlphaTestEnable, render_states[RenderStateType_AlphaTestEnable]);
  RC_SetRenderState(RenderStateType_ZFunc, render_states[RenderStateType_ZFunc]);
  RC_SetRenderState(RenderStateType_CullMode, render_states[RenderStateType_CullMode]);
  RC_SetRenderState(RenderStateType_FillMode, render_states[RenderStateType_FillMode]);
  RC_SetRenderState(RenderStateType_AlphaFunc, render_states[RenderStateType_AlphaFunc]);
  RC_SetRenderState(RenderStateType_AlphaRef, render_states[RenderStateType_AlphaRef]);
  RC_SetRenderState(RenderStateType_SrcBlend, render_states[RenderStateType_SrcBlend]);
  RC_SetRenderState(RenderStateType_DestBlend, render_states[RenderStateType_DestBlend]);
  RC_SetRenderState(RenderStateType_BlendOp, render_states[RenderStateType_BlendOp]);
  RC_SetRenderState(RenderStateType_ScissorTestEnable, render_states[RenderStateType_ScissorTestEnable]);
  for (int i = 0; i < kNumTextureSamplerUnits; ++i) {
    for (int j = SamplerStateType_None+1; j < SamplerStateType_Max; ++j) {
      RC_SetSamplerState(i, (SamplerStateType)j, sampler_states[i][j]);
    }
    RC_SetTexture(NULL, i, textures[i]);
  }
  RC_SetVertexDeclaration(vertex_decl);
  RC_SetShaderProgram(TXT("renderstatecacheapply"), shader_program, vertex_shader, pixel_shader);
}

void RenderStates::Reset() {
  // Clear with not-set value
  for (int i = 0; i < RenderStateType_Max; ++i) {
    render_states[i] = -1; // -1 = not-set
  }
  render_states[RenderStateType_ZWriteEnable] = 1;
  render_states[RenderStateType_ZEnable] = 1;
  render_states[RenderStateType_AlphaBlendEnable] = 0;
  render_states[RenderStateType_AlphaTestEnable] = 0;
  render_states[RenderStateType_ZFunc] = RenderStateCompareType_Greater;
  render_states[RenderStateType_CullMode] = CullModeType_CCW;
  render_states[RenderStateType_FillMode] = FillModeType_Solid;
  render_states[RenderStateType_AlphaFunc] = RenderStateCompareType_GreaterEqual;
  render_states[RenderStateType_AlphaRef] = 127;
  render_states[RenderStateType_SrcBlend] = RenderStateBlendType_Zero;
  render_states[RenderStateType_DestBlend] = RenderStateBlendType_Zero;
  render_states[RenderStateType_BlendOp] = RenderStateBlendOpType_Add;
  render_states[RenderStateType_ScissorTestEnable] = 0;
  for (int i = 0; i < kNumTextureSamplerUnits; ++i) {
    for (int j = 0; j < SamplerStateType_Max; ++j) {
      sampler_states[i][j] = -1; // -1 = not-set
    }
    sampler_states[i][SamplerStateType_AddressU] = TextureAddressType_Wrap;
    sampler_states[i][SamplerStateType_AddressV] = TextureAddressType_Wrap;
    sampler_states[i][SamplerStateType_AddressW] = TextureAddressType_Wrap;
    sampler_states[i][SamplerStateType_BorderColor] = 0;
    sampler_states[i][SamplerStateType_MagFilter] = TextureFilterType_Point;
    sampler_states[i][SamplerStateType_MinFilter] = TextureFilterType_Point;
    sampler_states[i][SamplerStateType_MipFilter] = TextureFilterType_None;
    sampler_states[i][SamplerStateType_MipmapLodBias] = 0;
    sampler_states[i][SamplerStateType_MaxMipLevel] = 0; // 0 is the largest mip level. value range from 0 to (n-1)
    sampler_states[i][SamplerStateType_MaxAnisotropy] = 1; ;
    sampler_states[i][SamplerStateType_SrgbTexture] = 0;
    sampler_states[i][SamplerStateType_ElementIndex] = 0;
    sampler_states[i][SamplerStateType_DmapOffset] = 0;
    textures[i] = NULL;
  }
  vertex_decl = NULL;
  shader_program = NULL;
  vertex_shader = NULL;
  pixel_shader = NULL;
}

template<>
bool RenderState_AlphaBlendEnable::Apply() {
  return RC_SetRenderState(RenderStateType_AlphaBlendEnable, value_);
}

template<>
bool RenderState_AlphaTestEnable::Apply() {
  return RC_SetRenderState(RenderStateType_AlphaTestEnable, value_);
}

template<>
bool RenderState_ScissorTestEnable::Apply() {
  return RC_SetRenderState(RenderStateType_ScissorTestEnable, value_);
}

template<>
bool RenderState_ZEnable::Apply() {
  return RC_SetRenderState(RenderStateType_ZEnable, value_);
}

template<>
bool RenderState_ZWriteEnable::Apply() {
  return RC_SetRenderState(RenderStateType_ZWriteEnable, value_);
}

template<>
bool RenderState_CullMode::Apply() {
  return RC_SetRenderState(RenderStateType_CullMode, value_);
}

template<>
bool RenderState_ZFunc::Apply() {
  return RC_SetRenderState(RenderStateType_ZFunc, value_);
}

template<>
bool RenderState_SrcBlend::Apply() {
  return RC_SetRenderState(RenderStateType_SrcBlend, value_);
}

template<>
bool RenderState_DestBlend::Apply() {
  return RC_SetRenderState(RenderStateType_DestBlend, value_);
}

template<>
bool RenderState_BlendOp::Apply() {
  return RC_SetRenderState(RenderStateType_BlendOp, value_);
}

template<>
bool RenderState_AlphaFunc::Apply() {
  return RC_SetRenderState(RenderStateType_AlphaFunc, value_);
}

template<>
bool RenderState_AlphaRef::Apply() {
  return RC_SetRenderState(RenderStateType_AlphaRef, value_);
}

} // namespace dg
