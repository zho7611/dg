// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "core_first.h"
#include <Objbase.h>

namespace dg {

bool BigSizeGuid::MakeUnique() {
  GUID* guid = reinterpret_cast<GUID*>(this);
  HRESULT result = ::CoCreateGuid(guid);
  return (result == S_OK);
}

} // namespace dg
