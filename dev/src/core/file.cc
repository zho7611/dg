// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "core_first.h"

namespace dg {

unsigned int File::ReadClassTo(String& out_value) {
  unsigned int readLength;
  int length;
  readLength = Read(length);
  Check(readLength == sizeof(length));
  uint8_t * buff = NULL;
  if (length > 0) {
    buff = new uint8_t[length+1];
    readLength = ReadBytes(buff, length);
    Check(length == readLength);
  }
  buff[length] = TXT('\0');
  out_value.Set(reinterpret_cast<Cstr*>(buff));
  if (buff) {
    delete[] buff;
  }
  return readLength;
}

bool File::PrepareBuffer() {
  if (flag_ == kWrite) {
    DG_UNIMPLEMENTED();
  }
  int kLength = size();
  if (kLength > 0) {
    data_ = new uint8_t[kLength];
    return ReadBytes(data_, kLength) == kLength;
  }
  return true;
}

unsigned int File::ReadClassArrayTo(String* out_values, unsigned int num_elems) {
  unsigned int read_length = 0;
  for (unsigned int idx = 0; idx < num_elems; ++idx) {
    read_length += ReadClassTo(out_values[idx]);
  }
  return read_length;
}

unsigned int File::WriteStringFrom(const String& value) {
  unsigned int writeLength;
  int length = value.size();
  writeLength = Write(length);
  Check(writeLength == sizeof(length));
  if (length > 0) {
    writeLength = WriteBytes(reinterpret_cast<const uint8_t*>(value.GetConst()), length);
    Check(writeLength == sizeof(Cstr)*length);
  }
  return writeLength;
}

unsigned int File::WriteStringArrayFrom(const String* values, unsigned int num_elems) {
  unsigned int write_length = 0;
  for (unsigned int idx = 0; idx < num_elems; ++idx) {
    write_length += WriteStringFrom(values[idx]);
  }
  return write_length;
}

bool File::IsSameFile(const Cstr* source, const Cstr* target) {
  File sourceFile(source);
  File targetFile(target);
  sourceFile.PrepareBuffer();
  targetFile.PrepareBuffer();
  return (0 == ::memcmp(targetFile.data(), sourceFile.data(), sourceFile.size()));
}

} // namespace dg
