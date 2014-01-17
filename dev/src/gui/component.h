// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

enum ActiveState {
    ActiveState_Unknown = 0,
    ActiveState_Hover = 1,
    ActiveState_Selected = 2,
    ActiveState_Disabled = 4,
};

class Component : public RefCount {
public:
  virtual void ReadFromPath(const Cstr* path);
  virtual void WriteToPath(const Cstr* path);
  virtual void ReadFrom(Stream* stream) {
    DG_UNIMPLEMENTED();
  }
  virtual void WriteTo(Stream* out_stream) {
    DG_UNIMPLEMENTED();
  }
  virtual void UpdateGui() DG_PURE;
  virtual void DrawGui(Canvas& in_canvas) DG_PURE;
  ActiveState GetState() const {
    return state_;
  }
  void SetLocalRect(const Rect& rect) {
    local_rect_ = rect;
  }
  void SetLocalRect(int left, int top, int right, int bottom);

protected:
  ActiveState state_;
  Rect local_rect_;
};

} // namespace dg
