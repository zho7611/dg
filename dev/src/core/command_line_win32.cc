// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "core_first.h"
#include <shellapi.h>

namespace dg {

CommandLine::CommandLine() {
  const Cstr* parameters = ::GetCommandLine();
  int token_size = 0;
  bool is_except_quote = true;
  const Cstr* next_parameter = string_util::GetToken(
      parameters,
      TXT(" \t"),
      token_size,
      is_except_quote);
  while (token_size) {
    String param(parameters, 0, token_size);
    param.StripQuotes(string_util::kDoubleQuoteChar);
    options_.PushBack(param);
    parameters = next_parameter;
    next_parameter = string_util::GetToken(
        parameters,
        TXT(" \t"),
        token_size,
        is_except_quote);
  }
}

} // namespace dg

