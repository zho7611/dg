// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "core_first.h"

namespace dg {

Transform::Transform() :
  scale_(1.0f),
  position_(0, 0, 0),
  rotation_(0, 0, 0, 1.0f) {}

Transform::~Transform() {}

void Transform::ReadFrom(Stream* stream) {
  StreamReadFlat(stream, position_);
  StreamReadFlat(stream, rotation_);
  StreamReadFlat(stream, scale_);
}

void Transform::WriteTo(Stream* out_stream) {
  StreamWriteFlat(out_stream, position_);
  StreamWriteFlat(out_stream, rotation_);
  StreamWriteFlat(out_stream, scale_);
}

void Transform::ToMatrix(Matrix& out_matrix) const {
  out_matrix.AffineTransformation(
      scale_,
      NULL,
      &rotation_,
      &position_);
}

Matrix& Transform::ToInverseMatrix(Matrix& out_matrix) const {
  return out_matrix.StoreInverse(ToMatrix());
}

Vector3& Transform::TransformNormal(Vector3& out_to, const Vector3& in_from) const {
  out_to.StoreTransformNormal(in_from, ToMatrix());
  return out_to;
}

Vector3& Transform::TransformCoord(Vector3& out_to, const Vector3& in_from) const {
  out_to.StoreTransformCoord(in_from, ToMatrix());
  return out_to;
}

void Transform::LocalTranslate(const Vector3& position) {
  Vector3 transformed_pos;
  position_.Add(TransformNormal(transformed_pos, position));
}

void Transform::Translate(const Vector3& position) {
  position_.Add(position);
}

void Transform::Multiply(const Transform &lhs, const Transform &rhs) {
  // According to the book:
  // Http://books.google.co.kr/books?id=T8d4hBklV3UC&lpg=PA145&ots=XRLZb1Zbv9&dq=srt%20transform&hl=ko&pg=PA145#v=onepage&q=srt%20transform&f=false
  // 3D Game Engine Design by David H. Eberly
  // 4.1.1. Transform > Local Transforms > The product of two SRT-transforms is
  rhs.TransformCoord(position_, lhs.position_);
  position_.Add(rhs.position_);
  rotation_.StoreMultiply(lhs.rotation_, rhs.rotation_);
  scale_ = lhs.scale_ * rhs.scale_;
}

void Transform::Normalize() {
  position_ = Vector3::kZero;
  rotation_ = Quaternion::kIdentity;
  scale_ = 1.0f;
}

} // namespace dg
