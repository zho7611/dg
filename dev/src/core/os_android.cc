// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "core_first.h"

namespace dg {

void Os::GetUserDirectory(String& out_directory) {
  const AnsiStr* home_path = ::getenv(ANSI_TXT("HOMEPATH"));
  out_directory.Set(home_path);
}

void Os::GetModuleDirectory(String& out_directory) {
  Cstr buffer[kMaxSprintf];
  Cstr path[kMaxSprintf];
  MySnprintf(buffer, kMaxSprintf, TXT("/proc/%d/exe"), ::getpid());
  const int kPathLength = ::readlink(buffer, path, kMaxSprintf);
  path[kPathLength] = '\0';
  out_directory.Set(path);
}

void Os::GetModuleFileName(String& out_file_name) {
  DG_UNREF(out_file_name);
  DG_UNIMPLEMENTED();
}

void Os::GetWorkingDirectory(String& out_directory) {
  DG_UNIMPLEMENTED();
}

void Os::SetWorkingDirectory(const Cstr* path) {
  DG_UNIMPLEMENTED();
}

void Os::SleepMilliSeconds(int milli_seconds) {
  ::usleep(milli_seconds*1000);
}

bool Os::Execute(const Cstr* file_path, const Cstr* parameters, const Cstr* directory) {
  DG_UNIMPLEMENTED();
  return false;
}

} // namespace dg
