// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "core_first.h"

namespace dg {
namespace string_util {

Cstr ToLower(Cstr ch) {
  if ((ch >= TXT('A')) && (ch <= TXT('Z'))) {
    return ch - TXT('A') + TXT('a');
  }
  return ch;
}

Cstr ToUpper(Cstr ch) {
  if ((ch >= TXT('a')) && (ch <= TXT('z'))) {
    return ch - TXT('a') + TXT('A');
  }
  return ch;
}

bool IsEmpty(const Cstr* rhs) {
  return (!rhs) || (rhs[0] == TXT('\0'));
}

int GetLength(const Cstr* rhs) {
  Check(rhs);
  int cur_len = 0;
  const Cstr* ch = rhs;
  while (*ch != TXT('\0')) {
    ++ch;
    ++cur_len;
  }
  return cur_len;
}

void CopyString(Cstr* target, const Cstr* source) {
  int kSrcLength = GetLength(source);
  MyMemCpy(target, source, (kSrcLength + 1)*sizeof(Cstr));
}

bool IsWhiteSpace(Cstr ch) {
  switch (ch) {
  case kSpaceChar:
  case kTabChar:
  case kCarriageReturnChar:
  case kLineFeedChar:
    return true;
  }
  return false;
}

bool IsNumeric(Cstr ch) {
  return ((ch >= TXT('0')) && (ch <= TXT('9')));
}

bool IsAlphaNumeric(Cstr ch) {
  return (((ch >= TXT('a')) && (ch <= TXT('z'))) ||
          ((ch >= TXT('A')) && (ch <= TXT('Z'))) ||
          ((ch >= TXT('0')) && (ch <= TXT('9'))));
}

bool IsEqual(const Cstr* lhs, const Cstr* rhs) {
  Check(lhs && rhs);
  return (MyStrCmp(lhs, rhs) == 0);
}

#if defined(_UNICODE)
bool IsEqual(const AnsiStr* lhs, const AnsiStr* rhs) {
  Check(lhs && rhs);
  return (MyStrCmp(lhs, rhs) == 0);
}
#else
bool IsEqual(const UnicodeStr* lhs, const UnicodeStr* rhs) {
  Check(lhs && rhs);
  return (MyStrCmp(lhs, rhs) == 0);
}
#endif

bool IsEqualNoCase(const Cstr* lhs, const Cstr* rhs) {
  Check(lhs && rhs);
  return (MyStriCmp(lhs, rhs) == 0);
}

void ConvertToSlash(Cstr* inout_cstr) {
  int idx = 0;
  while (inout_cstr[idx]) {
    if (inout_cstr[idx] == kBackSlashChar) {
      inout_cstr[idx] = kSlashChar;
    }
    ++idx;
  }
}

bool IsLess(const Cstr* lhs, const Cstr* rhs) {
  Check(lhs && rhs);
  return MyStrCmp(lhs, rhs) < 0;
}

const Cstr* LeftTrim(const Cstr* buffer, const Cstr* separators) {
  Cstr ch = *buffer;
  if (!separators) {
    separators = TXT(" \t");
  }
  while (ch) {
    bool isTrimming = false;
    for (int idx = 0; separators[idx]; ++idx) {
      if (ch == separators[idx]) {
        isTrimming = true;
        break;
      }
    }
    if (!isTrimming) {
      return buffer;
    }
    ++buffer;
    ch = *buffer;
  }
  return 0;
}

void RightTrim(Cstr* inout_buffer, const Cstr* separators) {
  DG_UNREF(inout_buffer);
  DG_UNREF(separators);
  DG_UNIMPLEMENTED();
}

// Get a token string from a string buffer with separators
// Buffer - The source string buffer
// Separators - The separater characters. e.g. " ,.\t"
// Out_token_num_chars - The number of characters in the token found
// Return - The buffer pointer right next to the separator
const Cstr* GetToken(
    const Cstr* buffer,
    const Cstr* separators,
    int& out_token_num_chars,
    bool is_except_quote) {
  if (!buffer) {
    out_token_num_chars = 0;
    return NULL;
  }
  int token_idx = 0;
  bool is_separated_once = false;
  bool is_in_quote = false;
  bool is_currently_matched = false;
  for (; buffer[token_idx]; is_currently_matched = false) {
    if (!is_separated_once && is_except_quote && (buffer[token_idx] == TXT('\"'))) {
      if (is_in_quote) {
        is_in_quote = false;
      } else {
        is_in_quote = true;
      }
    }
    if (!is_in_quote) {
      for (int idx = 0; separators[idx]; ++idx) {
        if (buffer[token_idx] == separators[idx]) {
          if (!is_separated_once) {
            out_token_num_chars = token_idx;
            is_separated_once = true;
          }
          is_currently_matched = true;
        }
      }
      bool is_next_null = (buffer[token_idx+1] == NULL);
      if (is_separated_once && (!is_currently_matched || is_next_null)) {
        return buffer + token_idx;
      }
    }
    ++token_idx;
  }
  out_token_num_chars = token_idx;
  return NULL;
}

// Split the string into parts with the separators
// Separators : The characters in the separators are used as delimiters
//              to split the string. e.g. "\n\t "
// Returns the number of items split and 0 if failed
int Split(
    Array<String>& out_splited,
    const Cstr* source,
    const Cstr* separators) {
  int token_size = 0;
  Check(out_splited.IsEmpty());
  out_splited.Clear();
  if (!separators) {
    separators = TXT(" \t");
  }
  const Cstr* curr_source = source;
  const Cstr* next_source = GetToken(curr_source, separators, token_size);
  while (token_size) {
    out_splited.PushBack(String(curr_source, 0, token_size));
    curr_source = next_source;
    next_source = GetToken(curr_source, separators, token_size);
  }
  return out_splited.Count();
}

bool StripQuotes(Cstr* value_cstr, Cstr quote_char) {
  int kLength = GetLength(value_cstr);
  if (kLength < 2) {
    return false;
  }
  if (*value_cstr != quote_char) {
    return false;
  }
  if (value_cstr[kLength-1] != quote_char) {
    return false;
  }
  // Shift left by one
  Cstr* current = value_cstr;
  while (*current) {
    *current = current[1];
    ++current;
  }
  // Owerwrite the last quote with null
  value_cstr[kLength-2] = TXT('\0');
  return true;
}

int AnsiToUnicode(
    const AnsiStr* source_ansi, int source_length,
    UnicodeStr* target_unicode, int target_length) {
#ifdef DG_TARGET_WIN32
  return ::MultiByteToWideChar(
      CP_ACP, MB_PRECOMPOSED,
      source_ansi, source_length,
      target_unicode, target_length);
#else
  DG_UNIMPLEMENTED();
  return 0;
#endif
}

int UnicodeToAnsi(
    const UnicodeStr* source_unicode, int source_length,
    AnsiStr* target_ansi, int target_length) {
#ifdef DG_TARGET_WIN32
  return ::WideCharToMultiByte(
      CP_ACP, 0, source_unicode, source_length,
      target_ansi, target_length, NULL, NULL);
#else
  DG_UNIMPLEMENTED();
  return 0;
#endif
}

String FormatString(const Cstr* format, ...) {
  String str;
  va_list va;
  va_start(va, format);
  str.Format(format, va);
  va_end(va);
  return str;
}

} // namespace string_util
} // namespace dg
