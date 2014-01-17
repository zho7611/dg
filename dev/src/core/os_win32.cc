// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "core_first.h"
#include <shellapi.h>
#include <userenv.h>

#pragma comment(lib, "userenv.lib")

namespace dg {

void Os::GetUserDirectory(String& out_directory) {
  Cstr home_dir_buffer[kMaxPathText] = { 0 };
  HANDLE token_handle = 0;
  ::OpenProcessToken(::GetCurrentProcess(), TOKEN_QUERY, &token_handle);
  DWORD buffer_size = kMaxPathText;
  ::GetUserProfileDirectory(token_handle, home_dir_buffer, &buffer_size);
  ::CloseHandle(token_handle);
  out_directory.Set(home_dir_buffer);
}

void Os::GetModuleDirectory(String& out_directory) {
  Cstr base_dir[kMaxShortText] = TXT("");
  ::GetModuleFileName(NULL, base_dir, kMaxShortText);
  TrimFileNameFromPath(base_dir, string_util::GetLength(base_dir));
  out_directory.Set(base_dir);
}

void Os::SleepMilliSeconds(int milli_seconds) {
  ::Sleep(milli_seconds);
}

void Os::GetModuleFileName(String& out_file_name) {
  Cstr module_name[kMaxShortText] = TXT("");
  ::GetModuleFileName(NULL, module_name, kMaxShortText);
  out_file_name.Set(module_name);
}

void Os::GetWorkingDirectory(String& out_directory) {
  Cstr current_dir[kMaxShortText] = TXT("");
  ::GetCurrentDirectory(kMaxShortText , current_dir);
  int length = string_util::GetLength(current_dir);
  TrimPathSeparatorFromDirectory(current_dir, length);
  out_directory.Set(current_dir);
}

void Os::SetWorkingDirectory(const Cstr* path) {
  Check(path);
  BOOL result = ::SetCurrentDirectory(path);
  if (!result) {
    LogErrorCode(ErrorCode_InvalidParameters);
  }
}

bool Os::Execute(const Cstr* file_path,
    const Cstr* parameters,
    const Cstr* directory) {
  HWND hwnd = NULL;
  HINSTANCE hresult = ::ShellExecute(hwnd, TXT("open"), file_path,
          parameters, directory, SW_SHOWDEFAULT);
  int error_code = int(hresult);
  if (error_code > 32) {
    return true;
  }
  switch (error_code) {
  case ERROR_FILE_NOT_FOUND:
  case ERROR_PATH_NOT_FOUND:
    LogErrorCode(ErrorCode_FileNotFound);
    break;
  default:
    LogErrorCode(ErrorCode_Unknown);
  }
  return false;
}

} // namespace dg
