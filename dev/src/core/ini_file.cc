// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "core_first.h"
#include "ini_file_handler.h"

namespace dg {

bool IniFile::GetValue(const Cstr* section, const Cstr* key, int& out_value) {
  String value;
  if (GetValue(section, key, value)) {
    out_value = value.ToInt();
    return true;
  }
  return false;
}

bool IniFile::GetValue(const Cstr* section, const Cstr* key, float& out_value) {
  String value;
  if (GetValue(section, key, value)) {
    out_value = value.ToFloat();
    return true;
  }
  return false;
}

bool IniFile::GetValue(const Cstr* section, const Cstr* key, bool& out_value) {
  String value;
  if (GetValue(section, key, value)) {
    out_value = value.ToBool();
    return true;
  }
  return false;
}

bool IniFile::SetValue(const Cstr* section, const Cstr* key, int value) {
  String valueStr;
  valueStr.FromInt(value);
  return SetValue(section, key, valueStr);
}

bool IniFile::SetValue(const Cstr* section, const Cstr* key, float value) {
  String valueStr;
  valueStr.FromFloat(value);
  return SetValue(section, key, valueStr);
}

bool IniFile::SetValue(const Cstr* section, const Cstr* key, bool value) {
  String valueStr;
  valueStr.FromBool(value);
  return SetValue(section, key, valueStr);
}

} // namespace dg
