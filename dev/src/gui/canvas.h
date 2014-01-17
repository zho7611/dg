// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class Canvas {
public:
  Canvas();
  void PreRender();
  void PostRender();
  void ClearCanvas();

  Ptr<RenderTarget> target_;
  Stack<Point2> origins_;
  ColorInt clear_color_;
};

} // namespace dg
