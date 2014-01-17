// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "../gui/gui_first.h"
#include "tester.h"

using namespace dg;

// Usage: tester.exe [testname1] [testname2] [testname3] ...
int dg::ApplicationMain() {
  CommandLine command_line;
  Check(!g_app);
  Application app;
  if (app.Init(TXT("tester"), 0)) {
    if (command_line.NumOptions() > 1) {
      FixedArray<String> testlist;
      testlist.Resize(command_line.NumOptions()-1);
      for (int idx = 0; idx < command_line.NumOptions(); ++idx) {
        if (idx > 0) {
          testlist.Get(idx-1) = command_line.GetOption(idx);
        }
      }
      Tester::Instance().RunTestSet(testlist);
    } else {
      Tester::Instance().RunAllTests();
    }
    app.Clear();
  }
  return 0;
}
