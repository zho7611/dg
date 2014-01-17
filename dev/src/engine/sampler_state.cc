// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

template<>
bool SamplerState_AddressU::Apply() {
  return RC_SetSamplerState(stage_index_, SamplerStateType_AddressU, value_);
}

template<>
bool SamplerState_AddressV::Apply() {
  return RC_SetSamplerState(stage_index_, SamplerStateType_AddressV, value_);
}

template<>
bool SamplerState_AddressW::Apply() {
  return RC_SetSamplerState(stage_index_, SamplerStateType_AddressW, value_);
}

template<>
bool SamplerState_MagFilter::Apply() {
  return RC_SetSamplerState(stage_index_, SamplerStateType_MagFilter, value_);
}

template<>
bool SamplerState_MinFilter::Apply() {
  return RC_SetSamplerState(stage_index_, SamplerStateType_MinFilter, value_);
}

template<>
bool SamplerState_MipFilter::Apply() {
  return RC_SetSamplerState(stage_index_, SamplerStateType_MipFilter, value_);
}

template<>
bool SamplerState_SrgbTexture::Apply() {
  return RC_SetSamplerState(stage_index_, SamplerStateType_SrgbTexture, value_);
}

} // namespace dg
