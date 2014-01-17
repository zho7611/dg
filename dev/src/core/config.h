// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class Config {
public:
  Config();
  virtual ~Config();
  bool Open(const Cstr* file_name);
  void Close();

  String working_dir_;
  String log_path_;
  String system_file_path_;

protected:
  virtual bool LoadFrom(class IniFile* ini_file);
};

} // namespace dg
