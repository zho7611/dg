// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

Application::Application() {
  g_app = this;
}

Application::~Application() {
  Check(is_quit_);
  g_app = NULL;
}

void Application::ResetRenderer() {
  DG_UNIMPLEMENTED();
}

bool Application::Dispatch() {
  DG_UNIMPLEMENTED();
  return true;
}

bool Application::Init(const Cstr* name, const Cstr* config_file) {
  DG_UNIMPLEMENTED();
  g_renderer.InitRenderDevice();
  g_renderer.BindResource();
  g_renderer.BindUnmanagedResource();
  return true;
}

} // namespace dg

#include <jni.h>

extern "C" {

JNIEXPORT jstring JNICALL Java_com_jiho_dgandroidtest_DgAndroidTest_stringFromJNI( JNIEnv* env, jobject thiz) {
  int ret = ApplicationMain();
  if (ret == 0) {
    return env->NewStringUTF("OK");
  }
  return env->NewStringUTF("Failed");
}

} // Extern "C" {
