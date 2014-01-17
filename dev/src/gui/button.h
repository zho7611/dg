// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class Button : public Label {
public:
  virtual void UpdateGui();
  virtual void DrawGui(Canvas& in_canvas);
};

} // namespace dg
