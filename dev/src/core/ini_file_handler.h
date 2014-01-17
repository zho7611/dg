// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class IniFileHandler {
public:
  void ReadFrom(Stream* stream);
  void WriteTo(Stream* out_stream);
  bool GetSectionKeyValue(const String& section, const String& key, String& out_value);
  bool SetSectionKeyValue(const String& section, const String& key, const String& value);

private:
  typedef StringMap<String> SectionType;

  bool ReadSection(Stream* stream, String& out_section);
  bool ReadString(Stream* stream, String& out_string, const Cstr* escapes);
  void SkipComments(Stream* stream);
  void SkipWhiteSpaces(Stream* stream);

  StringMap<SectionType> sections_;
};

} // namespace dg
