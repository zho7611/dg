// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class Thread {
public:
#ifdef DG_TARGET_WIN32
  typedef HANDLE ThreadHandleType;
  unsigned int thread_id_;
#elif defined(DG_TARGET_ANDROID)
  typedef pthread_t ThreadHandleType;
#else
  typedef int ThreadHandleType;
#endif

  Thread();
  virtual ~Thread();
  void Run();
  void EndThread();
  void WaitForComplete();

protected:
  virtual void ThreadMain();
#ifdef DG_TARGET_WIN32
  static unsigned __stdcall CallBack(void* param);
#elif defined(DG_TARGET_ANDROID)
  static void* CallBack(void* param);
#endif

  ThreadHandleType handle_;
};

} // namespace dg
