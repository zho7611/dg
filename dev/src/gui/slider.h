// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class Slider : public Component {
public:
  float min_value;
  float max_value;
  float step_size;
  float default_value;
};

} // namespace dg
