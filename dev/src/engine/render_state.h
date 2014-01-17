// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

// Same as DX9 D3DCULL
enum CullModeType {
  CullModeType_None = 1,
  CullModeType_CW = 2,
  CullModeType_CCW = 3,
  CullModeType_Max,
};

// Same as DX9 D3DFILLMODE
enum FillModeType {
  FillModeType_Point = 1,
  FillModeType_Wireframe = 2,
  FillModeType_Solid = 3,
  FillModeType_Max,
};

// Same as DX9 D3DRENDERSTATETYPE
enum RenderStateType {
  RenderStateType_None = 0,
  RenderStateType_ZEnable = 7,
  RenderStateType_FillMode = 8,
  RenderStateType_ZWriteEnable = 14,
  RenderStateType_AlphaTestEnable = 15,
  RenderStateType_AlphaBlendEnable = 27,
  RenderStateType_SrcBlend = 19,
  RenderStateType_DestBlend = 20,
  RenderStateType_CullMode = 22,
  RenderStateType_ZFunc = 23,
  RenderStateType_AlphaRef = 24,
  RenderStateType_AlphaFunc = 25,
  RenderStateType_BlendOp = 171,
  RenderStateType_ScissorTestEnable = 174,
  RenderStateType_Max,
};

// Same as DX9 D3DBLEND
enum RenderStateBlendType {
  RenderStateBlendType_None = 0,
  RenderStateBlendType_Zero,
  RenderStateBlendType_One,
  RenderStateBlendType_SrcColor,
  RenderStateBlendType_InvSrcColor,
  RenderStateBlendType_SrcAlpha,
  RenderStateBlendType_InvSrcAlpha,
  RenderStateBlendType_DestAlpha,
  RenderStateBlendType_InvDestAlpha,
  RenderStateBlendType_DestColor,
  RenderStateBlendType_InvDestColor,
  RenderStateBlendType_SrcAlphaSat,
  RenderStateBlendType_BothSrcAlpha,
  RenderStateBlendType_BothInvSrcAlpha,
  RenderStateBlendType_BlendFactor,
  RenderStateBlendType_InvBlendFactor,
  RenderStateBlendType_SrcColor2,
  RenderStateBlendType_InvSrcColor2,
  RenderStateBlendType_Max,
};

// Same as DX9 D3DBLENDOP
enum RenderStateBlendOpType {
  RenderStateBlendOpType_None = 0,
  RenderStateBlendOpType_Add,
  RenderStateBlendOpType_Subtract,
  RenderStateBlendOpType_RevSubtract,
  RenderStateBlendOpType_Min,
  RenderStateBlendOpType_Max,
};

// Same as DX9 D3DCMPFUNC
enum RenderStateCompareType {
  RenderStateCompareType_None = 0,
  RenderStateCompareType_Never,
  RenderStateCompareType_Less,
  RenderStateCompareType_Equal,
  RenderStateCompareType_LessEqual,
  RenderStateCompareType_Greater,
  RenderStateCompareType_NotEqual,
  RenderStateCompareType_GreaterEqual,
  RenderStateCompareType_Always,
  RenderStateCompareType_Max,
};

// Same as DX9 D3DSAMPLERSTATETYPE
enum SamplerStateType {
  SamplerStateType_None = 0,
  SamplerStateType_AddressU = 1,
  SamplerStateType_AddressV = 2,
  SamplerStateType_AddressW = 3,
  SamplerStateType_BorderColor = 4,
  SamplerStateType_MagFilter = 5,
  SamplerStateType_MinFilter = 6,
  SamplerStateType_MipFilter = 7,
  SamplerStateType_MipmapLodBias = 8,
  SamplerStateType_MaxMipLevel = 9,
  SamplerStateType_MaxAnisotropy = 10,
  SamplerStateType_SrgbTexture = 11,
  SamplerStateType_ElementIndex = 12,
  SamplerStateType_DmapOffset = 13,
  SamplerStateType_Max,
};

// Same as DX9 D3DTEXTUREFILTERTYPE
enum TextureFilterType {
  TextureFilterType_None = 0,
  TextureFilterType_Point = 1,
  TextureFilterType_Linear = 2,
  TextureFilterType_Anisotropic = 3,
  TextureFilterType_PyramidalQuad = 6,
  TextureFilterType_GaussianQuad = 7,
  TextureFilterType_ConvolutionMono = 8,
  TextureFilterType_Max,
};

