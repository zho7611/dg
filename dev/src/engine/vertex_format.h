// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {
namespace VertexFormat {

struct FormatPosCol {
  Vector3 position;
  ColorInt color;
};

struct FormatPosTex {
  Vector3 position;
  Vector2 tex_coord;
};

struct FormatPosTexCol {
  Vector3 position;
  Vector2 tex_coord;
  ColorInt color;
};

struct FormatPosNorTexTan {
  Vector3 position;
  Vector3 normal;
  Vector2 tex_coord;
  Vector4 tangent;
};

// The maximum number of affecting joints on a vertex
static const int kMaxJointInfluence = 4;

struct FormatPosNorTexTanJi4Jw4 {
  Vector3 position;
  Vector3 normal;
  Vector2 tex_coord;
  Vector4 tangent;
  int joint_indices[kMaxJointInfluence];
  float joint_weights[kMaxJointInfluence];
};

} // namespace VertexFormat
} // namespace dg
