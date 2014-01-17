// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class Border : public Component {
public:
  enum Style {
      Style_Null = 0,
      Style_Line,
      Style_Default,
  };

  Border();
  virtual void ReadFromPath(const Cstr* path);
  virtual void WriteToPath(const Cstr* path);
  virtual void ReadFrom(Stream* stream);
  virtual void WriteTo(Stream* out_stream);
  virtual void DrawGui(Canvas& in_canvas);
  virtual void UpdateGui() {}

  Style style;
  ColorInt color;

private:
  void DrawGuiLineBorder(Canvas& in_canvas);
  void DrawGuiDefaultBorder(Canvas& in_canvas);
};

} // namespace dg
