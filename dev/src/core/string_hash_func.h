// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

struct StringHashFunc {
  unsigned int operator()(const String& key) const {
    return CstringHashFunc()(key.GetConst());
  }
};

} // namespace dg
