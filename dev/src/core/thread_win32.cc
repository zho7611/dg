// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "core_first.h"

#include <process.h>

namespace dg {

Thread::Thread() : handle_(0), thread_id_(0) {}

Thread::~Thread() {
  // Do not call WaitForComplete() here. The virtual table could have been changed before CallBack()
  Check(handle_ == NULL);
}

void Thread::EndThread() {
  _endthreadex(0);
}

unsigned __stdcall Thread::CallBack(void* param) {
  Thread* thread = (Thread*)param;
  thread->ThreadMain();
  return 0;
}

void Thread::Run() {
  handle_ = (HANDLE)::_beginthreadex( NULL, 0, CallBack, this, 0, &thread_id_);
}

void Thread::WaitForComplete() {
  if (handle_) {
    ::WaitForSingleObject(handle_, INFINITE);
    ::CloseHandle(handle_);
    handle_ = NULL;
  }
}

} // namespace dg
