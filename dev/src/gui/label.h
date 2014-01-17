// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class Label : public Component {
public:
  Label();

  virtual void UpdateGui();
  virtual void DrawGui(Canvas& canvas);

  FontText text;
  ColorInt text_color;
  Ptr<Border> border;
};

} // namespace dg
