// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

// Console window that shows log messages for batcher application
class Console {
public:
  Console();
  ~Console();
  bool Open();
  void Close();
  Cstr GetChar();
  void Print(const Cstr* output);
  void PrintFormat(const Cstr* format,...);

private:
#ifdef DG_TARGET_WIN32
  HANDLE output_handle_;
  HANDLE input_handle_;
#endif
  bool is_opened_;
};

} // namespace dg
