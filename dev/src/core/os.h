// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {
namespace Os {

// Get current user name. e.g. "jchoi"
void GetUsername(String& out_user_name);
// Get the user directory. e.g. "c:/Users/Jiho"
void GetUserDirectory(String& out_directory);
// Get the module directory. e.g. "d:/Project/dg/Bin"
void GetModuleDirectory(String& out_directory);
// Get the module file name. e.g. "d:/Project/dg/Bin/Viewer.exe"
void GetModuleFileName(String& out_file_name);
// Get the application working directory. e.g. "d:/Project/dg/Bin"
void GetWorkingDirectory(String& out_directory);
// Set the application working directory
void SetWorkingDirectory(const Cstr* path);
// Sleep the application within some milli-seconds
void SleepMilliSeconds(int milli_seconds);
// Remove the file_name part in path
// E.g. "d:/temp/bin/file_name.ext" -> "d:/temp/bin"
void TrimFileNameFromPath(Cstr* out_path, int path_length);
// Remove the last path separator from the directory path
// E.g. "d:/temp/bin/" -> "d:/temp/bin"
void TrimPathSeparatorFromDirectory(Cstr* out_directory, int dir_length);
// Shell execution
// @see Win32::ShellExecute()
bool Execute(const Cstr* file_path,
    const Cstr* parameters,
    const Cstr* directory);

} // namespace Os
} // namespace dg

