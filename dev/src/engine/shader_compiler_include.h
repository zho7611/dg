// Copyright (C) 2010 by Jiho Choi. All rights reserved.
// [License]

#pragma once

namespace Dg {
#if defined(CoreDeviceDriverDx9)
class EffectInclude : public ID3DXInclude {
public:
  EffectInclude();
  ~EffectInclude();

private:
  STDMETHOD(Open)(
      D3DXINCLUDE_TYPE includeType, LPCSTR filename,
      LPCVOID parentData, LPCVOID * dataPointer, UINT * bytes);
  STDMETHOD(Close)(LPCVOID data);
};
#else
class EffectInclude {
};
#endif

extern EffectInclude EffectInclude_;
}
