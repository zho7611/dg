// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

SamplerStateBase* SamplerStateSet::Find(SamplerStateType type) {
  Iterator it = Begin();
  Iterator ie = End();
  for (; it != ie; ++it) {
    SamplerStateBase* item = static_cast<SamplerStateBase*>((*it).ptr());
    if (item->GetType() == type) {
      return item;
    }
  }
  return NULL;
}

void SamplerStateSet::ReadFrom(Stream* stream) {
  int count = 0;
  StreamReadFlat(stream, count);
  for (int idx = 0; idx < count; ++idx) {
    SamplerStateType type;
    StreamReadFlat(stream, type);
    SamplerStateBase* state = NULL;
    switch (type) {
    case SamplerStateType_AddressU:
      state = new SamplerState_AddressU;
      break;
    case SamplerStateType_AddressV:
      state = new SamplerState_AddressV;
      break;
    case SamplerStateType_AddressW:
      state = new SamplerState_AddressW;
      break;
    case SamplerStateType_MagFilter:
      state = new SamplerState_MagFilter;
      break;
    case SamplerStateType_MinFilter:
      state = new SamplerState_MinFilter;
      break;
    case SamplerStateType_MipFilter:
      state = new SamplerState_MipFilter;
      break;
    case SamplerStateType_SrgbTexture:
      state = new SamplerState_SrgbTexture;
      break;
    default:
      DG_UNIMPLEMENTED();
    }
    state->ReadFrom(stream);
    PushBack(state);
  }
}

void SamplerStateSet::WriteTo(Stream* out_stream) {
  int count = Count();
  StreamWriteFlat(out_stream, count);
  SamplerStateSet::Iterator it = Begin();
  SamplerStateSet::Iterator ie = End();
  for (; it != ie; ++it, --count) {
    SamplerStateBase* state = (*it).ptr();
    SamplerStateType type = state->GetType();
    StreamWriteFlat(out_stream, type);
    state->WriteTo(out_stream);
  }
  Check(count == 0);
}

void SamplerStateSet::Apply() {
  SamplerStateSet::Iterator it = Begin();
  SamplerStateSet::Iterator ie = End();
  for (; it != ie; ++it) {
    SamplerStateBase* state = (*it).ptr();
    state->Apply();
  }
}

} // namespace dg
