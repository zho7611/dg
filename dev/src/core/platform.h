// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

#if !defined(UsePredefinedCorePlatform)

// operating system
#if defined(_WIN32)
  #define DG_TARGET_WIN32
#elif defined(__APPLE__)
  #define DG_TARGET_MAC // Mac OS X
#elif defined(__ANDROID__)
  #define DG_TARGET_ANDROID
  //#define DG_TARGET_NULL // Null OS
#endif

// Renderer
#if defined(DG_TARGET_WIN32)
  #if defined(DG_OPENGL)
    #define DG_RENDERER_GL2
  #else
    #define DG_RENDERER_DX9
  #endif
#elif defined(DG_TARGET_ANDROID)
  #define DG_RENDERER_GL2
#else // DG_TARGET_WIN32
  #define DG_RENDERER_NULL
#endif // DG_TARGET_WIN32

// font renderer
#if defined(DG_TARGET_WIN32)
  //#define DG_FONT_FREETYPE// Requires DG_TARGET_WIN32
  #define DG_FONT_NULL
#else
  #define DG_FONT_NULL
#endif

#endif // End of UsePredefinedCorePlatform

// Configuration Modes
//   DG_DEBUG : Debugging Configuration. It could be too slow
//   DG_PROFILE : Performance Test Configuration. Limited debugging is possible
//   DG_SHIP : Ship Configuration. No debugging enabled
#if defined(DG_TARGET_WIN32)
  #if defined(_DEBUG)
    #define DG_DEBUG
    #undef DG_SHIP
    #undef DG_PROFILE
  #else // #ifdef _DEBUG
    #undef DG_DEBUG
    #undef DG_SHIP
    #define DG_PROFILE
  #endif // #if defined(_DEBUG)
#endif // #if defined(DG_TARGET_WIN32)

#if defined(DG_TARGET_MAC)
  #if defined(_GLIBCXX_DEBUG)
    #define DG_DEBUG
  #else // #if defined(_GLIBCXX_DEBUG)
    #define DG_PROFILE
  #endif // #if defined(_GLIBCXX_DEBUG)
#endif // #if defined(DG_TARGET_MAC)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include <time.h>
#include <float.h>

#if _MSC_VER < 1600
typedef signed __int8     int8_t;
typedef signed __int16    int16_t;
typedef signed __int32    int32_t;
typedef unsigned __int8   uint8_t;
typedef unsigned __int16  uint16_t;
typedef unsigned __int32  uint32_t;
typedef signed __int64    int64_t;
typedef unsigned __int64  uint64_t;
#else
#include <stdint.h>
#endif

#if defined(DG_TARGET_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#if defined(DG_TARGET_ANDROID)
#include <unistd.h>
#include <android/log.h>
#include <pthread.h>
#include <time.h>
#endif

#if defined(DG_RENDERER_DX9)
#include "d3dx9.h"
#endif
