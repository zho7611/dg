// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "core_first.h"
#include <wchar.h> // For wcscmp()

namespace dg {

#if !defined(DG_DEBUG)
#if defined(DG_TARGET_WIN32)
bool AssertFunc(const Cstr* message, const Cstr* file_name, int lineNumber) {
  String msg;
  msg.Format(TXT("Error: %s: %d"), file_name, lineNumber);
  int Ret = ::MessageBox(NULL, message, msg.GetConst(), MB_OKCANCEL | MB_TOPMOST);
  switch (Ret) {
  case IDOK:
    return true;
    break;
  case IDCANCEL:
    return false;
  }
  return false;
}
#else
bool AssertFunc(const Cstr* message, const Cstr* file_name, int lineNumber) {
  DG_UNREF(file_name);
  DG_UNREF(lineNumber);
  printf(message);
  return true;
}
#endif // DG_TARGET_WIN32
#endif // CoreDebug

void OutputDebugFunc(const Cstr* message) {
#if defined(DG_TARGET_WIN32)
  ::OutputDebugString(message);
#elif defined(DG_TARGET_ANDROID)
  __android_log_write(ANDROID_LOG_INFO, "dg", message);
#else
  printf(message);
#endif
}

void OutputDebugFuncFormat(const Cstr * format, ...) {
  va_list va;
  va_start(va, format);
  String buff;
  buff.Format(format, va);
  va_end(va);
  OutputDebugFunc(buff.GetConst());
}

void PopupMessage(const Cstr* caption, const Cstr* format, ...) {
  Check(caption);
  va_list va;
  va_start(va, format);
  String buff;
  buff.Format(format, va);
  va_end(va);
#if defined(DG_TARGET_WIN32)
  ::MessageBox(NULL, buff.GetConst(), caption, MB_OK | MB_TOPMOST);
#else
  printf(buff.GetConst());
#endif
}

const Cstr* ExtractFileName(const Cstr* full_path, int size) {
  if (size <= 0) {
    Check(0); // Empty path
    return 0;
  }
  int count = size-1;
  while (count--) {
    Cstr c = full_path[count];
    if (c == string_util::kSlashChar || c == string_util::kBackSlashChar) {
      return &(full_path[count+1]);
    }
  }
  return full_path;
}

int MyVsnprintf(Cstr* buffer, int count, const Cstr* format, va_list argptr) {
#if defined(DG_TARGET_WIN32)
#ifdef _UNICODE
  return ::_vsnwprintf(buffer, count, format, argptr);
#else
  return ::_vsnprintf(buffer, count, format, argptr);
#endif
#else
  return ::vsnprintf(buffer, count, format, argptr);
#endif
}

float MyAtof(const Cstr* cstr) {
#ifdef _UNICODE
  return (float)::_wtof(cstr);
#else
  return (float)::atof(cstr);
#endif
}

int MyAtoi(const Cstr* cstr) {
#ifdef _UNICODE
  return ::_wtoi(cstr);
#else
  return ::atoi(cstr);
#endif
}

Cstr* MyStrCpy(Cstr* destination, const Cstr* source) {
#ifdef _UNICODE
  return ::wcscpy(destination, source);
#else
  return ::strcpy(destination, source);
#endif
}

Cstr* MyStrnCpy(Cstr* destination, const Cstr* source, int size) {
#ifdef _UNICODE
  return ::wcsncpy(destination, source, size);
#else
  return ::strncpy(destination, source, size);
#endif
}

// Not including the terminating null character
int MyStrLen(const Cstr* str) {
#ifdef _UNICODE
  return ::wcslen(str);
#else
  return ::strlen(str);
#endif
}

// Not including the terminating null character
int MyStrLenAnsi(const AnsiStr* str) {
  return ::strlen(str);
}

#ifdef _UNICODE
int MyStrCmp(const AnsiStr* string1, const AnsiStr* string2) {
  return ::strcmp(string1, string2);
}
#else
int MyStrCmp(const UnicodeStr* string1, const UnicodeStr* string2) {
  return ::wcscmp(string1, string2);
}
#endif

int MyStrCmp(const Cstr* string1, const Cstr* string2) {
#ifdef _UNICODE
  return ::wcscmp(string1, string2);
#else
  return ::strcmp(string1, string2);
#endif
}

int MyStriCmp(const Cstr* string1, const Cstr* string2) {
#if defined(DG_TARGET_WIN32)
#ifdef _UNICODE
  return ::_wcsicmp(string1, string2);
#else
  return ::_stricmp(string1, string2);
#endif
#else
  String str1(string1), str2(string2);
  str1.MakeLower();
  str2.MakeLower();
  return ::strcmp(str1.Get(), str2.Get());
#endif
}

int MyStrnCmp(const Cstr* string1, const Cstr* string2, int size) {
#ifdef _UNICODE
  return ::wcsncmp(string1, string2, size);
#else
  return ::strncmp(string1, string2, size);
#endif
}

int MyStrniCmp(const Cstr* string1, const Cstr* string2, int size) {
#if defined(DG_TARGET_WIN32)
#ifdef _UNICODE
  return ::_wcsnicmp(string1, string2, size);
#else
  return ::_strnicmp(string1, string2, size);
#endif
#else
  String str1(string1), str2(string2);
  str1.MakeLower();
  str2.MakeLower();
  return ::strncmp(str1.Get(), str2.Get(), size);
#endif
}

Cstr* MyStrStr(Cstr* str, const Cstr* str_search) {
#ifdef _UNICODE
  return ::wcsstr(str, str_search);
#else
  return ::strstr(str, str_search);
#endif
}

AnsiStr* MyStrStrAnsi(AnsiStr* str, const AnsiStr* str_search) {
  return ::strstr(str, str_search);
}

unsigned int GetUintHashCode(unsigned int key) {
  // Reference: Thomas Wang's hash32shift()
  // Http://www.concentric.net/~Ttwang/tech/inthash.htm
  unsigned int hashCode= ~key + (key << 15); // Key = (key << 15) - key - 1;
  hashCode = hashCode ^ (hashCode >> 12);
  hashCode = hashCode + (hashCode << 2);
  hashCode = hashCode ^ (hashCode >> 4);
  hashCode = hashCode * 2057; // HashCode = (hashCode + (hashCode << 3)) + (hashCode << 11);
  hashCode = hashCode ^ (hashCode >> 16);
  return hashCode;
}

unsigned int GetStringHashCode(const Cstr* key) {
  // Reference: djb2 hash algorithm
  // Http://www.cs.yorku.ca/~oz/hash.html
  unsigned int hashCode = 5381;
  int ch;
  const Cstr* buff = key;
  while (ch = *buff++) {
    hashCode = ((hashCode << 5) + hashCode) + ch; // HashCode * 33 + ch
  }
  return hashCode;
}

} // namespace dg
