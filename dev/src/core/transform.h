// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class Transform {
public:
  Transform();
  ~Transform();
  void Normalize();
  void ReadFrom(Stream* stream);
  void WriteTo(Stream* out_stream);
  void Translate(const Vector3& position);
  void LocalTranslate(const Vector3& position);
  Vector3& TransformCoord(Vector3& out_to, const Vector3& in_from) const;
  Vector3& TransformNormal(Vector3& out_to, const Vector3& in_from) const;
  Matrix ToMatrix() const {
    Matrix out;
    ToMatrix(out);
    return out;
  }
  void ToMatrix(Matrix& out_matrix) const;
  Matrix& ToInverseMatrix(Matrix& out_matrix) const;
  void Multiply(const Transform& lhs, const Transform& rhs);

  Vector3 position_;
  Quaternion rotation_;
  float scale_;
};

} // namespace dg
