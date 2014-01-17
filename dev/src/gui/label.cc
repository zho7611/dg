// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "gui_first.h"

namespace dg {

Label::Label() {
  text_color = DG_MAKE_COLOR_INT(255, 0, 0, 0);
}

void Label::UpdateGui() {
  DG_UNIMPLEMENTED();
}

void Label::DrawGui(Canvas& canvas) {
  Point2 location = canvas.origins_.Last();
  // Draw the border
  if (border) {
    border->SetLocalRect(local_rect_);
    border->DrawGui(canvas);
  }
  // Draw the text
  {
    Point2 text_location = local_rect_.GetLocation();
    text.DrawText(canvas, text_location, text_color);
  }
}

} // namespace dg
