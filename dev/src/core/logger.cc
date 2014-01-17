// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "core_first.h"

namespace dg {

Logger::Logger() :
    is_flush_always_(false),
    is_initialized_(false),
    is_log_enabled_(true),
    is_use_console_(false) {
}

bool Logger::Open(const Cstr* file_name) {
  if (!is_log_enabled_) {
    return true;
  }
  Check(!is_initialized_);
  Check(file_name);
  if (!file_.IsFound(file_name)) {
    if (file_.Open(file_name, File::kWrite)) {
      is_initialized_ = true;
      return true;
    }
  }
  if (!file_.Open(file_name, File::kWrite)) {
    // Append mode
    file_.Seek(File::kEnd, 0);
    return false;
  }
  is_initialized_ = true;
  if (is_use_console_) {
    console_.Open();
  }
  return true;
}

void Logger::Close() {
  if (!is_log_enabled_ | !is_initialized_) {
    return;
  }
  if (is_use_console_) {
    console_.Close();
  }
  is_initialized_ = false;
  file_.Close();
}

void Logger::Push(const Cstr* message) {
  if (!is_log_enabled_) {
    return;
  }
  OutputDebugFunc(message);
  if (!is_initialized_) {
    // Not initialized. File-logging and console logging are disabled
    return;
  }
  if (is_use_console_) {
    console_.Print(message);
  }
  Check(file_.IsOpened());
  file_.StreamWriteFlatMultiple(message, string_util::GetLength(message));
  if (is_flush_always_) {
    file_.Flush();
  }
}

void Logger::PushFormat(const Cstr* format, ...) {
  if (!is_log_enabled_) {
    return;
  }
  va_list va;
  va_start(va, format);
  String buff;
  buff.Format(format, va);
  va_end(va);
  return Push(buff.GetConst());
}

void Logger::PushFormatLine(const Cstr* format, ...) {
  if (!is_log_enabled_ || !is_initialized_) {
    return;
  }
  va_list va;
  va_start(va, format);
  String buff;
  buff.Format(format, va);
  va_end(va);
  OutputDebugFunc(buff.GetConst());
  if (is_initialized_) {
    Check(file_.IsOpened());
    if (is_use_console_) {
      console_.Print(buff.GetConst());
    }
  }
  OutputDebugFunc(string_util::kLineEndStr);
  if (is_initialized_) {
    if (is_use_console_) {
      console_.Print(string_util::kLineEndStr);
    }
    file_.StreamWriteFlatMultiple(buff.GetConst(), buff.size());
    file_.StreamWriteFlatMultiple(string_util::kLineEndStr, string_util::GetLength(string_util::kLineEndStr));
    if (is_flush_always_) {
      file_.Flush();
    }
  }
}

String Logger::GetDateTimeString() {
  DateTime now = DateTime::Now();
  Cstr buffer[kMaxSprintf];
  MySnprintf(
      buffer,
      kMaxSprintf,
      TXT("%02d:%02d:%02d %02d/%02d/%04d"),
      now.Hour(), now.Minute(), now.Second(),
      now.Month(), now.Day(), now.Year());
  return String(buffer);
}

String Logger::GetTimeString() {
  DateTime now = DateTime::Now();
  Cstr buffer[kMaxSprintf];
  MySnprintf(
      buffer,
      kMaxSprintf,
      TXT("%02d:%02d:%02d.%03d"),
      now.Hour(), now.Minute(), now.Second(), now.MilliSeconds());
  return String(buffer);
}

String Logger::GetLogFriendlyFilePath(const Cstr* path) {
  FilePath filePath(path);
  return filePath.GetFileNameString().MakeLower();
}

void dg::LogMatrix(const Matrix& matrix) {
  g_logger->PushFormatLine(TXT("matrix = "));
  for (int idx = 0; idx < 4; ++idx) {
    g_logger->PushFormatLine(TXT("%ff, %ff, %ff, %ff,"),
        matrix.items_44_[idx][0],
        matrix.items_44_[idx][1],
        matrix.items_44_[idx][2],
        matrix.items_44_[idx][3]);
  }
}

} // namespace dg
