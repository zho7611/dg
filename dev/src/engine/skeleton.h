// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class Skeleton : public Resource {
public:
  typedef FixedArray<Joint> JointsType;

  Joint& GetJoint(int idx) {
    return joints_.Get(idx);
  }
  const Joint& GetJoint(int idx) const {
    return joints_.GetConst(idx);
  }
  void Resize(int new_size) {
    joints_.Resize(new_size);
  }
  int GetNumJoints() const {
    return joints_.Count();
  }
  const Joint* FindJointByIdSlow(const Name& joint_id) const;
  virtual void ReadFrom(Stream* stream);
  virtual void WriteTo(Stream* out_stream);

private:
  JointsType joints_;

  DG_DECLARE_RESOURCE(Skeleton, Resource);
};

} // namespace dg
