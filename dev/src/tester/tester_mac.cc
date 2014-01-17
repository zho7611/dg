// Copyright (C) 2011 by Jiho Choi. All rights reserved.
// [License]

#include "../gui/gui_first.h"
#include "./tester.h"

using namespace Dg;

void AddCoreTests(Tester& tester);
void AddEngineTests(Tester& tester);

#if defined(CoreOsMac)
int main() {
  Application app;
  if (app.OpenApplication(Text("UnitTest"))) {
    Tester tester;
    tester.Application_ = &app;
    AddCoreTests(tester);
    AddEngineTests(tester);
    tester.RunTests();
    app.CloseApplication();
  }
  return 0;
}
#endif
