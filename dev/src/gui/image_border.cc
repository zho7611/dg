// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "gui_first.h"

namespace dg {

void ImageBorder::ReadFrom(Stream* stream) {
  Border::ReadFrom(stream);
  StreamReadClass(stream, left_);
  StreamReadClass(stream, center_);
  StreamReadClass(stream, right_);
}

void ImageBorder::WriteTo(Stream* out_stream) {
  Border::WriteTo(out_stream);
  StreamWriteClass(out_stream, left_);
  StreamWriteClass(out_stream, center_);
  StreamWriteClass(out_stream, right_);
}

void ImageBorder::DrawGui(Canvas& in_canvas) {
  Point2 origin = in_canvas.origins_.Last() + local_rect_.GetLocation();
  Rect drawRect;
  // Draw left
  {
    drawRect.SetLocation(origin + left_.draw_rect.GetLocation());
    drawRect.SetSize(left_.draw_rect.size());
    g_renderer.translucent_sprite.DrawSprite(
        left_.texture.ptr(),
        drawRect,
        left_.tex_coord_rect,
        color);
  }
  const int centerWidth = local_rect_.GetWidth() - left_.draw_rect.GetWidth() - right_.draw_rect.GetWidth();
  // Draw center
  {
    drawRect.SetLocation(origin + center_.draw_rect.GetLocation());
    drawRect.SetSize(centerWidth, center_.draw_rect.GetHeight());
    g_renderer.translucent_sprite.DrawSprite(
        center_.texture.ptr(),
        drawRect,
        center_.tex_coord_rect,
        color);
  }
  // Draw right
  {
    drawRect.SetLocation(origin + center_.draw_rect.GetLocation() + Size2(centerWidth, 0));
    drawRect.SetSize(right_.draw_rect.size());
    g_renderer.translucent_sprite.DrawSprite(
        right_.texture.ptr(),
        drawRect,
        right_.tex_coord_rect,
        color);
  }
}

} // namespace dg
