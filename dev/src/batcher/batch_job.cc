// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "batcher_first.h"

namespace dg {
namespace batcher {

Console g_console;
BatchJobMain* BatchJobMain::s_instance_ = NULL;

void BatchJobMain::Usage() {
  batcher::g_console.Print(
      TXT("Usage:\t batcher.exe [-debug|-packer|-generate|-compileshader] [-silentexit] [-hidewindow]\n\n"));
}

bool BatchJobMain::Run(CommandLine* command_line) {
  int idx = 0;
  bool is_ok = false;
  DG_LOG_LINE(TXT("batchjob.run: begin"));
  if (command_line->HasOption(TXT("-silentexit"))) {
    IsSilentExit_ = true;
  }
  if (command_line->HasOption(TXT("-debug"))) {
    GenerateResourceJob generator;
    is_ok = generator.Run(0);
    CompileShaderJob compiler;
    is_ok = compiler.Run(0);
  } else if (command_line->HasOption(TXT("-packer"))) {
    PackagingJob packer;
    is_ok = packer.Run(command_line);
  } else if (command_line->HasOption(TXT("-generate"))) {
    GenerateResourceJob generator;
    is_ok = generator.Run(command_line);
  } else if (command_line->HasOption(TXT("-compileshader"))) {
    CompileShaderJob compiler;
    is_ok = compiler.Run(command_line);
  } else if (command_line->HasOption(TXT("-compileshaders"))) {
    CompileShaderJob compiler;
    is_ok = compiler.Run(command_line);
  } else {
    Usage();
    return false;
  }
  DG_LOG_LINE(TXT("batchjob.run: end:%d"), is_ok);
  return is_ok;
}

} // namespace batcher
} // namespace dg
