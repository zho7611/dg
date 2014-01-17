// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "batcher_first.h"

int dg::ApplicationMain() {
  Application app;
  bool is_ok = true;
  bool is_show = true;
  CommandLine command_line;
  if (command_line.HasOption(TXT("-hidewindow"))) is_show = false;
  app.is_show_ = is_show;
  if (app.Init(TXT("Batcher"), 0)) {
    {
      batcher::BatchJobMain job_main(is_show /* is_console */);
      // Print the command line options
      for (int idx = 0; idx < command_line.NumOptions(); ++idx) {
        batcher::g_console.PrintFormat(TXT("%s "), command_line.GetOption(idx));
      }
      batcher::g_console.Print(TXT("\n"));
      // Run the main job
      is_ok = job_main.Run(&command_line);
      // Print the end message
      batcher::g_console.PrintFormat(TXT("batcher.completed: isok:%d\n"), is_ok);
    }
    app.Clear();
  }
  return (is_ok) ? 0 : 1;
}
