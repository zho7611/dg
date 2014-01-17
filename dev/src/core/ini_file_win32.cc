// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "core_first.h"

namespace dg {

IniFile::IniFile() {}

IniFile::~IniFile() {}

bool IniFile::Load(const Cstr* file_name) {
  file_name_ = file_name;
  return File::IsFound(file_name);
}

bool IniFile::Save() {
  DG_UNIMPLEMENTED();
  return false;
}

bool IniFile::GetValue(const Cstr* section, const Cstr* key, String& out_value) {
  const int kMaxProfileString = 255;
  Cstr result_str[kMaxProfileString];
  DWORD size = ::GetPrivateProfileString(
      section, key, NULL,
      result_str, kMaxProfileString, file_name_.Get());
  if (size > 0) {
    out_value.Set(result_str);
    return true;
  }
  return false;
}

bool IniFile::SetValue(const Cstr* section, const Cstr* key, const String& value) {
  BOOL ret = ::WritePrivateProfileString(
      section, key, value.GetConst(),
      file_name_.Get());
  return (ret == TRUE);
}

} // namespace dg
