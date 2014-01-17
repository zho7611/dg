// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {
namespace batcher {

extern Console g_console;

class BatchJob {
public:
  virtual void Usage() = 0;
  virtual bool Run(CommandLine* command_line) = 0;
};

class BatchJobMain : public BatchJob {
public:
  BatchJobMain(bool is_console) {
    Check(s_instance_ == NULL);
    s_instance_ = this;
    IsSilentExit_ = false;
    if (is_console) {
      batcher::g_console.Open();
    }
  }
  ~BatchJobMain() {
    if (!IsSilentExit_) {
      batcher::g_console.Print(TXT("\nPress the enter key."));
      batcher::g_console.GetChar();
    }
    batcher::g_console.Close();
    s_instance_ = NULL;
  }
  static BatchJobMain* GetInstance() {
    return s_instance_;
  }
  void Usage();
  bool Run(CommandLine* command_line);

protected:
  bool IsSilentExit_;
  static BatchJobMain* s_instance_;
};

} // namespace batcher
} // namespace dg
