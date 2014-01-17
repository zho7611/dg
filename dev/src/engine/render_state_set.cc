// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

RenderStateBase* RenderStateSet::Find(RenderStateType type) {
  Iterator it = Begin();
  Iterator ie = End();
  for (; it != ie; ++it) {
    RenderStateBase* item = static_cast<RenderStateBase*>((*it).ptr());
    if (item->GetType() == type) {
      return item;
    }
  }
  return NULL;
}

void RenderStateSet::ReadFrom(Stream* stream) {
  int count = 0;
  StreamReadFlat(stream, count);
  for (int idx = 0; idx < count; ++idx) {
    RenderStateType type;
    StreamReadFlat(stream, type);
    RenderStateBase* state = NULL;
    switch (type) {
    case RenderStateType_AlphaBlendEnable:
      state = new RenderState_AlphaBlendEnable;
      break;
    case RenderStateType_AlphaTestEnable:
      state = new RenderState_AlphaTestEnable;
      break;
    case RenderStateType_ZEnable:
      state = new RenderState_ZEnable;
      break;
    case RenderStateType_ZWriteEnable:
      state = new RenderState_ZWriteEnable;
      break;
    case RenderStateType_ZFunc:
      state = new RenderState_ZFunc;
      break;
    case RenderStateType_CullMode:
      state = new RenderState_CullMode;
      break;
    case RenderStateType_AlphaFunc:
      state = new RenderState_AlphaFunc;
      break;
    case RenderStateType_AlphaRef:
      state = new RenderState_AlphaRef;
      break;
    case RenderStateType_SrcBlend:
      state = new RenderState_SrcBlend;
      break;
    case RenderStateType_DestBlend:
      state = new RenderState_DestBlend;
      break;
    case RenderStateType_BlendOp:
      state = new RenderState_BlendOp;
      break;
    case RenderStateType_ScissorTestEnable:
      state = new RenderState_ScissorTestEnable;
      break;
    default:
      DG_UNREACHABLE();
    }
    state->ReadFrom(stream);
    PushBack(state);
  }
}

void RenderStateSet::WriteTo(Stream* out_stream) {
  int count = Count();
  StreamWriteFlat(out_stream, count);
  RenderStateSet::Iterator it = Begin();
  RenderStateSet::Iterator ie = End();
  for (; it != ie; ++it, --count) {
    RenderStateBase* state = (*it).ptr();
    RenderStateType type = state->GetType();
    StreamWriteFlat(out_stream, type);
    state->WriteTo(out_stream);
  }
  Check(count == 0);
}

void RenderStateSet::Apply() {
  RenderStateSet::Iterator it = Begin();
  RenderStateSet::Iterator ie = End();
  for (; it != ie; ++it) {
    RenderStateBase* state = (*it).ptr();
    state->Apply();
  }
}

} // namespace dg
