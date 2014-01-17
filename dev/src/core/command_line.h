// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class CommandLine {
public:
  CommandLine();
  CommandLine(int argc, Cstr** argv);
  bool HasOption(const Cstr* option) const {
    return GetOptionIndex(option) > 0;
  }
  bool HasOption(const Cstr* option, int& out_index) const {
    out_index = GetOptionIndex(option);
    return (out_index > 0);
  }
  int GetOptionIndex(const Cstr* option) const;
  int NumOptions() const {
    return options_.Count();
  }
  const Cstr* GetOption(int index) const;

private:
  Array<String> options_;
};

} // namespace dg
