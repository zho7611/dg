// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class Light {
public:
  enum LightType {
    LightType_None,
    LightType_Ambient,
    LightType_Directional,
    LightType_Point,
  };

  ColorRGBA diffuse_color_;
  ColorRGBA ambient_color_;
  Vector3 position_;
  // Direction from the surface to the light source
  Vector3 direction_;
  bool is_cast_shadow_;
  bool is_enabled_;
  LightType type_;

  Light();
  virtual ~Light();
};

} // namespace dg
