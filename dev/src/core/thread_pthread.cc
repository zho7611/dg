// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "core_first.h"

namespace dg {

Thread::Thread() : handle_(0) {
}

Thread::~Thread() {
  // Do not call WaitForComplete() here. The virtual table could have been changed before CallBack()
  Check(handle_ == NULL);
}

void Thread::EndThread() {
  pthread_exit(NULL);
}

void* Thread::CallBack(void* param) {
  Thread* thread = (Thread*)param;
  thread->ThreadMain();
  return 0;
}

void Thread::Run() {
  int ret = pthread_create(&handle_, NULL, &CallBack, this);
  if (ret != 0) {
    DG_LOG_LINE(TXT("error: thread.run.failed: thread-creation-failed"));
  }
}

void Thread::WaitForComplete() {
  if (handle_) {
    pthread_join(handle_, NULL);
    handle_ = NULL;
  }
}

} // namespace dg
