// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "core_first.h"

namespace dg {

#include <sys/types.h>
#include <sys/stat.h>

File::File() :
  handle_(0),
  size_(0),
  data_(0),
  flag_(kRead) {
}

File::File(const Cstr* path, OpenFlag flag) :
  handle_(0),
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
  absolute_path.MakeAbsolute();
  flag_ = flag;
  const AnsiStr* mode = 0;
  switch (flag) {
  case kRead:
    mode = "rb";
    break;
  case kWrite:
    mode = "wb";
    break;
  }
  handle_ = ::fopen(path, mode);
  if (handle_ == 0) {
    DG_LOG_LINE(TXT("error: core.file.open.failed: file:%s"), path);
    // Returning the false in File::Open() shouldn't be considered as an error, but for now let it be
    // The caller has to handle the result
    DG_UNREACHABLE();
    return false;
  }
  path_ = path;
  return true;
}

void File::Close() {
  if (handle_) {
    if (::fclose(handle_) != 0) {
      Check(0); // Close error
    }
    handle_ = NULL;
  }
  size_ = 0;
  DG_SAFE_DELETE(data_);
}

int File::size() const {
  if (size_ > 0) {
    return size_;
  }
  struct stat64 file_stat;
  int err = ::stat64(path_.GetConst(), &file_stat);
  if (0 != err) {
    return 0;
  }
  Check(file_stat.st_size < 0xFFFFFFFF); // It shouldn't be bigger than 4GB size!
  size_ = (unsigned int)file_stat.st_size;
  return size_;
}

void File::Rewind() {
  Check(handle_);
  ::fseek(handle_, 0, SEEK_SET);
}

bool File::IsFound(const Cstr* file_name) {
    struct stat file_stat;
    int err = ::stat(file_name, &file_stat);
    if (err == -1) {
      return false;
    }
    return true;
  }
}

bool File::IsOpened() const {
  return (handle_ != NULL);
}

bool File::DeleteFile(const Cstr* file_name) {
  return (::remove(file_name) == 0);
}

unsigned int File::ReadBytes(uint8_t* out_value, unsigned int size) {
  Check(handle_);
  size_t readLength = ::fread(out_value, size, 1, handle_);
  Check(readLength == size);
  return readLength;
}

unsigned int File::WriteBytes(const uint8_t* value, unsigned int size) {
  Check(handle_);
  size_t writeLength= ::fwrite(value, size, 1, handle_);
  Check(writeLength== size);
  return writeLength;
}

bool File::RenameFile(const Cstr* source, const Cstr* target) {
  return (::rename(source, target) == 0);
}

bool File::CopyFile(const Cstr* source, const Cstr* target, bool is_fail_if_exists) {
  FILE* file_source = ::fopen(source, "rb");
  if (file_source == 0) {
    return false;
  }
  const Cstr* mode = "wb+";
  if (is_fail_if_exists) {
    mode = "wb";
  }
  FILE* file_target = ::fopen(target, mode);
  if (file_target == 0) {
    ::fclose(file_source);
    return false;
  }
  const int kBufferSize = 16;
  Cstr buff[kBufferSize];
  int read_size = ::fread(buff, kBufferSize, 1, file_source);
  while (read_size) {
    ::fwrite(buff, kBufferSize, 1, file_target);
    read_size = ::fread(buff, kBufferSize, 1, file_source);
  }
  ::fclose(file_target);
  ::fclose(file_source);
  return true;
}

void File::Flush() {
  ::fflush(handle_);
}

void File::Seek(SeekFlag flag, int offset) {
  switch (flag) {
  case kBegin:
    ::fseek(handle_, offset, SEEK_SET);
    break;
  case kEnd:
    ::fseek(handle_, offset, SEEK_END);
    break;
  case kCurrent:
    ::fseek(handle_, offset, SEEK_CUR);
    break;
  default:
    DG_UNREACHABLE();
    break;
  }
}

} // namespace dg
