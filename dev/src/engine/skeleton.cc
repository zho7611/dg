// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

void Skeleton::ReadFrom(Stream* stream) {
  StreamReadClass(stream, joints_);
}

void Skeleton::WriteTo(Stream* out_stream) {
  StreamWriteClass(out_stream, joints_);
}

const Joint* Skeleton::FindJointByIdSlow(const Name& joint_id) const {
  for (int idx = 0; idx < joints_.Count(); ++idx) {
    const Joint& joint = joints_.GetConst(idx);
    if (joint.id_.IsEqual(joint_id)) {
      return &joint;
    }
  }
  return 0;
}

} // namespace dg
