// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class Chunk;

class String {
public:
  String();
  String(const String& rhs);
  String(Cstr rhs);
  String(const Cstr* rhs);
#ifdef _UNICODE
  String(const AnsiStr* rhs);
  String(const AnsiStr* rhs, int start_index, int size);
#endif
  String(const Cstr* rhs, int start_index, int size);
  String(const Name& rhs);
  ~String();
  String& operator=(const Cstr* rhs);
  String& operator=(const String& rhs);
  bool operator==(const String& rhs) const;
  bool operator<(const String& rhs) const;
  // Set the size, but ignores the old data
  void SetSize(int size);
  // Set the size and preserve the old data
  void Resize(int size);
  void Set(const Cstr* rhs, int size = 0);
  void Set(const String& rhs);
  void Set(const Name& rhs);
  // Get the string buffer
  Cstr* Get();
  // Get the string buffer at the index location
  Cstr* Get(int index);
  // Get the const string buffer
  const Cstr* GetConst() const;
  // Get the const string buffer at the index location
  const Cstr* GetConst(int index) const;
  // Get the number of characters in the string except for the null char
  int size() const {
    return size_;
  }
  // Return true if the string is empty
  bool IsEmpty() const;
  // Clear the buffer
  void Clear();
  int IndexOf(Cstr ch);
  int LastIndexOf(Cstr ch);
  int IndexOfAny(const Cstr* anyOf);
  int LastIndexOfAny(const Cstr* anyOf);
  void SetAt(int index, Cstr ch);
  Cstr GetAt(int index) const;
  String Substring(int begin_index, int end_index);
  String Left(int length);
  String Right(int length);
  String Mid(int begin_index, int length);
  // Store the concatenation of the left and the right string
  String& Join(const String& left, const String& right);
  String& AddCharRight(Cstr rhs);
  String& AddRight(const Cstr* rhs, int length = 0);
  String& AddRight(const String& rhs);
  String& AddCharLeft(Cstr lhs);
  String& AddLeft(const Cstr* lhs, int length = 0);
  String& AddLeft(const String& lhs);
  String& SubtractLeft(const Cstr* lhs, int length = 0, bool is_case_sensitive = true);
  String& SubtractLeft(const String& lhs, bool is_case_sensitive = true);
  String& SubtractRight(const Cstr* rhs, int length = 0, bool is_case_sensitive = true);
  String& SubtractRight(const String& rhs, bool is_case_sensitive = true);
  void CopyFromRange(const Cstr* source, int start_index_in_source, int length);
  void CopyFromRange(String& source, int start_index_in_source, int length);
  bool IsInteger() const;
  int ToInt() const;
  unsigned int ToUintHexadecimal() const;
  unsigned int ToUint() const;
  float ToFloat() const;
  bool ToBool() const;
  void FromInt(int value);
  // Store the unsigned integer value as an hexadecimal format
  void FromUintHexadecimal(unsigned int value);
  // Store the unsigned integer value as integer format
  void FromUint(unsigned int value);
  // Store the float value as float format
  void FromFloat(float value);
  // Store the boolean value as boolean format. "True" or "False"
  void FromBool(bool value);
  // Return true if this string and rhs string are identical
  bool IsEqual(const String& rhs) const;
  // Return true if this string and rhs string are identical except for their case-ness
  bool IsEqualNoCase(const String& rhs) const;
  bool IsEqual(const Cstr* rhs) const;
  bool IsEqualNoCase(const Cstr* rhs) const;
  // Store the formatted string. The formating will be the same as printf()'s
  String& Format(const Cstr * format, ...);
  String& Format(const Cstr* format, va_list va);
  void ReadFrom(Stream* stream);
  void WriteTo(Stream* out_stream);
  void TrimLeft(const Cstr* separators = 0);
  int Split(Array<String>& out_splited, const Cstr* separators = 0) const;
  // Convert slashes/back-slashes into platform-specific path separators for the consistency
  // It also removes the reduntant path separators. "a//b" -> "a/b", "a//////b" -> "a/b", "a//b////c" -> "a/b/c"
  void FixupPathSeparators();
  // Split this path string into directory strings array
  void SplitDirs(Array<String>& out_dirs) const;
  // Delete current path mark('.') if possible in the string
  // "././abc/./." -> "abc"
  void DeleteCurrentPathMark();
  // Shrink parent path mark('..') if possible in the string
  // "a/b/c/../../" -> "a/"
  void ShrinkParentPathMark();
  bool StripQuotes(Cstr quote_char);
  String& MakeLower();
  String& MakeUpper();
  const AnsiStr* GetAnsiStr();
  const UnicodeStr* GetUnicodeStr();

private:
  static const int kMinLength = 128;

  // Small-sized buffer
  Cstr small_buffer_[kMinLength+1];
  // Large-sized buffer
  Cstr* buffer_;
  // String character count, not counting the last null char
  int size_;
  // This buffer is used only when converting to string type
  void* convert_buffer_;

  void CopyFrom(const Cstr* rhs, int length);
  void CopyFrom(const String& rhs);
};

} // namespace dg
