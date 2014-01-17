// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

// SkinJoint is a small set of the skeleton joints
// In contrast of the skeleton joints, SkinJoint has the matrix for the animation
// The skeleton joint doesn't have any instance-specific information
class SkinJoint {
public:
  int skeleton_joint_index_; // Index to the skeleton joints
  int skin_parent_index_; // Parent index to the skin joints, not in the skeleton joints
  Matrix animation_matrix_; // In local joint frame
  Matrix bind_to_model_matrix_; // From bind reference pose frame to post-animated model frame

  void ReadFrom(Stream* stream);
  void WriteTo(Stream* out_stream);
};

} // namespace dg
