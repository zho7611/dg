// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class FilePath {
public:
  FilePath(const Cstr* path = 0);
  ~FilePath();
  // Set the file path. path is the full path with file_name + extension. e.g. "Med/Sources/Box.pak"
  void SetPath(const Cstr* path);
  // Set the directory name only
  void SetDir(const Cstr* directory);
  // Set the file_name(with extension) only
  void SetFileName(const Cstr* file_name);
  // Returns the fill path (path/file_name.ext)
  const Cstr* GetPath() const {
    return path_.GetConst();
  }
  // Returns the directory names only. not the file_name
  const Cstr* GetDir() const {
    return dir_.GetConst();
  }
  // Returns the file_name (with extension)
  const Cstr* GetFileName() const {
    return file_name_.GetConst();
  }
  // Returns the file path (path/file_name.ext) in String format
  String GetPathString() const {
    return path_;
  }
  // Returns the directory names only in String format excecpt the file_name
  String GetDirString() const {
    return dir_;
  }
  // Returns the file_name (with extension) in String format
  String GetFileNameString() const {
    return file_name_;
  }
  FilePath& AddRight(const FilePath& rhs);
  FilePath& AddLeft(const FilePath& lhs);
  FilePath& Combine(const FilePath& lhs, const FilePath& rhs);
  // Make the file path be an absolute path. e.g. "Med/Test.pak" -> "d:/dg/Med/Test.pak"
  void MakeAbsolute();
  // Make the file path be a local path. e.g. "d:/dg/Med/Test.pak" -> "Med/Test.pak"
  void MakeLocal();
  bool IsAbsolute() const;

private:
  String path_; // Path_ = dir_ + '/' + file_name_
  String dir_; // Not-including the last '/'
  String file_name_; // Including period and extension

  void UpdateFromPath();
  void UpdateToPath();
};

} // namespace dg
