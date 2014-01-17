// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class SamplerStateBase : public RefCount {
public:
  virtual SamplerStateType GetType() = 0;
  virtual bool Apply() = 0;
  virtual void ReadFrom(Stream* stream) = 0;
  virtual void WriteTo(Stream* out_stream) = 0;
};

template<SamplerStateType StateType, class ValueType>
class SamplerState : public SamplerStateBase {
public:
  SamplerState() {}
  SamplerState(int stage_index, ValueType value) {
    stage_index_ = stage_index;
    value_ = value;
  }
  SamplerStateType GetType() {
    return StateType;
  }
  SamplerState<StateType, ValueType>& operator =(const SamplerState<StateType, ValueType>& rhs) {
    if (this != &rhs) {
      stage_index_ = rhs.stage_index_;
      value_ = rhs.value_;
    }
    return *this;
  }
  virtual bool Apply();
  virtual void ReadFrom(Stream* stream) {
    StreamReadFlat(stream, stage_index_);
    StreamReadFlat(stream, value_);
  }
  virtual void WriteTo(Stream* out_stream) {
    StreamWriteFlat(out_stream, stage_index_);
    StreamWriteFlat(out_stream, value_);
  }

  int stage_index_;
  ValueType value_;
};

typedef SamplerState<SamplerStateType_AddressU, TextureAddressType> SamplerState_AddressU;
typedef SamplerState<SamplerStateType_AddressV, TextureAddressType> SamplerState_AddressV;
typedef SamplerState<SamplerStateType_AddressW, TextureAddressType> SamplerState_AddressW;
typedef SamplerState<SamplerStateType_MagFilter, TextureFilterType> SamplerState_MagFilter;
typedef SamplerState<SamplerStateType_MinFilter, TextureFilterType> SamplerState_MinFilter;
typedef SamplerState<SamplerStateType_MipFilter, TextureFilterType> SamplerState_MipFilter;
typedef SamplerState<SamplerStateType_SrgbTexture, bool> SamplerState_SrgbTexture;

} // namespace dg
