// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "core_first.h"

namespace dg {

File::File() :
  handle_(INVALID_HANDLE_VALUE),
  size_(0),
  data_(0),
  flag_(kRead) {}

File::File(const Cstr* path, OpenFlag flag) :
  handle_(INVALID_HANDLE_VALUE),
  size_(0),
  data_(0),
  flag_(kRead) {
  Open(path, flag);
}

File::~File() {
  Close();
}

bool File::Open(const Cstr* path, OpenFlag flag) {
  if (!path || (string_util::GetLength(path) == 0)) {
    LogErrorCode(ErrorCode_InvalidParameters);
    return false;
  }
  FilePath absolute_path(path);
  Check(!absolute_path.IsAbsolute());
  absolute_path.MakeAbsolute();
  flag_ = flag;
  DWORD access = GENERIC_READ;
  DWORD share = 0;
  DWORD creation = CREATE_ALWAYS;
  switch (flag) {
  case kRead:
    access = GENERIC_READ;
    creation = OPEN_EXISTING;
    break;
  case kWrite:
    access = GENERIC_WRITE;
    creation = CREATE_ALWAYS;
    break;
  }
  handle_ = ::CreateFile(
      absolute_path.GetPath(), access, share, NULL, creation,
      FILE_ATTRIBUTE_NORMAL, NULL);
  if (handle_ == INVALID_HANDLE_VALUE) {
    DWORD lastError = ::GetLastError();
    switch (lastError) {
    case 2:
      DG_LOG_LINE(TXT("error: file-not-found: file:%s"), path);
      break;
    case 3:
      DG_LOG_LINE(TXT("error: path-not-found: path:%s"), path);
      break;
    case 0x20:
      DG_LOG_LINE(TXT("error: file-open-sharing-violation: path:%s"), path);
      break;
    default:
      DG_LOG_LINE(TXT("error: file-open-failed: path:%s error:%d"), path, lastError);
      break;
    }
    // Returning the false in File::Open() shouldn't be considered as an error, but for now let it be
    // The caller has to handle the result
    return false;
  }
  path_ = path;
  return true;
}

void File::Close() {
  if (handle_ != INVALID_HANDLE_VALUE) {
    if (!::CloseHandle(handle_)) {
      Check(0);
    }
    handle_ = INVALID_HANDLE_VALUE;
  }
  size_ = 0;
  DG_SAFE_DELETE(data_);
}

int File::size() const {
  if (size_ > 0) {
    return size_;
  }
  LARGE_INTEGER largeSize;
  if (::GetFileSizeEx(handle_, &largeSize)) {
    Check(largeSize.HighPart == 0);
    size_ = largeSize.LowPart;
  } else {
    size_ = 0;
  }
  return size_;
}

void File::Seek(SeekFlag flag, int offset) {
  Check(handle_ != INVALID_HANDLE_VALUE);
  DWORD moveMethod;
  switch (flag) {
  case kCurrent:
    moveMethod = FILE_CURRENT;
    break;
  case kBegin:
    moveMethod = FILE_BEGIN;
    break;
  case kEnd:
    moveMethod = FILE_END;
    break;
  }
  ::SetFilePointer(handle_, offset, 0, moveMethod);
}

void File::Rewind() {
  Check(handle_ != INVALID_HANDLE_VALUE);
  ::SetFilePointer(handle_, 0, 0, FILE_BEGIN);
}

bool File::IsFound(const Cstr* file_name) {
  DWORD access = GENERIC_READ;
  DWORD share = 0;
  DWORD creation = OPEN_EXISTING;
  HANDLE handle = ::CreateFile(
      file_name, GENERIC_READ, FILE_SHARE_READ,
      NULL, OPEN_EXISTING,   FILE_ATTRIBUTE_NORMAL, NULL);
  if (handle == INVALID_HANDLE_VALUE) {
    return false;
  }
  if (!::CloseHandle(handle)) {
    Check(0);
  }
  return true;
}

bool File::IsOpened() const {
  return (handle_ != INVALID_HANDLE_VALUE);
}

bool File::DeleteFile(const Cstr* file_name) {
  return (::DeleteFile(file_name) != 0);
}

unsigned int File::ReadBytes(uint8_t* out_value, unsigned int size) {
  Check(handle_ != INVALID_HANDLE_VALUE);
  DWORD readLength;
  ::ReadFile(handle_, out_value, size, &readLength, NULL);
  Check(readLength == size);
  return readLength;
}

unsigned int File::WriteBytes(const uint8_t* value, unsigned int size) {
  Check(handle_ != INVALID_HANDLE_VALUE);
  DWORD writeLength;
  ::WriteFile(handle_, value, size, &writeLength, NULL);
  Check(writeLength == size);
  return writeLength;
}

bool File::RenameFile(const Cstr* source, const Cstr* target) {
  return ::MoveFile(source, target) == TRUE;
}

bool File::CopyFile(const Cstr* source, const Cstr* target, bool is_fail_if_exists) {
  return ::CopyFile(source, target, (is_fail_if_exists == true)) == TRUE;
}

void File::Flush() {
  ::FlushFileBuffers(handle_);
}

} // namespace dg
