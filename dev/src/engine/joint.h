// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class Joint {
public:
  static const int kInvalidIndex = -1;

  // Unique name in the skeleton
  Name id_;
  // Index of the skeleton joints
  int index_;
  // Parent index of the skeleton joints
  int parent_index_;
  // Inverse of the joint's binding matrix
  // Model-to-joint matrix
  Matrix inverse_bind_matrix_;

  Joint();
  ~Joint();
  void ReadFrom(Stream* stream);
  void WriteTo(Stream* out_stream);
};

} // namespace dg
