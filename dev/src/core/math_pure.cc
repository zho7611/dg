// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "core_first.h"

namespace dg {

void Matrix::AffineTransformation(
    float scale,
    const Vector3* rotation_center,
    const Quaternion* rotation,
    const Vector3* translation) {
  Matrix tempMatrix, inverseCenterMatrix, rotationMatrirx, centerMatrix, translationMatrix;
  if (!rotation_center) {
    inverseCenterMatrix.MakeIdentity();
    centerMatrix.MakeIdentity();
  } else {
    inverseCenterMatrix.StoreTranslation(-rotation_center->x, -rotation_center->y, -rotation_center->z);
    centerMatrix.StoreTranslation(rotation_center->x, rotation_center->y, rotation_center->z);
  }
  if (!rotation) {
    rotationMatrirx.MakeIdentity();
  } else {
    rotationMatrirx.StoreRotationQuaternion(*rotation);
  }
  if (!translation) {
    translationMatrix.MakeIdentity();
  } else {
    translationMatrix.StoreTranslation(translation->x, translation->y, translation->z);
  }
  // Result = Scale * CenterInv * Quaternion * Center * Trans
  tempMatrix.StoreScale(scale, scale, scale);
  tempMatrix.StoreMultiply(tempMatrix, inverseCenterMatrix);
  tempMatrix.StoreMultiply(tempMatrix, rotationMatrirx);
  tempMatrix.StoreMultiply(tempMatrix, centerMatrix);
  StoreMultiply(tempMatrix, translationMatrix);
}

Matrix& Matrix::StoreMultiply(const Matrix& lhs, const Matrix& rhs) {
  Matrix result;
  result.m11_ = lhs.m11_*rhs.m11_ + lhs.m12_*rhs.m21_ + lhs.m13_*rhs.m31_ + lhs.m14_*rhs.m41_;
  result.m12_ = lhs.m11_*rhs.m12_ + lhs.m12_*rhs.m22_ + lhs.m13_*rhs.m32_ + lhs.m14_*rhs.m42_;
  result.m13_ = lhs.m11_*rhs.m13_ + lhs.m12_*rhs.m23_ + lhs.m13_*rhs.m33_ + lhs.m14_*rhs.m43_;
  result.m14_ = lhs.m11_*rhs.m14_ + lhs.m12_*rhs.m24_ + lhs.m13_*rhs.m34_ + lhs.m14_*rhs.m44_;
  result.m21_ = lhs.m21_*rhs.m11_ + lhs.m22_*rhs.m21_ + lhs.m23_*rhs.m31_ + lhs.m24_*rhs.m41_;
  result.m22_ = lhs.m21_*rhs.m12_ + lhs.m22_*rhs.m22_ + lhs.m23_*rhs.m32_ + lhs.m24_*rhs.m42_;
  result.m23_ = lhs.m21_*rhs.m13_ + lhs.m22_*rhs.m23_ + lhs.m23_*rhs.m33_ + lhs.m24_*rhs.m43_;
  result.m24_ = lhs.m21_*rhs.m14_ + lhs.m22_*rhs.m24_ + lhs.m23_*rhs.m34_ + lhs.m24_*rhs.m44_;
  result.m31_ = lhs.m31_*rhs.m11_ + lhs.m32_*rhs.m21_ + lhs.m33_*rhs.m31_ + lhs.m34_*rhs.m41_;
  result.m32_ = lhs.m31_*rhs.m12_ + lhs.m32_*rhs.m22_ + lhs.m33_*rhs.m32_ + lhs.m34_*rhs.m42_;
  result.m33_ = lhs.m31_*rhs.m13_ + lhs.m32_*rhs.m23_ + lhs.m33_*rhs.m33_ + lhs.m34_*rhs.m43_;
  result.m34_ = lhs.m31_*rhs.m14_ + lhs.m32_*rhs.m24_ + lhs.m33_*rhs.m34_ + lhs.m34_*rhs.m44_;
  result.m41_ = lhs.m41_*rhs.m11_ + lhs.m42_*rhs.m21_ + lhs.m43_*rhs.m31_ + lhs.m44_*rhs.m41_;
  result.m42_ = lhs.m41_*rhs.m12_ + lhs.m42_*rhs.m22_ + lhs.m43_*rhs.m32_ + lhs.m44_*rhs.m42_;
  result.m43_ = lhs.m41_*rhs.m13_ + lhs.m42_*rhs.m23_ + lhs.m43_*rhs.m33_ + lhs.m44_*rhs.m43_;
  result.m44_ = lhs.m41_*rhs.m14_ + lhs.m42_*rhs.m24_ + lhs.m43_*rhs.m34_ + lhs.m44_*rhs.m44_;
  CopyFrom(result);
  return *this;
}

// Matrix inverse using Cramer's rule
// From: ftp://download.intel.com/design/PentiumIII/sml/24504301.pdf
// #todo Compare with Laplace Expansion
// Http://www.geometrictools.com/Documentation/LaplaceExpansionTheorem.pdf
Matrix& Matrix::StoreInverse(const Matrix& rhs) {
  float tmp[12]; // Temp array for pairs
  float src[16]; // Array of transpose source matrix
  float det; // Determinant
  // Transpose rhs.items_16_
  for (int i = 0; i < 4; i++) {
    src[i] = rhs.items_16_[i*4];
    src[i + 4] = rhs.items_16_[i*4 + 1];
    src[i + 8] = rhs.items_16_[i*4 + 2];
    src[i + 12] = rhs.items_16_[i*4 + 3];
  }
  // Calculate pairs for first 8 elements (cofactors)
  tmp[0] = src[10] * src[15];
  tmp[1] = src[11] * src[14];
  tmp[2] = src[9] * src[15];
  tmp[3] = src[11] * src[13];
  tmp[4] = src[9] * src[14];
  tmp[5] = src[10] * src[13];
  tmp[6] = src[8] * src[15];
  tmp[7] = src[11] * src[12];
  tmp[8] = src[8] * src[14];
  tmp[9] = src[10] * src[12];
  tmp[10] = src[8] * src[13];
  tmp[11] = src[9] * src[12];
  // Calculate first 8 elements (cofactors)
  items_16_[0] = tmp[0]*src[5] + tmp[3]*src[6] + tmp[4]*src[7];
  items_16_[0] -= tmp[1]*src[5] + tmp[2]*src[6] + tmp[5]*src[7];
  items_16_[1] = tmp[1]*src[4] + tmp[6]*src[6] + tmp[9]*src[7];
  items_16_[1] -= tmp[0]*src[4] + tmp[7]*src[6] + tmp[8]*src[7];
  items_16_[2] = tmp[2]*src[4] + tmp[7]*src[5] + tmp[10]*src[7];
  items_16_[2] -= tmp[3]*src[4] + tmp[6]*src[5] + tmp[11]*src[7];
  items_16_[3] = tmp[5]*src[4] + tmp[8]*src[5] + tmp[11]*src[6];
  items_16_[3] -= tmp[4]*src[4] + tmp[9]*src[5] + tmp[10]*src[6];
  items_16_[4] = tmp[1]*src[1] + tmp[2]*src[2] + tmp[5]*src[3];
  items_16_[4] -= tmp[0]*src[1] + tmp[3]*src[2] + tmp[4]*src[3];
  items_16_[5] = tmp[0]*src[0] + tmp[7]*src[2] + tmp[8]*src[3];
  items_16_[5] -= tmp[1]*src[0] + tmp[6]*src[2] + tmp[9]*src[3];
  items_16_[6] = tmp[3]*src[0] + tmp[6]*src[1] + tmp[11]*src[3];
  items_16_[6] -= tmp[2]*src[0] + tmp[7]*src[1] + tmp[10]*src[3];
  items_16_[7] = tmp[4]*src[0] + tmp[9]*src[1] + tmp[10]*src[2];
  items_16_[7] -= tmp[5]*src[0] + tmp[8]*src[1] + tmp[11]*src[2];
  // Calculate pairs for second 8 elements (cofactors)
  tmp[0] = src[2]*src[7];
  tmp[1] = src[3]*src[6];
  tmp[2] = src[1]*src[7];
  tmp[3] = src[3]*src[5];
  tmp[4] = src[1]*src[6];
  tmp[5] = src[2]*src[5];
  tmp[6] = src[0]*src[7];
  tmp[7] = src[3]*src[4];
  tmp[8] = src[0]*src[6];
  tmp[9] = src[2]*src[4];
  tmp[10] = src[0]*src[5];
  tmp[11] = src[1]*src[4];
  // Calculate second 8 elements (cofactors)
  items_16_[8] = tmp[0]*src[13] + tmp[3]*src[14] + tmp[4]*src[15];
  items_16_[8] -= tmp[1]*src[13] + tmp[2]*src[14] + tmp[5]*src[15];
  items_16_[9] = tmp[1]*src[12] + tmp[6]*src[14] + tmp[9]*src[15];
  items_16_[9] -= tmp[0]*src[12] + tmp[7]*src[14] + tmp[8]*src[15];
  items_16_[10] = tmp[2]*src[12] + tmp[7]*src[13] + tmp[10]*src[15];
  items_16_[10]-= tmp[3]*src[12] + tmp[6]*src[13] + tmp[11]*src[15];
  items_16_[11] = tmp[5]*src[12] + tmp[8]*src[13] + tmp[11]*src[14];
  items_16_[11]-= tmp[4]*src[12] + tmp[9]*src[13] + tmp[10]*src[14];
  items_16_[12] = tmp[2]*src[10] + tmp[5]*src[11] + tmp[1]*src[9];
  items_16_[12]-= tmp[4]*src[11] + tmp[0]*src[9] + tmp[3]*src[10];
  items_16_[13] = tmp[8]*src[11] + tmp[0]*src[8] + tmp[7]*src[10];
  items_16_[13]-= tmp[6]*src[10] + tmp[9]*src[11] + tmp[1]*src[8];
  items_16_[14] = tmp[6]*src[9] + tmp[11]*src[11] + tmp[3]*src[8];
  items_16_[14]-= tmp[10]*src[11] + tmp[2]*src[8] + tmp[7]*src[9];
  items_16_[15] = tmp[10]*src[10] + tmp[4]*src[8] + tmp[9]*src[9];
  items_16_[15]-= tmp[8]*src[9] + tmp[11]*src[10] + tmp[5]*src[8];
  // Calculate determinant
  det=src[0]*items_16_[0]+src[1]*items_16_[1]+src[2]*items_16_[2]+src[3]*items_16_[3];
  // Calculate matrix inverse
  det = 1/det;
  for (int j = 0; j < 16; j++) {
    items_16_[j] *= det;
  }
  return *this;
}

Matrix& Matrix::StoreScale(float scale_x, float scale_y, float scale_z) {
  MakeIdentity();
  m11_ = scale_x;
  m22_ = scale_y;
  m33_ = scale_z;
  return *this;
}

bool Matrix::Decompose(Vector3& out_scale, Quaternion& out_rotation, Vector3& out_translation) {
  Matrix normalized;
  Vector3 basis;
  // Scale
  basis.x = items_44_[0][0];
  basis.y = items_44_[0][1];
  basis.z = items_44_[0][2];
  out_scale.x = basis.GetLength();
  basis.x = items_44_[1][0];
  basis.y = items_44_[1][1];
  basis.z = items_44_[1][2];
  out_scale.y = basis.GetLength();
  basis.x = items_44_[2][0];
  basis.y = items_44_[2][1];
  basis.z = items_44_[2][2];
  out_scale.z = basis.GetLength();;
  // Translation
  out_translation.x = items_44_[3][0];
  out_translation.y = items_44_[3][1];
  out_translation.z = items_44_[3][2];
  // Rotation
  if ((out_scale.x == 0.0f) || (out_scale.y == 0.0f) || (out_scale.z == 0.0f)) {
    return false;
  }
  normalized.items_44_[0][0] = items_44_[0][0]/out_scale.x;
  normalized.items_44_[0][1] = items_44_[0][1]/out_scale.x;
  normalized.items_44_[0][2] = items_44_[0][2]/out_scale.x;
  normalized.items_44_[1][0] = items_44_[1][0]/out_scale.y;
  normalized.items_44_[1][1] = items_44_[1][1]/out_scale.y;
  normalized.items_44_[1][2] = items_44_[1][2]/out_scale.y;
  normalized.items_44_[2][0] = items_44_[2][0]/out_scale.z;
  normalized.items_44_[2][1] = items_44_[2][1]/out_scale.z;
  normalized.items_44_[2][2] = items_44_[2][2]/out_scale.z;
  out_rotation.StoreRotationMatrix(normalized);
  return true;
}

// Reference:
//   http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/
void Quaternion::StoreRotationMatrix(const Matrix& rotation_matrix) {
  float S;
  float m00 = rotation_matrix.items_44_[0][0];
  float m11 = rotation_matrix.items_44_[1][1];
  float m22 = rotation_matrix.items_44_[2][2];
  float trace = m00 + m11 + m22;
  if (trace > 0) {
    S = .5f / sqrt(trace + 1.0f);
    w = .25f / S;
    x = (rotation_matrix.items_44_[1][2] - rotation_matrix.items_44_[2][1]) * S;
    y = (rotation_matrix.items_44_[2][0] - rotation_matrix.items_44_[0][2]) * S;
    z = (rotation_matrix.items_44_[0][1] - rotation_matrix.items_44_[1][0]) * S;
    return;
  }
  if ((m00 > m11) && (m00 > m22)) {
    S = 2.0f * sqrt(1.0f + m00 - m11 - m22);
    w = (rotation_matrix.items_44_[1][2] - rotation_matrix.items_44_[2][1]) / S;
    x = 0.25f * S;
    y = (rotation_matrix.items_44_[0][1] + rotation_matrix.items_44_[1][0]) / S;
    z = (rotation_matrix.items_44_[0][2] + rotation_matrix.items_44_[2][0]) / S;
  } else if (m11 > m22) {
    S = 2.0f * sqrt(1.0f + m11 - m00 - m22);
    w = (rotation_matrix.items_44_[2][0] - rotation_matrix.items_44_[0][2]) / S;
    x = (rotation_matrix.items_44_[0][1] + rotation_matrix.items_44_[1][0]) / S;
    y = 0.25f * S;
    z = (rotation_matrix.items_44_[1][2] + rotation_matrix.items_44_[2][1]) / S;
  } else {
    S = 2.0f * sqrt(1.0f + m22 - m00 - m11);
    w = (rotation_matrix.items_44_[0][1] - rotation_matrix.items_44_[1][0]) / S;
    x = (rotation_matrix.items_44_[0][2] + rotation_matrix.items_44_[2][0]) / S;
    y = (rotation_matrix.items_44_[1][2] + rotation_matrix.items_44_[2][1]) / S;
    z = 0.25f * S;
  }
}

void Quaternion::StoreRotationYawPitchRoll(float yaw, float pitch, float roll) {
  const float kSinHalfYaw = sinf(0.5f * yaw);
  const float kCosHalfYaw = cosf(0.5f * yaw);
  const float kSinHalfPitch = sinf(0.5f * pitch);
  const float kCosHalfPitch = cosf(0.5f * pitch);
  const float kSinHalfRoll = sinf(0.5f * roll);
  const float kCosHalfRoll = cosf(0.5f * roll);
  x = kCosHalfRoll * kSinHalfPitch * kCosHalfYaw + kSinHalfRoll * kCosHalfPitch * kSinHalfYaw;
  y = kCosHalfRoll * kCosHalfPitch * kSinHalfYaw - kSinHalfRoll * kSinHalfPitch * kCosHalfYaw;
  z = kSinHalfRoll * kCosHalfPitch * kCosHalfYaw - kCosHalfRoll * kSinHalfPitch * kSinHalfYaw;
  w = kCosHalfRoll * kCosHalfPitch * kCosHalfYaw + kSinHalfRoll * kSinHalfPitch * kSinHalfYaw;
}

void Quaternion::StoreRotationAxisAngle(const Vector3& axis, float angle) {
  Vector3 temp;
  temp.NormalizeFrom(axis);
  const float kSinHalfAngle = sin(.5f * angle);
  x = kSinHalfAngle * temp.x;
  y = kSinHalfAngle * temp.y;
  z= kSinHalfAngle * temp.z;
  w = cos(.5f * angle);
}

void Quaternion::ToAxisAngle(Vector3& out_axis, float& out_angle) {
  out_axis.x = x;
  out_axis.y = y;
  out_axis.z = z;
  out_angle = 2.0f * acos(w);
}

void Quaternion::StoreMultiply(
    const Quaternion& lhs,
    const Quaternion& rhs) {
  Quaternion result;
  result.x = rhs.w * lhs.x + rhs.x * lhs.w + rhs.y * lhs.z - rhs.z * lhs.y;
  result.y = rhs.w * lhs.y - rhs.x * lhs.z + rhs.y * lhs.w + rhs.z * lhs.x;
  result.z = rhs.w * lhs.z + rhs.x * lhs.y - rhs.y * lhs.x + rhs.z * lhs.w;
  result.w = rhs.w * lhs.w - rhs.x * lhs.x - rhs.y * lhs.y - rhs.z * lhs.z;
  CopyFrom(result);
}

void Matrix::OrthoLH(
    float width,
    float height,
    float near_plane,
    float far_plane) {
  MakeIdentity();
  m11_ = 2.f / width;
  m22_ = 2.f / height;
  m33_ = 1.0f / (far_plane - near_plane);
  m43_ = -near_plane / (far_plane - near_plane);
}

void Matrix::OrthoRH(
    float width,
    float height,
    float near_plane,
    float far_plane) {
  MakeIdentity();
  m11_ = 2.0f / width;
  m22_ = 2.0f / height;
  m33_ = 1.0f / (near_plane - far_plane);
  m43_ = near_plane / (near_plane - far_plane);
}

void Matrix::OrthoOffCenterLH(float in_left, float in_right, float in_bottom, float in_top, float near_plane, float far_plane) {
  MakeIdentity();
  m11_ = 2.f / (in_right - in_left);
  m22_ = 2.f / (in_top - in_bottom);
  m33_ = 1.f / (far_plane - near_plane);
  m41_ = (in_left + in_right) / (in_left - in_right);
  m42_ = (in_top + in_bottom) / (in_bottom - in_top);
  m43_ = near_plane / (near_plane - far_plane);
}

void Matrix::OrthoOffCenterRH(float in_left, float in_right, float in_bottom, float in_top, float near_plane, float far_plane) {
  MakeIdentity();
  m11_ = 2.f / (in_right - in_left);
  m22_ = 2.f / (in_top - in_bottom);
  m33_ = 1.f / (near_plane - far_plane);
  m41_ = (in_left + in_right) / (in_left - in_right);
  m42_ = (in_top + in_bottom) / (in_bottom - in_top);
  m43_ = near_plane / (near_plane - far_plane);
}

// Reference from http://msdn.microsoft.com/en-us/library/bb205350(v=vs.85).aspx
void Matrix::PerspectiveFovLH(
    float fov_y, // FOV in y direction, in radians
    float aspect_ratio, // Width / Height
    float near_plane, // Near plane z value
    float far_plane) { // Far plane z value
  MakeIdentity();
  const float kYScale = 1.0f / tan(.5f*fov_y);
  const float kXScale = kYScale / aspect_ratio;
  m11_ = kXScale;
  m22_ = kYScale;
  m44_ = 0.0f;
  m34_ = 1.0f;
#if defined(DG_OPENGL)
  m33_ = (far_plane + near_plane)/(far_plane - near_plane);
  m43_ = -2.0f*far_plane*near_plane/(far_plane - near_plane);
#else // DX
  m33_ = far_plane / (far_plane - near_plane);
  m43_ = (near_plane * far_plane) / (near_plane - far_plane);
#endif
}

// Reference from http://msdn.microsoft.com/en-us/library/bb205355(v=vs.85).aspx
void Matrix::PerspectiveFovRH(
    float fov_y, // FOV in y direction, in radians
    float aspect_ratio, // Width / Height
    float near_plane, // Near plane z value
    float far_plane) { // Far plane z value
  float kYScale = 1.0f / tan(.5f*fov_y);
  float kXScale = kYScale / aspect_ratio;
  m11_ = kXScale;
  m22_ = kYScale;
  m34_ = -1.0f;
  m44_ = 0.0f;
#if defined(DG_OPENGL)
  m33_ = (far_plane + near_plane)/(near_plane - far_plane);
  m43_ = 2.0f*far_plane*near_plane/(far_plane - near_plane);
#else // DX
  m33_ = far_plane / (near_plane - far_plane);
  m43_ = (near_plane * far_plane) / (near_plane - far_plane);
#endif
}

// Reference from http://msdn.microsoft.com/en-us/library/microsoft.windowsmobile.directx.matrix.perspectivefovlh(v=VS.90).aspx
void Matrix::LookAtLH(
    const Vector3* cameraPosition,
    const Vector3* cameraTarget,
    const Vector3* cameraUpVector) {
  Vector3 zaxis, xaxis, yaxis;
  zaxis.NormalizeFrom(*cameraTarget - *cameraPosition);
  xaxis.NormalizeFrom(Cross(*cameraUpVector, zaxis));
  yaxis = Cross(zaxis, xaxis);
  m11_ = xaxis.x;
  m12_ = yaxis.x;
  m13_ = zaxis.x;
  m14_ = 0;
  m21_ = xaxis.y;
  m22_ = yaxis.y;
  m23_ = zaxis.y;
  m24_ = 0;
  m31_ = xaxis.z;
  m32_ = yaxis.z;
  m33_ = zaxis.z;
  m34_ = 0;
  m41_ = -Dot(xaxis, *cameraPosition);
  m42_ = -Dot(yaxis, *cameraPosition);
  m43_ = -Dot(zaxis, *cameraPosition);
  m44_ = 1.0f;
}

// Reference from http://msdn.microsoft.com/en-us/library/microsoft.windowsmobile.directx.matrix.lookatrh.aspx
void Matrix::LookAtRH(
    const Vector3* cameraPosition,
    const Vector3* cameraTarget,
    const Vector3* cameraUpVector) {
  Vector3 zaxis, xaxis, yaxis;
  zaxis.NormalizeFrom(*cameraPosition - *cameraTarget);
  xaxis.NormalizeFrom(Cross(*cameraUpVector, zaxis));
  yaxis = Cross(zaxis, xaxis);
  m11_ = xaxis.x;
  m12_ = yaxis.x;
  m13_ = zaxis.x;
  m14_ = 0;
  m21_ = xaxis.y;
  m22_ = yaxis.y;
  m23_ = zaxis.y;
  m24_ = 0;
  m31_ = xaxis.z;
  m32_ = yaxis.z;
  m33_ = zaxis.z;
  m34_ = 0;
  m41_ = -Dot(xaxis, *cameraPosition);
  m42_ = -Dot(yaxis, *cameraPosition);
  m43_ = -Dot(zaxis, *cameraPosition);
  m44_ = 1.0f;
}

Matrix& Matrix::StoreTranspose(const Matrix& lhs) {
  Matrix result;
  for (int row_idx = 0; row_idx < 4; row_idx++) {
    for (int col_idx = 0; col_idx < 4; col_idx++) {
      result.items_44_[row_idx][col_idx] = lhs.items_44_[col_idx][row_idx];
    }
  }
  CopyFrom(result);
  return *this;
}

Matrix& Matrix::StoreRotationNormal(const Vector3& normal, float angle_in_radian) {
  MakeIdentity();
  const float kCosAngle = cosf(angle_in_radian);
  const float kSinAngle = sinf(angle_in_radian);
  const float kOneMinusCosAngle = 1.0f - kCosAngle;
  m11_ = kOneMinusCosAngle * normal.x * normal.x + kCosAngle;
  m12_ = kOneMinusCosAngle * normal.y * normal.x + kSinAngle * normal.z;
  m13_ = kOneMinusCosAngle * normal.z * normal.x - kSinAngle * normal.y;
  m21_ = kOneMinusCosAngle * normal.x * normal.y - kSinAngle * normal.z;
  m22_ = kOneMinusCosAngle * normal.y * normal.y + kCosAngle;
  m23_ = kOneMinusCosAngle * normal.z * normal.y + kSinAngle * normal.x;
  m31_ = kOneMinusCosAngle * normal.x * normal.z + kSinAngle * normal.y;
  m32_ = kOneMinusCosAngle * normal.y * normal.z - kSinAngle * normal.x;
  m33_ = kOneMinusCosAngle * normal.z * normal.z + kCosAngle;
  return *this;
}

Matrix& Matrix::StoreRotationAxis(const Vector3& axis, float angle_in_radian) {
  Check(!axis.IsZero());
  Vector3 normal;
  normal.NormalizeFrom(axis);
  StoreRotationNormal(normal, angle_in_radian);
  return *this;
}

Vector3& Vector3::StoreTransformCoord(const Vector3& point, const Matrix& matrix) {
  Vector3 result;
  float kInvNorm = 1.0f /
      (matrix.items_44_[0][3] * point.x +
          matrix.items_44_[1][3] * point.y +
          matrix.items_44_[2][3] * point.z +
          matrix.items_44_[3][3]);
  result.x = (matrix.items_44_[0][0] * point.x +
          matrix.items_44_[1][0] * point.y +
          matrix.items_44_[2][0] * point.z +
          matrix.items_44_[3][0]) * kInvNorm;
  result.y = (matrix.items_44_[0][1] * point.x +
          matrix.items_44_[1][1] * point.y +
          matrix.items_44_[2][1] * point.z +
          matrix.items_44_[3][1]) * kInvNorm;
  result.z = (matrix.items_44_[0][2] * point.x +
          matrix.items_44_[1][2] * point.y +
          matrix.items_44_[2][2] * point.z +
          matrix.items_44_[3][2]) * kInvNorm;
  *this = result;
  return *this;
}

Vector3& Vector3::StoreTransformNormal(const Vector3& normal, const Matrix& matrix) {
  Vector3 result;
  result.x = matrix.items_44_[0][0] * normal.x + matrix.items_44_[1][0] * normal.y + matrix.items_44_[2][0] * normal.z;
  result.y = matrix.items_44_[0][1] * normal.x + matrix.items_44_[1][1] * normal.y + matrix.items_44_[2][1] * normal.z;
  result.z = matrix.items_44_[0][2] * normal.x + matrix.items_44_[1][2] * normal.y + matrix.items_44_[2][2] * normal.z;
  *this = result;
  return *this;
}

const Vector3& Vector3::Normalize() {
  if (IsZero()) {
    return *this;
  }
  float kLength = GetLength();
  if (kLength == 0) {
    x = y = z = 0;
  } else {
    x /= kLength;
    y /= kLength;
    z /= kLength;
  }
  return *this;
}

Matrix& Matrix::StoreRotationQuaternion(const Quaternion& rotation) {
  MakeIdentity();
  m11_ = 1.0f - 2.0f * (rotation.y * rotation.y + rotation.z * rotation.z);
  m12_ = 2.0f * (rotation.x * rotation.y + rotation.z * rotation.w);
  m13_ = 2.0f * (rotation.x * rotation.z - rotation.y * rotation.w);
  m21_ = 2.0f * (rotation.x * rotation.y - rotation.z * rotation.w);
  m22_ = 1.0f - 2.0f * (rotation.x * rotation.x + rotation.z * rotation.z);
  m23_ = 2.0f * (rotation.y *rotation.z + rotation.x *rotation.w);
  m31_ = 2.0f * (rotation.x * rotation.z + rotation.y * rotation.w);
  m32_ = 2.0f * (rotation.y *rotation.z - rotation.x *rotation.w);
  m33_ = 1.0f - 2.0f * (rotation.x * rotation.x + rotation.y * rotation.y);
  return *this;
}

} // namespace dg
