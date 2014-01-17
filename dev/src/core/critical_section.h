// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {
	
class CriticalSection {
public:
  CriticalSection();
  ~CriticalSection();
  void Enter();
  void Leave();

private:
#if defined(DG_TARGET_WIN32)
  CRITICAL_SECTION handle_;
#elif defined(DG_TARGET_MAC)
  MPCriticalRegionID handle_;
#else
  pthread_mutex_t handle_;
#endif
};

} // namespace dg
