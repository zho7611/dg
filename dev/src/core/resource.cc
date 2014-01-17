// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "core_first.h"

namespace dg {

Resource::Resource() {}

Resource::~Resource() {
  Check(!resource_state_.IsBound());
  // Do not call virtual UnbindResource() or ClearResource() here
}

void Resource::BindResource() {
  if (resource_state_.IsLoaded() && !resource_state_.IsBound()) {
    resource_state_.SetBound(true);
    InitResource();
  }
}

void Resource::UnbindResource() {
  if (resource_state_.IsBound()) {
    resource_state_.SetBound(false);
    ClearResource();
  }
}

bool Resource::SerializeToDevice() {
  if (resource_state_.IsBound()) {
    resource_state_.SetDirty(false);
    return true;
  }
  return false;
}

bool Resource::SerializeFromDevice() {
  if (resource_state_.IsBound()) {
    resource_state_.SetDirty(false);
    return true;
  }
  return false;
}

ResourcePath Resource::GetResourcePath() const {
  Check(!resource_name_.IsEmpty());
  Resource* owner = resource_owner_.ptr();
  if (owner) {
    return ResourcePath(owner->resource_name_.GetConst(), resource_name_.GetConst());
  }
  return ResourcePath(resource_name_.GetConst());
}

ClassHash Resource::GetClassNameHash() {
  return GetStringHashCode(GetClassNameStr());
}

} // namespace dg
