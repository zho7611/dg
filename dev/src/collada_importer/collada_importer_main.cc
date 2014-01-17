// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "collada_importer_first.h"

using namespace dg;
using namespace dg::collada_importer;

static void Usage() {
  // E.g. "collada_importer.exe dev/res/fireman.dae res/fireman.pak"
  dg::PopupMessage(
      TXT("Help"),
      TXT("Usage:\t collada_importer.exe [ColladaPath] [PackagePath] [-hidewindow]\n\n")
      TXT("\t ColladaPath:\t The existing collada .dae file path.\n")
      TXT("\t\t\t e.g. dev/res/fireman.dae\n")
      TXT("\n")
      TXT("\t PackagePath:\t The package path to be created.\n")
      TXT("\t -hidewindow:\t Set to hide the window.\n")
      TXT("\t\t\t e.g. res/fireman.pak"));
}

int dg::ApplicationMain() {
  Application app;
  CommandLine command_line;
  if (command_line.NumOptions() > 2) {
    app.is_show_ = command_line.HasOption(TXT("-hidewindow")) ? false : true;
    if (app.Init(TXT("collada_importer"), 0)) {
      String collada_name = command_line.GetOption(1);
      String package_name = command_line.GetOption(2);
      if (collada_importer::g_collada_importer.Open(collada_name.Get())) {
        collada_importer::g_collada_importer.Import(package_name.Get());
      } else {
        DG_LOG_LINE(
          TXT("error: collada_importer.open.failed. file-not-found:%s"),
            collada_name.Get());
        Usage();
      }
      collada_importer::g_collada_importer.Close();
    }
    app.Clear();
  } else {
    Usage();
  }
  return 0;
}
