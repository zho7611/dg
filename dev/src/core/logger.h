// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class Logger {
public:
  bool is_flush_always_;
  bool is_log_enabled_;
  bool is_use_console_;

  Logger();
  bool Open(const Cstr* file_name);
  void Close();
  void Push(const Cstr* message);
  void PushFormat(const Cstr* format, ...);
  void PushFormatLine(const Cstr* format, ...);

  static String GetLogFriendlyFilePath(const Cstr* path);
  static String GetDateTimeString();
  static String GetTimeString();

private:
  File file_;
  bool is_initialized_;
  Console console_;
};

void LogMatrix(const Matrix& matrix);

#define DG_LOG_SOURCE_FILE_LINE_STRING \
  String().Format(TXT("%s(%05d): [%s] "), \
  Logger::GetLogFriendlyFilePath(TXT(__FILE__)).GetConst(), \
  __LINE__, \
  Logger::GetTimeString().GetConst())

#define DG_LOG(...) { g_logger->PushFormat(__VA_ARGS__); }

#if defined(DG_DEBUG) // In Debug mode, add "SourceFile.cpp(LineNumber): " for the short-cut to the source code position
// LogLine() is used for logging the source line indicator(msvc format) for debugging
// And the line-ending character(\n)
#define DG_LOG_LINE(...) { if (g_logger) { g_logger->PushFormat(DG_LOG_SOURCE_FILE_LINE_STRING.GetConst()); g_logger->PushFormatLine(__VA_ARGS__); } }
#else
#define DG_LOG_LINE(...) { if (g_logger) { g_logger->PushFormat(TXT("[%s] "), Logger::GetTimeString().GetConst()); g_logger->PushFormatLine(__VA_ARGS__); } }
#endif
#define DG_LOG_LINE_ONE_TIME(...) { static int is_first = 1; if (is_first) { is_first = 0; DG_LOG_LINE(__VA_ARGS__); } }

} // namespace dg
