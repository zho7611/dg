// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "core_first.h"

namespace dg {

#include "ini_file_handler.h"

void IniFileHandler::SkipWhiteSpaces(Stream* stream) {
  Cstr ch;
  chunk.StreamSneakFlat(ch);
  while (ch != TXT('\0')) {
    if (!string_util::IsWhiteSpace(ch)) {
      return;
    }
    chunk.StreamSneakFlat(ch);
  }
}

void IniFileHandler::SkipComments(Stream* stream) {
  const Cstr semicolon = ';';
  SkipWhiteSpaces(chunk);
  Cstr ch;
  chunk.StreamSneakFlat(ch);
  while (ch == semicolon) {
    StreamReadFlat(stream, ch);
    while (!chunk.IsEnd() && (ch != string_util::kLineFeedChar)) {
      StreamReadFlat(stream, ch);
    }
    if (chunk.IsEnd()) {
      break;
    }
    SkipWhiteSpaces(chunk);
    chunk.StreamSneakFlat(ch);
  }
}

bool IniFileHandler::ReadSection(Stream* stream, String& out_section) {
  Cstr ch;
  chunk.StreamSneakFlat(ch);
  if (ch == TXT('[')) {
    StreamReadFlat(stream, ch); // Read out '['
    StreamReadFlat(stream, out_section);
    out_section.SubtractRight(TXT("]"), 1); // Throw the last '[' away
    return true;
  }
  return false;
}

bool IniFileHandler::ReadString(Stream* stream, String& out_string, const Cstr* escapes) {
  String result;
  Cstr ch;
  chunk.StreamSneakFlat(ch);
  Check(escapes);
  bool is_escape_begin = false;
  bool is_done = false;
  while (!chunk.IsEnd() && !is_done) {
    bool is_meet_escape = false;
    for (int idx = 0; escapes[idx] != TXT('\0'); ++idx) {
      if (ch == escapes[idx]) {
        is_meet_escape = true;
        break;
      }
    }
    if (is_escape_begin && !is_meet_escape) {
      is_done = true;
    } else if (is_meet_escape) {
      is_escape_begin = true;
    } else {
      result.AddCharRight(ch);
    }
    StreamReadFlat(stream, ch);
  }
  return is_done;
}

void IniFileHandler::ReadFrom(Stream* stream) {
  String cur_section;
  while (!chunk.IsEnd()) {
    SkipComments(chunk);
    if (!chunk.IsEnd() && !ReadSection(stream, cur_section)) {
      String key, equal, value;
      const Cstr escapes[] = TXT("=;");
      ReadString(stream, key, escapes);
      StreamReadAuto(stream, equal);
      Check(equal.IsEqual(TXT("=")));
      ReadString(stream, value, escapes);
      sections_.Get(cur_section).Get(key) = value;
    }
  }
}

void IniFileHandler::WriteTo(Stream* out_stream) {
  DG_UNIMPLEMENTED();
}

bool IniFileHandler::GetSectionKeyValue(
    const String& section, const String& key, String& out_value) {
  if (!sections_.IsFound(section)) {
    return false;
  }
  SectionType& sect = sections_.Get(section);
  if (!sect.IsFound(key)) {
    return false;
  }
  out_value = sect.Get(key);
  return true;
}

bool IniFileHandler::SetSectionKeyValue(
    const String& section, const String& key, const String& value) {
  SectionType& sect = sections_.Get(section);
  if (sect.IsFound(key)) {
    return false;
  }
  sect.Get(key) = value;
  return true;
}

} // namespace dg
