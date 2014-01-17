// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "core_first.h"

namespace dg {

void Os::GetUsername(String& out_user_name) {
  const AnsiStr* username = ::getenv(ANSI_TXT("USERNAME"));
  out_user_name.Set(username);
}

void Os::TrimFileNameFromPath(Cstr* out_path, int path_length) {
  Check(path_length == string_util::GetLength(out_path));
  Check(path_length > 1);
  bool isFound = false;
  int current = path_length-1;
  for (; current > 0; --current) {
    if (isFound && (out_path[current] != string_util::kBackSlashChar) && (out_path[current] != string_util::kSlashChar)) {
      break;
    }
    if (out_path[current] == string_util::kBackSlashChar || out_path[current] == string_util::kSlashChar) {
      isFound = true;
    }
  }
  out_path[current + 1]=0;
}

void Os::TrimPathSeparatorFromDirectory(Cstr* out_directory, int dir_length) {
  Check(dir_length == string_util::GetLength(out_directory));
  Check(dir_length > 1);
  int current = dir_length-1;
  for (; current > 0; --current) {
    if ((out_directory[current] != string_util::kBackSlashChar) && (out_directory[current] != string_util::kSlashChar)) {
      break;
    }
  }
  out_directory[current + 1]=0;
}

} // namespace dg
