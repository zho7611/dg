// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class Texture;
class SpriteShaderEffect;

// Draw a textue quad on the screen with color
class Sprite {
public:
  enum Type {
    Type_None = 0,
    Type_Opaque,
    Type_Translucent,
    Type_Font,
  };

  Type sprite_type_;

  Sprite();
  ~Sprite();
  void Init();
  void Clear();
  // If the texture is NULL, then default white texture will be used
  void DrawSprite(Texture* texture, const Point2& position);
  void DrawSprite(
      Texture* texture,
      const Point2& position,
      const ColorInt& blend_color);
  void DrawSprite(
      Texture* texture,
      const Rect& rect,
      const ColorInt& blend_color);
  void DrawSprite(
      Texture* texture,
      const Rect& rect,
      const Rect& tex_coord_rect,
      const ColorInt& blend_color);

private:
  Ptr<SpriteShaderEffect> effect_sprite_;
  ResourceState resource_state_;
};

} // namespace dg
