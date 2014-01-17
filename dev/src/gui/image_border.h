// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class ImageBorder : public Border {
public:
  virtual void ReadFrom(Stream* stream);
  virtual void WriteTo(Stream* out_stream);
  virtual void DrawGui(Canvas& in_canvas);

  ImageCut left_;
  ImageCut right_;
  ImageCut center_;
};

} // namespace dg
