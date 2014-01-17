// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

// Font resource that matches a ttf file
class Font : public Resource {
public:
  friend class FontFace;

  Font();
  ~Font();
  virtual void CopyFrom(const uint8_t* data, int size) {
    blob_.CopyFrom(data, size);
  }
  virtual void ReadFrom(Stream* stream);
  virtual void WriteTo(Stream* out_stream);

  DG_DECLARE_RESOURCE(Font, Resource);

protected:
  AutoDeleteBlob blob_;
};

} // namespace dg
