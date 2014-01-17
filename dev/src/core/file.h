// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

// Disk file
class File {
public:
  enum OpenFlag {
      kRead = 1,
      kWrite = 2,
  };
  enum SeekFlag {
      kBegin = 0,
      kCurrent = 1,
      kEnd = 2,
  };

  File();
  File(const Cstr* path, OpenFlag flag = kRead);
  ~File();
  bool Open(const Cstr* path, OpenFlag flag = kRead);
  void Close();
  const Cstr* GetPath() const {
    return path_.GetConst();
  }
  bool PrepareBuffer();
  uint8_t* data() {
    return data_;
  }
  const uint8_t* data_const() const {
    return data_;
  }
  int size()const;
  void Seek(SeekFlag flag, int offset);
  void Rewind();
  void Flush();
  unsigned int ReadBytes(uint8_t* out_value, unsigned int size);
  unsigned int WriteBytes(const uint8_t* value, unsigned int size);
  template<class T> unsigned int Read(T& out_value);
  template<class T> unsigned int StreamReadFlatMultiple(T* out_values, unsigned int num_elems);
  template<class T> unsigned int Write(const T& value);
  template<class T> unsigned int StreamWriteFlatMultiple(const T* values, unsigned int num_elems);
  unsigned int ReadClassTo(String& out_value);
  unsigned int ReadClassArrayTo(String* out_values, unsigned int num_elems);
  unsigned int WriteStringFrom(const String& value);
  unsigned int WriteStringArrayFrom(const String* values, unsigned int num_elems);
  bool IsOpened() const;
  bool IsLoading() const {
    return flag_ & kRead;
  }
  OpenFlag GetOpenFlag() const {
    return flag_;
  }
  void Reopen();
  void SetDataZero();
  void SetData(uint8_t* data, int size);

  static bool DeleteFile(const Cstr* file_name);
  static bool IsFound(const Cstr* file_name);
  static bool RenameFile(const Cstr* source, const Cstr* target);
  static bool CopyFile(const Cstr* source, const Cstr* target, bool is_fail_if_exists = true);
  static bool IsSameFile(const Cstr* source, const Cstr* target);

private:
  OpenFlag flag_;
  String path_;
  uint8_t* data_;
  mutable unsigned int size_;

#if defined(DG_TARGET_WIN32)
private:
  HANDLE handle_;
#else
private:
  FILE* handle_;
#endif // #if defined(DG_TARGET_WIN32)
};

inline void File::SetDataZero() {
  data_ = NULL;
  size_ = 0;
}

inline void File::SetData(uint8_t* in_data, int size) {
  Check(in_data);
  Check(size);
  data_ = in_data;
  size_ = size;
}

template<class T>
unsigned int File::Read(T& value) {
  return ReadBytes(reinterpret_cast<uint8_t*>(&value), sizeof(T));
}

template<class T>
unsigned int File::StreamReadFlatMultiple(T* values, unsigned int num_elems) {
  return ReadBytes(reinterpret_cast<uint8_t*>(values), sizeof(T)*num_elems);
}

template<class T>
unsigned int File::Write(const T& value) {
  return WriteBytes(reinterpret_cast<const uint8_t*>(&value), sizeof(T));
}

template<class T>
unsigned int File::StreamWriteFlatMultiple(const T* values, unsigned int num_elems) {
  return WriteBytes(reinterpret_cast<const uint8_t*>(values), sizeof(T)*num_elems);
}

} // namespace dg
