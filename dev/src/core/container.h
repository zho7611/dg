// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

template<typename T>
static void DeleteAll(T& container) {
  typename T::Iterator it = container.Begin();
  typename T::Iterator ie = container.End();
  for (; it != ie; ++it) {
    DG_SAFE_DELETE(it.Value());
  }
}

// Do not call ReleaseAll() for container that has Ptr as its Value.
// In that case, Ptr->Release() and Ptr = 0 in DG_SAFE_RELEASE() can
// Cause runtime-error for deleting twice.
template<typename T>
static void ReleaseAll(T& container) {
  typename T::Iterator it = container.Begin();
  typename T::Iterator ie = container.End();
  for (; it != ie; ++it) {
    DG_SAFE_RELEASE(it.Value());
  }
}

} // namespace dg
