// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "../gui/gui_first.h"

using namespace dg;

class HelloWorldView : public View {
public:
  virtual void DrawPostRender() {
    g_engine.system_font.Draw(TXT("Hello, World!"), Point2(0, 16), DG_MAKE_COLOR_INT(255, 255, 0, 0));
  }
};

class HelloWorldApp : public Application {
public:
  virtual bool Init() {
    Application::Init(TXT("Hello World Application"), NULL);
    SetupDefaultView(NULL, new HelloWorldView, NULL, NULL);
    return true;
  }
};

int dg::ApplicationMain() {
  HelloWorldApp app;
  if (app.Init()) {
    app.Run();
    app.Clear();
  }
  return 0;
}
