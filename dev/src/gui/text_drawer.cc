// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "gui_first.h"

namespace dg {

void TextDrawer::DrawText(
    Canvas& canvas, FontText* font_text, Point2& position,
    ColorInt color) {
  FontFace* face = font_text->GetFontFace();
  Texture* tex = font_text->GetTexture();
  // Check whether the size is OK to be rendered
  {
    Check(tex->size().x > 0);
    Check(tex->size().y > 0);
  }
  // If the texture is ready, render the text onto the texture
  Point2 pos = font_text->GetPenPosition();
  face->RenderText(tex, font_text->GetText(), pos);
  // Draw the text sprite with proper position
  {
    Rect tex_rect(tex->size());
    Point2 location = canvas.origins_.Last() + position;
    tex_rect.SetLocation(location);
    canvas.PreRender();
    g_renderer.font_sprite.DrawSprite(tex, tex_rect, color);
    canvas.PostRender();
  }
}

} // namespace dg
