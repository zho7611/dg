// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "core_first.h"

namespace dg {

Console::Console() {
  is_opened_ = false;
  output_handle_ = INVALID_HANDLE_VALUE;
}

Console::~Console() {
  Close();
}

bool Console::Open() {
  Check(is_opened_ == false);
  BOOL ret = ::AllocConsole();
  if (!ret) {
    PopupMessage(TXT("Error"),
        TXT("Couldn't create a Console. \nIt might be already created by the logger. \nDo not use console for logger."));
    return false;
  }
  output_handle_ = ::CreateFile(
      TXT("CONOUT$"),
      GENERIC_WRITE, 0, 0,
      CREATE_NEW, FILE_ATTRIBUTE_NORMAL, 0);
  if (output_handle_ == INVALID_HANDLE_VALUE) {
    return false;
  }
  input_handle_ = ::CreateFile(
      TXT("CONIN$"),
      GENERIC_READ, 0, 0,
      CREATE_NEW, FILE_ATTRIBUTE_NORMAL, 0);
  if (input_handle_ == INVALID_HANDLE_VALUE) {
    return false;
  }
  ::SetConsoleTitle(TXT("Console"));
  COORD buffer_size;
  buffer_size.X = 132;
  buffer_size.Y = 4096;
  ::SetConsoleScreenBufferSize(output_handle_, buffer_size);
  is_opened_ = true;
  return true;
}

void Console::Close() {
  if (!is_opened_) return;
  if (output_handle_ != INVALID_HANDLE_VALUE) {
    ::CloseHandle(output_handle_);
    output_handle_ = INVALID_HANDLE_VALUE;
  }
  if (input_handle_ != INVALID_HANDLE_VALUE) {
    ::CloseHandle(input_handle_);
    input_handle_ = INVALID_HANDLE_VALUE;
  }
  ::FreeConsole();
  is_opened_ = false;
}

void Console::Print(const Cstr* output) {
  if (!is_opened_) return;
  if (output_handle_ != INVALID_HANDLE_VALUE) {
    DWORD written_chars = 0;
    int length = string_util::GetLength(output);
    Check(length > 0);
    ::WriteConsole(output_handle_, output, length, &written_chars, NULL /* reserved */);
    Check(written_chars > 0);
  }
}

void Console::PrintFormat(const Cstr* format, ...) {
  if (!is_opened_) return;
  va_list va;
  va_start(va, format);
  String buff;
  buff.Format(format, va);
  va_end(va);
  Print(buff.GetConst());
}

Cstr Console::GetChar() {
  Cstr input = TXT('\0');
  if (!is_opened_) return input;
  if (input_handle_ != INVALID_HANDLE_VALUE) {
    DWORD read_chars = 0;
    ::ReadConsole(input_handle_, &input, 1, &read_chars, NULL);
    Check(read_chars == 1);
  }
  return input;
}

} // namespace dg
