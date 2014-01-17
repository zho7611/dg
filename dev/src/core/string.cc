// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "core_first.h"

namespace dg {

String::String() :
    buffer_(NULL),
    convert_buffer_(0),
    size_(0) {
  small_buffer_[0] = TXT('\0');
}

String::String(Cstr rhs) :
    buffer_(NULL),
    convert_buffer_(0),
    size_(0) {
  small_buffer_[0] = TXT('\0');
  CopyFrom(&rhs, 1);
}

String::String(const String& rhs) :
    buffer_(NULL),
    convert_buffer_(0),
    size_(0) {
  small_buffer_[0] = TXT('\0');
  if (&rhs == this) {
    return;
  }
  CopyFrom(rhs);
}

String::String(const Cstr* rhs) :
    buffer_(NULL),
    convert_buffer_(0),
    size_(0) {
  small_buffer_[0] = TXT('\0');
  Set(rhs);
}

#ifdef _UNICODE
String::String(const AnsiStr* rhs) :
    buffer_(NULL),
    convert_buffer_(0),
    size_(0) {
  small_buffer_[0] = TXT('\0');
  int kLength = string_util::AnsiToUnicode(rhs, -1, 0, 0);
  SetSize(kLength-1);
  string_util::AnsiToUnicode(rhs, -1, Get(), kLength-1);
}

String::String(const AnsiStr* rhs, int start_index, int length) : 
    buffer_(NULL),
    convert_buffer_(0),
    size_(0) {
  small_buffer_[0] = TXT('\0');
  int kLength = string_util::AnsiToUnicode(rhs+start_index, length, 0, 0);
  SetSize(kLength-1);
  string_util::AnsiToUnicode(rhs+start_index, length, Get(), kLength-1);
}

#endif

String::String(const Cstr* rhs, int start_index, int length) :
  buffer_(NULL),
    convert_buffer_(0),
  size_(0) {
  CopyFromRange(rhs, start_index, length);
}

String::String(const Name& rhs) :
  buffer_(NULL),
  convert_buffer_(0),
  size_(0) {
  Set(rhs.GetConst());
}

String::~String() {
  DG_SAFE_DELETE(buffer_);
  DG_SAFE_DELETE(convert_buffer_);
}

const AnsiStr* String::GetAnsiStr() {
#if defined(_UNICODE)
  DG_SAFE_DELETE(convert_buffer_);
  int target_size = string_util::UnicodeToAnsi(GetConst(), -1, (AnsiStr*)convert_buffer_, 0);
  convert_buffer_ = new AnsiStr[target_size];
  string_util::UnicodeToAnsi(GetConst(), -1, (AnsiStr*)convert_buffer_, target_size);
  return (const AnsiStr*)convert_buffer_;
#else
  return GetConst();
#endif
}

const UnicodeStr* String::GetUnicodeStr() {
#if defined(_UNICODE)
  return GetConst();
#else
  DG_SAFE_DELETE(convert_buffer_);
  int target_size = string_util::AnsiToUnicode(GetConst(), -1, (UnicodeStr*)convert_buffer_, 0);
  convert_buffer_ = new UnicodeStr[target_size];
  string_util::AnsiToUnicode(GetConst(), -1, (UnicodeStr*)convert_buffer_, target_size);
  return (UnicodeStr*)convert_buffer_;
#endif
}

void String::Resize(int length) {
  Check(length >= 0);
  if (length <= kMinLength) {
    if (size_ > kMinLength) {
      MyMemCpy(small_buffer_, buffer_, length*sizeof(Cstr));
    }
    small_buffer_[length] = TXT('\0');
    DG_SAFE_DELETE(buffer_);
  } else if ((length > size_)) {
    Cstr* new_cstr = new Cstr[length+1];
    MyMemCpy(new_cstr, (size_ > kMinLength) ?
        buffer_ : small_buffer_, size_*sizeof(Cstr));
    DG_SAFE_DELETE(buffer_);
    buffer_ = new_cstr;
    buffer_[length] = TXT('\0');
    small_buffer_[0] = TXT('\0');
  } else {
    Check(buffer_);
    small_buffer_[0] = TXT('\0');
    buffer_[length] = TXT('\0');
  }
  size_ = length;
}

void String::SetSize(int length) {
  Check(length >= 0);
  if (length == size_) {
    return;
  }
  if (length <= kMinLength) {
    small_buffer_[length] = TXT('\0');
    DG_SAFE_DELETE(buffer_);
  } else if ((length > size_)) {
    Cstr* new_cstr = new Cstr[length+1];
    small_buffer_[0] = TXT('\0');
    DG_SAFE_DELETE(buffer_);
    buffer_ = new_cstr;
    buffer_[length] = TXT('\0');
  } else {
    Check(buffer_);
    small_buffer_[0] = TXT('\0');
    buffer_[length] = TXT('\0');
  }
  size_ = length;
}

void String::Set(const Name& rhs) {
  Set(rhs.GetConst());
}

void String::Set(const Cstr* rhs, int length) {
  Check((GetConst() != rhs) || (length != size_)); // Should be different
  if (!rhs) {
    SetSize(0);
    return;
  }
  if (length == 0) {
    length = string_util::GetLength(rhs);
    if (length == 0) {
      SetSize(0);
      return;
    }
  }
  Check(length >= 0);
  if (size_ && (length != size_)) {
    DG_SAFE_DELETE(buffer_);
    small_buffer_[0] = TXT('\0');
  }
  if (length > kMinLength) {
    buffer_ = new Cstr[length+1];
    MyMemCpy(buffer_, rhs, length*sizeof(Cstr));
    buffer_[length] = 0;
  } else {
    MyMemCpy(small_buffer_, rhs, length*sizeof(Cstr));
    small_buffer_[length] = 0;
  }
  size_ = length;
}

void String::Set(const String& rhs) {
  CopyFrom(rhs);
}

Cstr* String::Get() {
  return (size_ > kMinLength) ? buffer_ : small_buffer_;
}

const Cstr* String::GetConst() const {
  return (size_ > kMinLength) ? buffer_ : small_buffer_;
}

bool String::IsEmpty() const {
  return (size_ == 0);
}

void String::CopyFrom(const String& rhs) {
  Check(this != &rhs);
  DG_SAFE_DELETE(buffer_);
  size_ = rhs.size_;
  small_buffer_[0] = TXT('\0');
  buffer_ = NULL;
  if (size_ == 0) {
    return;
  }
  if (rhs.buffer_) {
    Check(size_ > kMinLength);
    buffer_ = new Cstr[size_+1];
    buffer_[size_] = TXT('\0');
    MyMemCpy(buffer_, rhs.buffer_, size_*sizeof(Cstr));
  } else {
    Check(size_ <= kMinLength);
    small_buffer_[size_] = TXT('\0');
    MyMemCpy(small_buffer_, rhs.small_buffer_, size_*sizeof(Cstr));
  }
}

void String::CopyFrom(const Cstr* rhs, int length) {
  SetSize(length);
  if (length == 0) {
    return;
  }
  MyMemCpy(Get(), rhs, length*sizeof(Cstr));
}

String& String::operator=(const Cstr* rhs) {
  Set(rhs);
  return *this;
}

String& String::operator=(const String& rhs) {
  if (this != &rhs) {
    CopyFrom(rhs);
  }
  return *this;
}

bool String::operator==(const String& rhs) const {
  return string_util::IsEqual(GetConst(), rhs.GetConst());
}

int String::IndexOf(Cstr ch) {
  int count = size();
  for (int idx = 0; idx < count; ++idx) {
    const Cstr* c = GetConst(idx);
    if (*c == ch) {
      return idx;
    }
  }
  return -1;
}

int String::LastIndexOf(Cstr ch) {
  int reverseIdx = size()- 1;
  while (reverseIdx > 0) {
    const Cstr* c = GetConst(reverseIdx);
    if (*c == ch) {
      return reverseIdx;
    }
    --reverseIdx;
  }
  return -1;
}

int String::IndexOfAny(const Cstr* anyOf) {
  int kLength = size();
  const Cstr* currentAny = NULL;
  for (int idx = 0; idx < kLength; ++idx) {
    const Cstr* c = GetConst(idx);
    currentAny = anyOf;
    while (*currentAny) {
      if (*c == *currentAny) {
        return idx;
      }
      ++currentAny;
    }
  }
  return -1;
}

int String::LastIndexOfAny(const Cstr* anyOf) {
  int reverseIdx = size()- 1;
  const Cstr* currentAny = NULL;
  while (reverseIdx > 0) {
    const Cstr* c = GetConst(reverseIdx);
    currentAny = anyOf;
    while (*currentAny) {
      if (*c == *currentAny) {
        return reverseIdx;
      }
      ++currentAny;
    }
    --reverseIdx;
  }
  return -1;
}

void String::SetAt(int index, Cstr ch) {
  if (index >= size_) {
    Check(TXT("out of range"));
    return;
  }
  if (size_ > kMinLength) {
    buffer_[index] = ch;
  } else {
    small_buffer_[index] = ch;
  }
}

Cstr String::GetAt(int index) const {
  if (index >= size_) {
    Check(TXT("out of range"));
    return TXT('\0');
  }
  if (size_ > kMinLength) {
    return buffer_[index];
  }
  return small_buffer_[index];
}

String String::Substring(int begin_index, int end_index) {
  Check(begin_index >= 0);
  Check(end_index >= 0);
  Check(end_index <= size());
  String result;
  for (int idx = begin_index; idx < end_index; ++idx) {
    result.AddCharRight(GetAt(idx));
  }
  return result;
}

String String::Left(int length) {
  return Substring(0, length);
}

String String::Right(int length) {
  int kTotalLength = size();
  return Substring(kTotalLength - length, kTotalLength);
}

String String::Mid(int begin_index, int length) {
  return Substring(begin_index, begin_index + length);
}

bool String::operator<(const String& rhs) const {
  Cstr leftCh, rightCh;
  const int kMinLength = Min(size_, rhs.size_);
  for (int idx = 0; idx < kMinLength; ++idx) {
    leftCh = GetAt(idx);
    rightCh = rhs.GetAt(idx);
    if (leftCh == rightCh) {
      continue;
    }
    return (leftCh < rightCh);
  }
  return (size_ < rhs.size_);
}

String& String::Join(const String& left, const String& right) {
  Set(left);
  AddRight(right);
  return *this;
}

String& String::AddCharRight(Cstr rhs) {
  return AddRight(&rhs, 1);
}

String& String::AddRight(const Cstr* rhs, int length) {
  Check(rhs);
  const int kFirstLength = size();
  const int kSecondLength = (length) ? length : string_util::GetLength(rhs);
  Check(kSecondLength);
  Resize(kFirstLength + kSecondLength);
  Cstr* data = Get() + kFirstLength;
  for (int idx = 0; idx < kSecondLength; ++idx) {
    *data++ = rhs[idx];
  }
  *data = TXT('\0');
  return *this;
}

String& String::AddRight(const String& rhs) {
  return AddRight(rhs.GetConst(), rhs.size());
}

String& String::AddCharLeft(Cstr lhs) {
  return AddLeft(&lhs, 1);
}

String& String::AddLeft(const Cstr* rhs, int length) {
  Check(rhs);
  int kFirstLength = (length) ? length : string_util::GetLength(rhs);
  int kSecondLength = size();
  int kNewLength = kFirstLength + kSecondLength;
  Cstr* new_buff = new Cstr[kNewLength+1];
  MyMemCpy(new_buff, rhs, kFirstLength*sizeof(Cstr));
  MyMemCpy(new_buff+kFirstLength, GetConst(), kSecondLength*sizeof(Cstr));
  new_buff[kNewLength] = TXT('\0');
  CopyFrom(new_buff, kNewLength);
  delete [] new_buff;
  return *this;
}

String& String::AddLeft(const String& rhs) {
  return AddLeft(rhs.GetConst(), rhs.size());
}

String& String::SubtractLeft(const Cstr* lhs, int length, bool is_case_sensitive) {
  Check(lhs != GetConst());
  Check(length >= 0);
  length = (length == 0) ? string_util::GetLength(lhs) : length;
  if (length == 0) {
    return *this;
  }
  String strTemp(*this);
  const Cstr* leftmost = strTemp.GetConst();
  length = Min(length, size());
  int idx = 0;
  for (; idx < length; ++idx) {
    if (is_case_sensitive) {
      if (leftmost[idx] != lhs[idx]) {
        break;
      }
    } else {
      if (string_util::ToLower(leftmost[idx]) != string_util::ToLower(lhs[idx])) {
        break;
      }
    }
  }
  Set(&(leftmost[idx]));
  return *this;
}

String& String::SubtractLeft(const String& lhs, bool is_case_sensitive) {
  return SubtractLeft(lhs.GetConst(), lhs.size(), is_case_sensitive);
}

String& String::SubtractRight(const Cstr* rhs, int length, bool is_case_sensitive) {
  Check(rhs != GetConst());
  length = (length == 0) ? string_util::GetLength(rhs) : length;
  if (length == 0) {
    return *this;
  }
  String strTemp(*this);
  const int kLhsLength = strTemp.size();
  const Cstr* lhsStr = strTemp.GetConst();
  const int kNumChecks = Min(length, kLhsLength);
  int idx = 0;
  for (; idx < kNumChecks; ++idx) {
    if (is_case_sensitive) {
      if (lhsStr[kLhsLength - idx - 1] != rhs[length - idx - 1]) {
        break;
      }
    } else {
      if (string_util::ToLower(lhsStr[kLhsLength - idx - 1]) != string_util::ToLower(rhs[length - idx - 1])) {
        break;
      }
    }
  }
  Set(lhsStr, kLhsLength - idx);
  return *this;
}

String& String::SubtractRight(const String& rhs, bool is_case_sensitive) {
  return SubtractRight(rhs.GetConst(), rhs.size(), is_case_sensitive);
}

Cstr* String::Get(int index) {
  Check(index >= 0);
  Check(index < size_);
  return (size_ > kMinLength) ? buffer_ + index : small_buffer_ + index;
}

const Cstr* String::GetConst(int index) const {
  Check(index >= 0);
  Check(index < size_);
  return (size_ > kMinLength) ? buffer_ + index : small_buffer_ + index;
}

bool String::IsEqual(const String& rhs) const {
  return string_util::IsEqual(GetConst(), rhs.GetConst());
}

bool String::IsEqualNoCase(const String& rhs) const {
  return string_util::IsEqualNoCase(GetConst(), rhs.GetConst());
}

bool String::IsEqual(const Cstr* rhs) const {
  Check(rhs);
  return string_util::IsEqual(GetConst(), rhs);
}

bool String::IsEqualNoCase(const Cstr* rhs) const {
  Check(rhs);
  return string_util::IsEqualNoCase(GetConst(), rhs);
}

void String::Clear() {
  DG_SAFE_DELETE(buffer_);
  size_ = 0;
  small_buffer_[0] = TXT('\0');
}

void String::CopyFromRange(
    const Cstr* source, int start_index_in_source, int length) {
  Check(length > 0);
  Check(start_index_in_source >= 0);
  Check((start_index_in_source + length) <= string_util::GetLength(source));
  SetSize(length);
  Cstr* curBuff = Get();
  MyMemCpy(curBuff, &source[start_index_in_source], length*sizeof(Cstr));
}

void String::CopyFromRange(
    String& source, int start_index_in_source, int length) {
  Check(length > 0);
  Check(start_index_in_source >= 0);
  Check((start_index_in_source + length) <= source.size());
  SetSize(length);
  Cstr* curBuff = Get();
  MyMemCpy(curBuff, source.GetConst(start_index_in_source), length*sizeof(Cstr));
}

void String::ReadFrom(Stream* stream) {
  int length;
  StreamReadFlat(stream, length);
  Check(length >= 0);
  SetSize(length);
  if (length > 0) {
    StreamReadFlatMultiple(stream, Get(), length);
  }
}

void String::WriteTo(Stream* out_stream) {
  int length = size();
  Check(length >= 0);
  StreamWriteFlat(out_stream, length);
  if (length > 0) {
    StreamWriteFlatMultiple(out_stream, Get(), length);
  }
}

void String::TrimLeft(const Cstr* separators) {
  const Cstr* trimmed = string_util::LeftTrim(Get(), separators);
  Set(trimmed);
}

// True if all characters are in [0-9]
bool String::IsInteger() const {
  for (int idx = 0; idx < size_; ++idx) {
    if (!string_util::IsNumeric(GetAt(idx))) {
      return false;
    }
  }
  return true;
}

int String::ToInt() const {
  return MyAtoi(GetConst());
}

unsigned int String::ToUintHexadecimal() const {
  unsigned int value;
  MySnscanf(GetConst(), size(), TXT("%08x"), &value);
  return value;
}

unsigned int String::ToUint() const {
  return static_cast<unsigned int>(MyAtof(GetConst()));
}

float String::ToFloat() const {
  return (float)MyAtof(GetConst());
}

bool String::ToBool() const {
  if (MyStriCmp(GetConst(), TXT("true")) == 0) {
    return true;
  } else if (MyAtoi(GetConst()) > 0) {
    return true;
  }
  return false;
}

// Maximum digit length = digit(INT_MAX) + 1
const int kMaxDigitSize_ = 11;

void String::FromInt(int value) {
  Cstr cstrBuff[kMaxDigitSize_+1];
  MySnprintf(cstrBuff, kMaxDigitSize_, TXT("%d"), value);
  Set(cstrBuff);
}

void String::FromUintHexadecimal(unsigned int value) {
  Cstr cstrBuff[kMaxDigitSize_+1];
  MySnprintf(cstrBuff, kMaxDigitSize_, TXT("%08x"), value);
  Set(cstrBuff);
}

void String::FromUint(unsigned int value) {
  Cstr cstrBuff[kMaxDigitSize_+1];
  MySnprintf(cstrBuff, kMaxDigitSize_, TXT("%d"), value);
  Set(cstrBuff);
}

void String::FromFloat(float value) {
  Cstr cstrBuff[] = TXT("0123456789.0123");
  MySnprintf(cstrBuff, sizeof(cstrBuff), TXT("%10.4f"), value);
  Set(cstrBuff);
}

void String::FromBool(bool value) {
  Set(value ? TXT("True") : TXT("False"));
}

String& String::Format(const Cstr* format, va_list va) {
  int count = -1;
  int next_size = 1024;
  Cstr* buff = NULL;
  while (count < 0) {
    if (buff) delete [] buff;
    buff = new Cstr[next_size+1];
    count = MyVsnprintf(buff, next_size, format, va);
    if (count >= 0) buff[count] = TXT('\0');
    next_size = next_size << 1;
  }
  Set(buff);
  if (buff) delete [] buff;
  return *this;
}

String& String::Format(const Cstr * format, ...) {
  va_list va;
  va_start(va, format);
  Format(format, va);
  va_end(va);
  return *this;
}

int String::Split(Array<String>& out_splited, const Cstr* separators) const {
  return string_util::Split(out_splited, GetConst(), separators);
}

void String::FixupPathSeparators() {
  int kLength = size();
  int read_idx = 0;
  int write_idx = 0;
  bool was_path_separator = false;
  for (int read_idx = 0; read_idx < kLength; ++read_idx) {
    Cstr ch = GetAt(read_idx);
    if ((ch == string_util::kBackSlashChar) || (ch == string_util::kSlashChar)) {
      if (was_path_separator) {
        // Do not add path separator more than once. Skip this
        continue;
      }
      was_path_separator = true;
      if (ch != string_util::kPathSeparatorChar) {
        ch = string_util::kPathSeparatorChar;
      }
    } else {
      was_path_separator = false;
    }
    SetAt(write_idx, ch);
    write_idx++;
  }
  if (write_idx < read_idx) {
    Resize(write_idx);
  }
}

bool String::StripQuotes(Cstr quote_char) {
  Cstr* buffer = new Cstr[size()+1];
  MyMemCpy(buffer, GetConst(), (size()+1)*sizeof(Cstr));
  bool ret = string_util::StripQuotes(buffer, quote_char);
  if (ret) {
    Set(buffer);
  }
  delete [] buffer;
  return ret;
}

String& String::MakeLower() {
  int kLength = this->size();
  for (int idx = 0; idx < kLength; ++idx) {
    SetAt(idx, string_util::ToLower(GetAt(idx)));
  }
  return *this;
}

String& String::MakeUpper() {
  int kLength = this->size();
  for (int idx = 0; idx < kLength; ++idx) {
    SetAt(idx, string_util::ToUpper(GetAt(idx)));
  }
  return *this;
}

void String::SplitDirs(Array<String>& out_dirs) const {
  const Cstr* curDir = GetConst();
  Cstr folder[kMaxShortText] = TXT("");
  int folder_pos = 0;
  const int kDirLength = size();
  for (int dir_pos = 0; dir_pos < kDirLength; ++dir_pos) {
    if (curDir[dir_pos] != string_util::kPathSeparatorChar) {
      folder[folder_pos] = curDir[dir_pos];
      ++folder_pos;
    } else {
      folder[folder_pos] = 0;
      out_dirs.PushBack(String(folder));
      folder_pos = 0;
    }
  }
  folder[folder_pos] = 0;
  if (folder_pos > 0) {
    out_dirs.PushBack(String(folder));
  }
}

void String::DeleteCurrentPathMark() {
  String newDir;
  const Cstr* str = GetConst();
  const int kLength = size();
  bool isLastDot = false;
  for (int idx = 0; idx < kLength; ++idx) {
    if (!isLastDot) {
      if (str[idx] == string_util::kDotChar) {
        if ((idx == (kLength-1)) || (str[idx+1] == string_util::kPathSeparatorChar)) {
          ++idx; // Skip "./"
          continue;
        }
        isLastDot = true;
      }
    } else if (str[idx] == string_util::kDotChar) {
      isLastDot = false; // Ignore the double dots
    }
    newDir.AddCharRight(str[idx]);
  }
  // Replace the current directory from the new one
  Set(newDir);
}

void String::ShrinkParentPathMark() {
  Array<String> dirs;
  Array<String> packed;
  SplitDirs(dirs);
  for (int idx = 0; idx < dirs.Count(); ++idx) {
    const Cstr* folder = dirs.GetConst(idx).GetConst();
    if (string_util::IsEqual(folder, TXT(".."))) {
      packed.PopBack();
    } else {
      packed.PushBack(folder);
    }
  }
  Clear();
  for (int idx = 0; idx < packed.Count(); ++idx) {
    const String& packedStr = packed.Get(idx);
    if (packedStr.size()) {
      if (idx > 0) {
        AddCharRight(string_util::kPathSeparatorChar);
      }
      AddRight(packedStr);
    }
  }
}

} // namespace dg
