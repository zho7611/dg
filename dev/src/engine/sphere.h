// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class Sphere {
public:
  // Number of segments in horizontal axis
  int num_segments_;
  Vector3 center_;
  float radius_;
};

} // namespace dg
