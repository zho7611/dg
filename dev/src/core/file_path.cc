// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "core_first.h"

namespace dg {

FilePath::FilePath(const Cstr* path) {
  if (path) {
    path_.Set(path);
    path_.FixupPathSeparators();
    UpdateFromPath();
    UpdateToPath();
  }
}

FilePath::~FilePath() {
}

void FilePath::SetPath(const Cstr* path) {
  path_.Set(path);
  path_.FixupPathSeparators();
  UpdateFromPath();
  UpdateToPath();
}

void FilePath::SetDir(const Cstr* directory) {
  dir_.Set(directory);
  dir_.FixupPathSeparators();
  UpdateToPath();
}

void FilePath::SetFileName(const Cstr* file_name) {
  file_name_.Set(file_name);
  UpdateToPath();
}

FilePath& FilePath::AddRight(const FilePath& rhs) {
  Check(this != &rhs);
  if (!rhs.GetDirString().IsEmpty()) {
    dir_.AddCharRight(string_util::kPathSeparatorChar);
    dir_.AddRight(rhs.GetDir());
  }
  file_name_.Set(rhs.GetFileName());
  UpdateToPath();
  return *this;
}

FilePath& FilePath::AddLeft(const FilePath& lhs) {
  Check(this != &lhs);
  if (!lhs.GetDirString().IsEmpty()) {
    dir_.AddCharLeft(string_util::kPathSeparatorChar);
    dir_.AddLeft(lhs.GetDir());
  }
  UpdateToPath();
  return *this;
}

FilePath& FilePath::Combine(
    const FilePath& lhs,
    const FilePath& rhs) {
  Check(this != &lhs);
  Check(this != &rhs);
  dir_.Set(lhs.GetDir());
  if (!rhs.GetDirString().IsEmpty()) {
    dir_.AddCharRight(string_util::kPathSeparatorChar);
    dir_.AddRight(rhs.GetDir());
  }
  file_name_.Set(rhs.GetFileName());
  UpdateToPath();
  return *this;
}

void FilePath::UpdateFromPath() {
  // If the path is empty, just clear the directory and file_name
  if (path_.IsEmpty()) {
    dir_.Clear();
    file_name_.Clear();
    return;
  }
  int last_slash_idx = path_.LastIndexOf(string_util::kPathSeparatorChar);
  if (last_slash_idx > 0) {
    dir_.CopyFromRange(path_, 0, last_slash_idx);
    dir_.DeleteCurrentPathMark();
    dir_.ShrinkParentPathMark();
    int filenameLength = path_.size()- last_slash_idx - 1;
    if (filenameLength) {
      file_name_.CopyFromRange(path_, last_slash_idx + 1, filenameLength);
    }
  } else {
    file_name_.Set(path_);
  }
}

void FilePath::UpdateToPath() {
  path_.Clear();
  if (!dir_.IsEmpty()) {
    dir_.DeleteCurrentPathMark();
    dir_.ShrinkParentPathMark();
    path_.AddRight(dir_);
  }
  if (!file_name_.IsEmpty()) {
    path_.AddCharRight(string_util::kPathSeparatorChar);
    path_.AddRight(file_name_);
  }
}

void FilePath::MakeAbsolute() {
  Check(!IsAbsolute());
  String curDir;
  Os::GetWorkingDirectory(curDir);
  dir_.AddCharLeft(string_util::kPathSeparatorChar);
  dir_.AddLeft(curDir);
  UpdateToPath();
}

void FilePath::MakeLocal() {
  Check(IsAbsolute());
  String curDir;
  Os::GetWorkingDirectory(curDir);
  Check(MyStrniCmp(dir_.GetConst(), curDir.GetConst(), curDir.size()) == 0);
  dir_.SubtractLeft(curDir, false);
  String pathSeparator;
  pathSeparator.AddCharRight(string_util::kPathSeparatorChar);
  dir_.SubtractLeft(pathSeparator);
  UpdateToPath();
}

bool FilePath::IsAbsolute() const {
#if defined(DG_TARGET_WIN32)
  if (dir_.size()>= 2) {
    if (*dir_.GetConst(1) == ':') {
      return true;
    }
  }
#else
  if (*(dir_.GetConst(0)) == string_util::kPathSeparatorChar) {
    return true;
  }
#endif
  return false;
}

} // namespace dg
