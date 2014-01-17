// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "core_first.h"

namespace dg {

CriticalSection::CriticalSection() {
  ::InitializeCriticalSection(&handle_);
}

CriticalSection::~CriticalSection() {
  ::DeleteCriticalSection(&handle_);
}

void CriticalSection::Enter() {
  ::EnterCriticalSection(&handle_);
}

void CriticalSection::Leave() {
  ::LeaveCriticalSection(&handle_);
}

} // namespace dg
