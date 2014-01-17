// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "core_first.h"

namespace dg {

Config::Config() {}

Config::~Config() {}

bool Config::LoadFrom(IniFile* ini_file) {
  Check(ini_file);
  // Ini
  {
    String include_path;
    if (ini_file->GetValue(TXT("Ini"), TXT("Include"), include_path)) {
      IniFile to_include;
      if (to_include.Load(include_path.Get())) {
        LoadFrom(&to_include);
      } else {
        DG_OUTPUT_DEBUG(TXT("warning: include.ini.failed.\n"));
        return false;
      }
    }
  }
  ini_file->GetValue(TXT("Log"), TXT("IsFlushAlways"), g_logger->is_flush_always_);
  ini_file->GetValue(TXT("Log"), TXT("IsLogEnabled"), g_logger->is_log_enabled_);
  ini_file->GetValue(TXT("Log"), TXT("IsProfilerEnabled"), Profiler::is_profile_enabled);
  if (g_logger->is_flush_always_) {
    Check(g_logger->is_log_enabled_);
    DG_OUTPUT_DEBUG(TXT("warning: IsFlushAlways can cause slow startup.\n"));
  }
  ini_file->GetValue(TXT("Log"), TXT("IsUseConsole"), g_logger->is_use_console_);
  // Path related
  {
    ini_file->GetValue(TXT("Path"), TXT("WorkingDir"), working_dir_);
    ini_file->GetValue(TXT("Path"), TXT("LogPath"), log_path_);
    ini_file->GetValue(TXT("Path"), TXT("SystemFilePath"), system_file_path_);
  }
  return true;
}

bool Config::Open(const Cstr* file_name) {
  if (!File::IsFound(file_name)) {
    LogErrorCode(ErrorCode_FileNotFound);
    DG_LOG_LINE(TXT("file_name: %s"), file_name);
    return false;
  }
  IniFile ini_file;
  ini_file.Load(file_name);
  return LoadFrom(&ini_file);
}

void Config::Close() {}

} // namespace dg
