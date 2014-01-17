// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {
namespace string_util {

// Constant characters [[
static const Cstr kCarriageReturnChar = TXT('\r');
static const Cstr kLineFeedChar = TXT('\n');
static const Cstr kTabChar = TXT('\t');
static const Cstr kQuoteChar = TXT('\'');
static const Cstr kDoubleQuoteChar = TXT('\"');
static const Cstr kSpaceChar = TXT(' ');
static const Cstr kBackSlashChar = TXT('\\');
static const Cstr kSlashChar = TXT('/');
static const Cstr kDotChar = TXT('.');
#if defined(DG_TARGET_WIN32)
static const Cstr kPathSeparatorChar = TXT('\\');
static const Cstr kLineEndStr[] = TXT("\r\n");
#else
static const Cstr kPathSeparatorChar = TXT('/');
static const Cstr kLineEndStr[] = TXT("\n");
#endif
// ]] constant ccharacters

// Utility methods [[
Cstr ToLower(Cstr ch);
Cstr ToUpper(Cstr ch);
bool IsEmpty(const Cstr* rhs);
// Get the length of the string. Not counting the terminating null. "hello" -> 5
int GetLength(const Cstr* rhs);
void CopyString(Cstr* target, const Cstr* source);
bool IsWhiteSpace(Cstr ch);
bool IsNumeric(Cstr ch);
bool IsAlphaNumeric(Cstr ch);
bool IsEqual(const Cstr* lhs, const Cstr* rhs);
#if defined(_UNICODE)
bool IsEqual(const AnsiStr* lhs, const AnsiStr* rhs);
#else
bool IsEqual(const UnicodeStr* lhs, const UnicodeStr* rhs);
#endif
bool IsEqualNoCase(const Cstr* lhs, const Cstr* rhs);
void ConvertToSlash(Cstr* inout_cstr);
bool IsLess(const Cstr* lhs, const Cstr* rhs);
// Returns the buffer position which points to the first buffer position that
// Do not matches the separators string
// This can be used to eat whitespace on the left-side of the string
const Cstr* LeftTrim(const Cstr* buffer, const Cstr* separators = 0);
// Trim right-side of the string
void RightTrim(Cstr* inout_buffer, const Cstr* separators);
const Cstr* GetToken(const Cstr* buffer, const Cstr* separators,
    int& out_token_num_chars, bool is_except_quote = true);
int Split(Array<String>& out_splited, const Cstr* source, const Cstr* separators);
bool StripQuotes(Cstr* value_cstr, Cstr quote_char);
// Convert ANSI string to Unicode string
// Source_length : If -1, it assumes that the source_ansi is null-terminated string
// Target_length : If zero, it makes no use of the target_unicode
// Returns the number of wide characters converted
int AnsiToUnicode(const AnsiStr* source_ansi, int source_length, UnicodeStr* target_unicode, int target_length);
// Convert Unicode string to ANSI string
// Source_length : If -1, it assumes that the source_unicode is null-terminated
// Target_length : If zero, it makes no use of the target buffer
// Returns the number of characters converted
int UnicodeToAnsi(const UnicodeStr* source_unicode, int source_length, AnsiStr* target_ansi, int target_length);
String FormatString(const Cstr* format, ...);
// ]] utility methods

} // namespace string_util
} // namespace dg
