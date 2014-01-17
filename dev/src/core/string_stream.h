// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class StringStream {
public:
  StringStream(const Cstr* value);
  bool ReadInt(int& out_value);
  bool ReadFloat(float& out_value);
  bool ReadClassTo(String& out_value);

private:
  const Cstr* Separators() const;

  int position_;
  Cstr* buffer_;
};

} // namespace dg
