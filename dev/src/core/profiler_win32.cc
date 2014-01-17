// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "core_first.h"

//#define DG_USE_NVTOOLSEXT

#if !defined(DG_USE_NVTOOLSEXT)

namespace dg {
void ProfilerPush(const Cstr*, ColorInt) {}
void ProfilerPop() {}

#else // DG_USE_NVTOOLSEXT

#ifndef _MSC_VER
#error "Use this header only with Microsoft Visual C++ compilers!"
#endif

#define NVTX_STDINT_TYPES_ALREADY_DEFINED
#ifndef _MSC_STDINT_H_ // [
typedef signed __int8 int8_t;
typedef signed __int16 int16_t;
typedef signed __int32 int32_t;
typedef unsigned __int8 uint8_t;
typedef unsigned __int16 uint16_t;
typedef unsigned __int32 uint32_t;
typedef signed __int64 int64_t;
typedef unsigned __int64 uint64_t;
#endif // ] _MSC_STDINT_H_

#include "../externals/NvToolsExt/include/nvToolsExt.h"

#pragma comment(lib, "../externals/NvToolsExt/lib/Win32/nvToolsExt32_1.lib")

namespace dg {

void ProfilerPush(const Cstr* name, ColorInt color) {
  // Zero the structure
  nvtxEventAttributes_t event_attrib = {0};
  // Set the version and the size information
  event_attrib.version = NVTX_VERSION;
  event_attrib.size = NVTX_EVENT_ATTRIB_STRUCT_SIZE;
  // Configure the attributes.  0 is the default for all attributes
  event_attrib.colorType = NVTX_COLOR_ARGB;
#if defined(DG_USE_COLOR_INT_AS_ABGR)
  event_attrib.color = DG_CONVERT_ARGTB_TO_ABGR(color);
#else
  event_attrib.color = color;
#endif
#if defined(_UNICODE)
  event_attrib.messageType = NVTX_MESSAGE_TYPE_UNICODE ;
  event_attrib.message.unicode = name;
#else
  event_attrib.messageType = NVTX_MESSAGE_TYPE_ASCII;
  event_attrib.message.ascii = name;
#endif
  nvtxRangePushEx(&event_attrib);
}

void ProfilerPop() {
  nvtxRangePop();
}

#endif // DG_USE_NVTOOLSEXT

} // namespace dg
