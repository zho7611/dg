// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "core_first.h"

namespace dg {

bool BigSizeGuid::MakeUnique() {
  struct timeval tv;
  struct timezone tz;
  ::gettimeofday(&tv, &tz);
  if (sizeof(tv) <= sizeof(*this)) {
    uint8_t* start = (uint8_t*)this;
    start += sizeof(*this) - sizeof(tv);
    MyMemCpy(start, &tv, sizeof(tv));
  } else {
    DG_LOG_LINE(TXT("error: bigsizeguid.makeunique.failed:"));
    return false;
  }
  return true;
}

} // namespace dg
