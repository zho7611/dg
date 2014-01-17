// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

#ifdef _UNICODE
typedef wchar_t Cstr;
#else
typedef char Cstr;
#endif

typedef char AnsiStr;
typedef wchar_t UnicodeStr;

#ifndef _DG_TEXT
#ifdef _UNICODE
#define _DG_TEXT(TextString) L##TextString
#else
#define _DG_TEXT(TextString) TextString
#endif // _UNICODE
#endif // Text

#ifndef TXT
#define TXT(TextString) _DG_TEXT(TextString)
#endif // Text

#ifndef ANSI_TXT
#define ANSI_TXT(TextString) TextString
#endif // AnsiText

#ifndef DG_SAFE_RELEASE
#define DG_SAFE_RELEASE(in_ptr) { if (in_ptr) { (in_ptr)->Release(); (in_ptr) = NULL; } }
#endif

#ifndef DG_SAFE_ADDREF
#define DG_SAFE_ADDREF(in_ptr) { if (in_ptr) { (in_ptr)->AddRef(); } }
#endif

#ifndef DG_SAFE_DELETE
#define DG_SAFE_DELETE(in_ptr) { if (in_ptr) { delete (in_ptr); (in_ptr) = NULL; } }
#endif

#ifndef DG_SAFE_DELETE_ARRAY
#define DG_SAFE_DELETE_ARRAY(in_ptr) { if (in_ptr) { delete [] (in_ptr); (in_ptr) = NULL; } }
#endif

#ifndef DG_UNREF
#define DG_UNREF(in_var) (in_var)
#endif

#ifndef DG_UNIMPLEMENTED
#define DG_UNIMPLEMENTED() Check(!"not implemented yet!")
#endif

// Use this for unimplemented but skip-able for now (with log message)
#ifndef DG_UNIMPLEMENTED_SKIP
#define DG_UNIMPLEMENTED_SKIP() DG_LOG_LINE(TXT("warning: not implemented yet!"))
#endif

#ifndef DG_UNREACHABLE
#define DG_UNREACHABLE() Check(!"Cannot reach here!")
#endif

#ifndef DG_PURE
#if defined(DG_TARGET_WIN32)
#define DG_PURE = 0 { Check(!"Pure virtual call!"); }
#else // DG_TARGET_WIN32
#define DG_PURE = 0
#endif // DG_TARGET_WIN32
#endif // DG_PURE

#if defined(DG_TARGET_WIN32)
#define DG_NO_VIRTUAL_TABLE __declspec(novtable)
#else
#define DG_NO_VIRTUAL_TABLE
#endif

// This should be in private: scope
#ifndef DG_DISALLOW_COPY_ASSIGN
#define DG_DISALLOW_COPY_ASSIGN(ClassType) ClassType(const ClassType&); void operator=(const ClassType&);
#endif

const int kMaxPathText = 256;
const int kMaxShortText = 0x7f;
const int kMaxSprintf = 2048;
const int kMaxText = 0x7ff;

} // namespace dg
