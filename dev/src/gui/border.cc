// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "gui_first.h"

namespace dg {

Border::Border() :
  style(Style_Null) {
  color = DG_MAKE_COLOR_INT(255, 255, 255, 255);
}

void Border::ReadFromPath(const Cstr* path) {
  Ptr<Chunk> chunk = CreateReadChunk(path);
  ReadFrom(chunk.ptr());
}

void Border::WriteToPath(const Cstr* path) {
  Ptr<Chunk> chunk = CreateWriteChunk(path);
  WriteTo(chunk.ptr());
}

void Border::ReadFrom(Stream* stream) {
  StreamReadFlat(stream, style);
  StreamReadFlat(stream, color);
}

void Border::WriteTo(Stream* out_stream) {
  StreamWriteFlat(out_stream, style);
  StreamWriteFlat(out_stream, color);
}

void Border::DrawGui(Canvas& in_canvas) {
  switch (style) {
  case Style_Null:
    return;
    break;
  case Style_Line:
    DrawGuiLineBorder(in_canvas);
    break;
  case Style_Default:
    DrawGuiDefaultBorder(in_canvas);
    break;
  }
}

void Border::DrawGuiLineBorder(Canvas& in_canvas) {
  Line2Drawer& drawer = g_line2d_drawer;
  drawer.PreRender();
  {
    Point2 draw_point[4];
    int width = local_rect_.GetWidth();
    int height = local_rect_.GetHeight();
    draw_point[0] = in_canvas.origins_.Last();
    draw_point[1] = draw_point[0] + Point2(width, 0);
    draw_point[2] = draw_point[1] + Point2(0, height);
    draw_point[3] = draw_point[0] + Point2(0, height);
    drawer.DrawLine2(draw_point[0], draw_point[1], color);
    drawer.DrawLine2(draw_point[1], draw_point[2], color);
    drawer.DrawLine2(draw_point[2], draw_point[3], color);
    drawer.DrawLine2(draw_point[3], draw_point[0], color);
  }
  drawer.PostRender();
}

void Border::DrawGuiDefaultBorder(Canvas& in_canvas) {
  Line2Drawer& drawer = g_line2d_drawer;
  drawer.PreRender();
  {
    Point2 draw_point[2];
    int width = local_rect_.GetWidth();
    int height = local_rect_.GetHeight();
    Point2 origin = in_canvas.origins_.Last();
    int decrement = 0;
    int gradient = 0;
    for (int row = 0; row < height; ++row) {
      draw_point[0] = origin + Point2(0, row);
      draw_point[1] = origin + Point2(width, row);
      gradient = height - row + decrement;
      ColorInt color = DG_MAKE_COLOR_INT(
          255, gradient, gradient, gradient);
      decrement *= decrement + 4;
      drawer.DrawLine2(draw_point[0], draw_point[1], color);
    }
  }
  drawer.PostRender();
  DrawGuiLineBorder(in_canvas);
}

} // namespace dg
