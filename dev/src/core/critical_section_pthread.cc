// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "core_first.h"

namespace dg {

CriticalSection::CriticalSection() {
  pthread_mutex_init(&handle_, NULL);
}

CriticalSection::~CriticalSection() {
  pthread_mutex_destroy(&handle_);
}

void CriticalSection::Enter() {
  pthread_mutex_lock(&handle_);
}

void CriticalSection::Leave() {
  pthread_mutex_unlock(&handle_);
}

} // namespace dg
