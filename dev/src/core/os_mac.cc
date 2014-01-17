// Copyright (C) 2010 by Jiho Choi. All rights reserved.
// [License]

#include "./core_first.h"

using namespace Dg;

#if defined(CoreOsMac)

#include <unistd.h>
#include <sys/time.h>
#include <CoreFoundation/CoreFoundation.h>

void Os::GetUserDirectory(String& outDirectory) {
  AnsiStr* homePath = getenv("HOMEPATH");
  outDirectory.Set(homePath);
}

void Os::GetModuleDirectory(String& outDirectory) {
  CFBundleRef bundle = CFBundleGetMainBundle();
  CFURLRef url = CFBundleCopyBundleURL(bundle);
  // outDirectory will be "file://localhost/Users/zho/Documents/Dg/Src/MacOs/UnitTest/build/Debug/"
  String urlStr;
  urlStr.Set(CFStringGetCStringPtr(CFURLGetString(url), CFStringGetSystemEncoding()));
  // Remove the last path separator from the url.
  outDirectory.CopyFromRange(urlStr, 0, urlStr.GetSize()-1);
}

void Os::SleepMilliSeconds(int milliSeconds) {
  ::sleep(milliSeconds);
}

void Os::GetModuleFileName(String& outFilename) {
  CFBundleRef bundle = CFBundleGetMainBundle();
  CFURLRef url = CFBundleCopyExecutableURL(bundle);
  // outFilename will be "file://localhost/Users/zho/Documents/Dg/Src/MacOs/UnitTest/build/Debug/UnitTest"
  outFilename.Set(CFStringGetCStringPtr(CFURLGetString(url), CFStringGetSystemEncoding()));
}

void Os::GetWorkingDirectory(String& outDirectory) {
  Cstr currentDirectory[Dg::kMaxPathText_] = Text("");
  // @todo: Make it Unicode compatible.
  ::getcwd(currentDirectory, Dg::kMaxPathText_);
  outDirectory.Set(currentDirectory);
}

void Os::SetWorkingDirectory(const Cstr* path) {
  int ret = ::chdir(path);
  if (ret != 0) {
    LogErrorCode(ErrorCode_Unknown);
  }
}

bool Os::Execute(const Cstr* filepath,
    const Cstr* parameters,
    const Cstr* directory) {
  Unimplemented();
  /*
  	HWND hwnd = NULL;
  	HINSTANCE hresult = ::ShellExecute(hwnd, Text("open"), filepath,
  		parameters, directory, SW_SHOWDEFAULT);
  	int errorCode = int(hresult);
  	if (errorCode > 32)
  	{
  		return true;
  	}
  	switch (errorCode)
  	{
  	case ERROR_FILE_NOT_FOUND:
  	case ERROR_PATH_NOT_FOUND:
  		LogErrorCode(Core::ErrorCode_FileNotFound);
  		break;
  	default:
  		LogErrorCode(Core::ErrorCode_Unknown);
  	}
  	return false;
  */
  return false;
}

#endif