// Same as DX9 D3DTEXTUREADDRESS
enum TextureAddressType {
  TextureAddressType_None = 0,
  TextureAddressType_Wrap = 1,
  TextureAddressType_Mirror = 2,
  TextureAddressType_Clamp = 3,
  TextureAddressType_Border = 4,
  TextureAddressType_MirrorOnce = 5,
  TextureAddressType_Max,
};

// Stores all render states in one structure
class RenderStates {
public:
  static const int kNumTextureSamplerUnits = 16;
  int render_states[RenderStateType_Max];
  int sampler_states[kNumTextureSamplerUnits][SamplerStateType_Max];
  
  TextureHandle textures[kNumTextureSamplerUnits];
  ShaderProgramHandle shader_program;
  VertexShaderHandle vertex_shader;
  PixelShaderHandle pixel_shader;
  VertexLayoutHandle vertex_decl;

  RenderStates() {
    Reset();
  }
  ~RenderStates() {
    // Not to actually release the holding resources
    Reset();
  }
  // Make it effect on the device with the current setting
  void Apply();
  void Reset();
  template <typename T>
  T GetRenderState(RenderStateType type) const {
    return *(T*)(&render_states[type]);
  }
  template <typename T>
  void SetRenderState(RenderStateType type, T value) {
    *(T*)(&render_states[type]) = value;
  }
  template <typename T>
  T GetSamplerState(int index, SamplerStateType type) const {
    Check(index < kNumTextureSamplerUnits);
    Check(type < SamplerStateType_Max);
    return *(T*)(&sampler_states[index][type]);
  }
  template <typename T>
  void SetSamplerState(int index, SamplerStateType type, T value) {
    Check(index < kNumTextureSamplerUnits);
    Check(type < SamplerStateType_Max);
    *(T*)(&sampler_states[index][type]) = value;
  }
};

class RenderStateBase : public RefCount {
public:
  virtual RenderStateType GetType() = 0;
  virtual bool Apply() = 0;
  virtual void ReadFrom(Stream* stream) = 0;
  virtual void WriteTo(Stream* out_stream) = 0;
};

template<RenderStateType StateType, class ValueType>
class RenderState : public RenderStateBase {
public:
  RenderState() {}
  RenderState(ValueType Value) {
    value_ = Value;
  }
  RenderStateType GetType() {
    return StateType;
  }
  RenderState<StateType, ValueType>& operator =(const RenderState<StateType, ValueType>& rhs) {
    if (this != &rhs) {
      value_ = rhs.value_;
    }
    return *this;
  }
  virtual bool Apply();
  virtual void ReadFrom(Stream* stream) {
    StreamReadFlat(stream, value_);
  }
  virtual void WriteTo(Stream* out_stream) {
    StreamWriteFlat(out_stream, value_);
  }

  ValueType value_;
};

typedef RenderState<RenderStateType_AlphaBlendEnable, bool> RenderState_AlphaBlendEnable;
typedef RenderState<RenderStateType_AlphaTestEnable, bool> RenderState_AlphaTestEnable;
typedef RenderState<RenderStateType_ZEnable, bool> RenderState_ZEnable;
typedef RenderState<RenderStateType_ZWriteEnable, bool> RenderState_ZWriteEnable;
typedef RenderState<RenderStateType_ScissorTestEnable, bool> RenderState_ScissorTestEnable;
typedef RenderState<RenderStateType_ZFunc, RenderStateCompareType> RenderState_ZFunc;
typedef RenderState<RenderStateType_CullMode, int> RenderState_CullMode;
typedef RenderState<RenderStateType_AlphaFunc, RenderStateCompareType> RenderState_AlphaFunc;
typedef RenderState<RenderStateType_AlphaRef, int> RenderState_AlphaRef;
typedef RenderState<RenderStateType_SrcBlend, RenderStateBlendType> RenderState_SrcBlend;
typedef RenderState<RenderStateType_DestBlend, RenderStateBlendType> RenderState_DestBlend;
typedef RenderState<RenderStateType_BlendOp, RenderStateBlendOpType> RenderState_BlendOp;

} // namespace dg
