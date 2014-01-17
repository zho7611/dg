// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class LightManager {
public:
  LightManager();
  ~LightManager();
  void Init();
  void Clear();

  Light* global_light_;
};

} // namespace dg
