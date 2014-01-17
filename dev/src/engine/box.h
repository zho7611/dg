// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class Box {
public:
  Box() : size_(Vector3::kOne) {}

  Transform center_;
  Vector3 size_;
};

} // namespace dg
