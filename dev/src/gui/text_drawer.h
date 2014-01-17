// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class FontText;

class TextDrawer {
public:
  static void DrawText(Canvas& canvas, FontText* font_text, Point2& position, ColorInt color);

private:
  // Draw-related members, like effect and vertex-decl
};

} // namespace dg
