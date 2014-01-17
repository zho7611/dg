// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class ImageCut {
public:
  void ReadFrom(Stream* stream);
  void WriteTo(Stream* out_stream);

  Rect draw_rect;
  Rect tex_coord_rect;
  Ptr<Texture> texture;
};

} // namespace dg
