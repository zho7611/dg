// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "core_first.h"

#ifdef DG_DEBUG
#define DG_USE_LEAK_DETECTOR
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

namespace dg {

class LeakDetector {
public:
  LeakDetector();
};

LeakDetector g_leak_detector_;

LeakDetector::LeakDetector() {
  // When the memory leak is reported, set the s_break_alloc_index to
  // Break at the location where the leaking memory is allocated
  static long s_break_alloc_index = 0;
#ifdef DG_USE_LEAK_DETECTOR
  _CrtSetBreakAlloc(s_break_alloc_index);
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
  _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
  _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
  _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_DEBUG);
#endif // #ifdef UseLeakDetector
}

} // namespace dg
