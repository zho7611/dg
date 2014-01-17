// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

template<typename T>
T MyMin(T lhs, T rhs) {
  return ((lhs < rhs) ? lhs : rhs);
}

template<typename T>
T MyMax(T lhs, T rhs) {
  return ((lhs > rhs) ? lhs : rhs);
}

// Keep the value in the range
template<typename T>
T BoundMinMax(T value, T min_bound, T max_bound) {
  return MyMin(MyMax(value, min_bound), max_bound);
}

// Write(+) the enum flag into the value
template<typename T>
void EnumWrite(T& state, T flag) {
  state = (T)((int)state | (int)flag);
}

// Erase(-) the enum flag from the value
template<typename T>
void EnumErase(T& state, T flag) {
  state = (T)((int)state & ~(int)flag);
}

template<typename T>
bool EnumCheck(T& state, T flag) {
  return ((int)state & (int)flag) ? true : false;
}

#if defined(DG_DEBUG)
inline bool AssertFunc(const Cstr* message, const Cstr* file_name, int line_number) {
  return false;
}
#else
bool AssertFunc(const Cstr* message, const Cstr* file_name, int line_number);
#endif

// Check() for Win32
#if defined(DG_TARGET_WIN32)
#if defined(DG_DEBUG)
// For Debug Mode
#define Check(expr) { if (!(expr) && !AssertFunc( TXT(#expr), TXT(__FILE__), __LINE__ )) { _asm { int 3 }; } }
#else
// For Release Mode
#define Check(expr)
#endif // #if defined(CoreDebug)
#endif // #if defined(DG_TARGET_WIN32)

// Check() for Non-Win32
#if !defined(DG_TARGET_WIN32) && defined(__GNUC__)
#if defined(DG_DEBUG)
// For Debug Mode
#define Check(expr) { if (!(expr)) { __builtin_trap(); } }
#else
// For Release Mode
#define Check(expr)
#endif
#endif // #if !defined(DG_TARGET_WIN32) && defined(__GNUC__)

#if defined(DG_DEBUG)
#define CheckWarn(expr) if (!(expr)) { DG_LOG_LINE(TXT("warning: check(\"%s\") failed"), TXT(#expr)); }
#else
#define CheckWarn(expr)
#endif

template<class Type>
inline void MySwap(Type& lhs, Type& rhs) {
  Type Temp = lhs;
  lhs = rhs, rhs = Temp;
}

inline int MyMemCmp(const void* lhs, const void* rhs, int size_in_bytes) {
  return ::memcmp(lhs, rhs, size_in_bytes);
}

inline void MyMemCpy(void* destination, const void* source, int size_in_bytes) {
  ::memcpy(destination, source, size_in_bytes);
}

inline void MyMemSet(void* destination, uint8_t value, int size_in_bytes) {
  ::memset(destination, value, size_in_bytes);
}

int MyVsnprintf(Cstr* buffer, int count, const Cstr* format, va_list argptr);

#if defined(DG_TARGET_WIN32)
#ifdef _UNICODE
#define MySnprintf(...) ::_snwprintf(__VA_ARGS__)
#define MySnscanf(...) ::_snwscanf(__VA_ARGS__)
#else
#define MySnprintf(...) ::_snprintf(__VA_ARGS__)
#define MySnscanf(...) ::_snscanf(__VA_ARGS__)
#endif // _UNICODE
#else
#define MySnprintf(...) ::snprintf(__VA_ARGS__)
#define MySnscanf(SrcStr, Size, ...) ::sscanf(SrcStr, __VA_ARGS__) // Skipped the second Size parameter
#endif

float MyAtof(const Cstr* cstr);
int MyAtoi(const Cstr* cstr);
// String copy
Cstr* MyStrCpy(Cstr* destination, const Cstr* source);
// String copy with size
Cstr* MyStrnCpy(Cstr* destination, const Cstr* source, int size);
// String length
int MyStrLen(const Cstr* str);
// String length in Ansi mode
int MyStrLenAnsi(const AnsiStr* str);
// Compare two string
int MyStrCmp(const Cstr* string1, const Cstr* string2);
#if defined(_UNICODE)
int MyStrCmp(const AnsiStr* string1, const AnsiStr* string2);
#else
int MyStrCmp(const UnicodeStr* string1, const UnicodeStr* string2);
#endif
// Compare two string without regard to case
int MyStriCmp(const Cstr* string1, const Cstr* string2);
// Compare two string with size
int MyStrnCmp(const Cstr* string1, const Cstr* string2, int size);
// Compare two string without regard to case, with size
int MyStrniCmp(const Cstr* string1, const Cstr* string2, int size);

Cstr* MyStrStr(Cstr* str, const Cstr* str_search);
AnsiStr* MyStrStrAnsi(AnsiStr* str, const AnsiStr* str_search);

#if defined(DG_DEBUG)
#define DG_OUTPUT_DEBUG(...) \
{ OutputDebugFuncFormat(TXT("%s(%d): "), __FILE__, __LINE__); \
OutputDebugFuncFormat(__VA_ARGS__); }
#else
#define DG_OUTPUT_DEBUG(...) OutputDebugNull(__VA_ARGS__)
#endif

inline void OutputDebugNull(const Cstr* format, ...) {
  DG_UNREF(format);
}

void OutputDebugFunc(const Cstr* message);
void OutputDebugFuncFormat(const Cstr* format, ...);
void PopupMessage(const Cstr* caption, const Cstr* format, ...);
const Cstr* ExtractFileName(const Cstr* full_path, int size);
unsigned int GetUintHashCode(unsigned int key);
unsigned int GetStringHashCode(const Cstr* key);

} // namespace dg
