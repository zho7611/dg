// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

#if !defined(DG_TARGET_WIN32)
class IniFileHandler;
#endif
class IniFile {
public:
  IniFile();
  ~IniFile();
  bool Load(const Cstr* file_name);
  bool Save();
  bool GetValue(const Cstr* section, const Cstr* key, String& out_value);
  bool GetValue(const Cstr* section, const Cstr* key, int& out_value);
  bool GetValue(const Cstr* section, const Cstr* key, float& out_value);
  bool GetValue(const Cstr* section, const Cstr* key, bool& out_value);
  bool SetValue(const Cstr* section, const Cstr* key, const String& value);
  bool SetValue(const Cstr* section, const Cstr* key, int value);
  bool SetValue(const Cstr* section, const Cstr* key, float value);
  bool SetValue(const Cstr* section, const Cstr* key, bool value);

private:
  String file_name_;
#if !defined(DG_TARGET_WIN32)
  IniFileHandler* handler_;
#endif
};

} // namespace dg
