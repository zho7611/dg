// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class ResourceState {
private:
  enum State {
    State_None = 0,
    State_Loaded = 1 << 0,
    State_Bound = 1 << 1,
    State_Dirty = 1 << 2,
  };

  State state_;

public:
  ResourceState() : state_(State_None) {}
  bool IsLoaded() const {
    return (state_ & (int)State_Loaded) != 0;
  }
  bool IsBound() const {
    return (state_ & (int)State_Bound) != 0;
  }
  bool IsDirty() const {
    return (state_ & (int)State_Dirty) != 0;
  }
  void SetLoaded(bool value) {
    state_ = (value) ? State((int)state_ | (int)State_Loaded) : State((int)state_ & ~(int)State_Loaded);
  }
  void SetBound(bool value) {
    state_ = (value) ? State((int)state_ | (int)State_Bound) : State((int)state_ & ~(int)State_Bound);
  }
  void SetDirty(bool value) {
    state_ = (value) ? State((int)state_ | (int)State_Dirty) : State((int)state_ & ~(int)State_Dirty);
  }
};

} // namespace dg
