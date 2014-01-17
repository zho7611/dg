// Copyright (C) 2010 by Jiho Choi. All rights reserved
// To use this source, see LICENSE file.

#include "./core_first.h"

#if defined(DG_TARGET_MAC)

namespace Dg {

CriticalSection::CriticalSection() {
  OSStatus status = ::MPCreateCriticalRegion(&Handle_);
  Check(status >= 0);
  DG_UNREF(status);
}

CriticalSection::~CriticalSection() {
  ::MPDeleteCriticalRegion(Handle_);
}

void CriticalSection::Enter() {
  ::MPEnterCriticalRegion(Handle_, kDurationForever);
}

void CriticalSection::Leave() {
  ::MPExitCriticalRegion(Handle_);
}

} // End of namespace Dg

#endif // End of DG_TARGET_MAC
