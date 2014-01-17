// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

Sprite::Sprite() :
  sprite_type_(Type_None) {
}

Sprite::~Sprite() {
  Clear();
  Check(effect_sprite_ == 0);
}

void Sprite::Init() {
  // Should be set beforehand
  Check(sprite_type_ != Type_None);
  if (!effect_sprite_) {
    effect_sprite_ = LoadResource<SpriteShaderEffect>(
        g_engine_config->sprite_effect_path_.Get());
  }
}

void Sprite::Clear() {
  effect_sprite_ = NULL;
}

void Sprite::DrawSprite(Texture* texture, const Point2& position) {
  Rect rect(position.x, position.y, texture->size());
  DrawSprite(texture, rect, DG_MAKE_COLOR_INT(255, 255, 255, 255));
}

void Sprite::DrawSprite(
    Texture* texture,
    const Point2& position,
    const ColorInt& blend_color) {
  Rect rect(position.x, position.y, texture->size());
  DrawSprite(texture, rect, blend_color);
}

void Sprite::DrawSprite(
    Texture* texture,
    const Rect& rect,
    const ColorInt& blend_color) {
  Check(sprite_type_ != Type_None);
  Rect tex_coord_rect(0, 0, (texture) ? texture->size() : Size2(0, 0));
  effect_sprite_->PreRender(sprite_type_);
  effect_sprite_->SubmitQuad(this, texture, rect, tex_coord_rect, blend_color);
  effect_sprite_->PostRender();
}

void Sprite::DrawSprite(
    Texture* texture,
    const Rect& rect,
    const Rect& tex_coord_rect,
    const ColorInt& blend_color) {
  effect_sprite_->PreRender(sprite_type_);
  effect_sprite_->SubmitQuad(this, texture, rect, tex_coord_rect, blend_color);
  effect_sprite_->PostRender();
}

} // namespace dg
