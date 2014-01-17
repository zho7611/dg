// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "core_first.h"

namespace dg {

void Matrix::OrthoLH(
    float width,
    float height,
    float near_plane,
    float far_plane) {
  ::D3DXMatrixOrthoLH(
      GetMatrixDx9(),
      width, height, near_plane, far_plane);
}

void Matrix::OrthoRH(float width, float height, float near_plane, float far_plane) {
  ::D3DXMatrixOrthoRH(
      GetMatrixDx9(),
      width, height,
      near_plane, far_plane);
}

void Matrix::OrthoOffCenterLH(float in_left, float in_right, float in_bottom, float in_top, float near_plane, float far_plane) {
  ::D3DXMatrixOrthoOffCenterLH(
      GetMatrixDx9(),
      in_left, in_right, in_bottom, in_top,
      near_plane, far_plane);
}

void Matrix::OrthoOffCenterRH(float in_left, float in_right, float in_bottom, float in_top, float near_plane, float far_plane) {
  ::D3DXMatrixOrthoOffCenterRH(
      GetMatrixDx9(),
      in_left, in_right, in_bottom, in_top,
      near_plane, far_plane);
}

void Matrix::PerspectiveFovLH(float fov_y, float aspect_ratio, float near_plane, float far_plane) {
  ::D3DXMatrixPerspectiveFovLH(
      GetMatrixDx9(),
      fov_y, aspect_ratio,
      near_plane, far_plane);
}

void Matrix::PerspectiveFovRH(float fov_y, float aspect_ratio, float near_plane, float far_plane) {
  ::D3DXMatrixPerspectiveFovRH(
      GetMatrixDx9(),
      fov_y, aspect_ratio, near_plane, far_plane);
}

void Matrix::AffineTransformation(
    float scale, const Vector3* rotation_center, const Quaternion* rotation,
    const Vector3* translation) {
  ::D3DXMatrixAffineTransformation(
      GetMatrixDx9(),
      scale,
      reinterpret_cast<const D3DXVECTOR3*>(rotation_center),
      reinterpret_cast<const D3DXQUATERNION*>(rotation),
      reinterpret_cast<const D3DXVECTOR3*>(translation));
}

void Matrix::LookAtLH(const Vector3* eye, const Vector3* at, const Vector3* up) {
  ::D3DXMatrixLookAtLH(
      GetMatrixDx9(),
      reinterpret_cast<const D3DXVECTOR3*>(eye),
      reinterpret_cast<const D3DXVECTOR3*>(at),
      reinterpret_cast<const D3DXVECTOR3*>(up));
}

void Matrix::LookAtRH(const Vector3* eye, const Vector3* at, const Vector3* up) {
  ::D3DXMatrixLookAtRH(
      GetMatrixDx9(),
      reinterpret_cast<const D3DXVECTOR3*>(eye),
      reinterpret_cast<const D3DXVECTOR3*>(at),
      reinterpret_cast<const D3DXVECTOR3*>(up));
}

Matrix& Matrix::StoreTranspose(const Matrix& lhs) {
  ::D3DXMatrixTranspose(GetMatrixDx9(), lhs.GetMatrixDx9());
  return *this;
}

Matrix& Matrix::StoreMultiply(const Matrix& lhs, const Matrix& rhs) {
  ::D3DXMatrixMultiply(GetMatrixDx9(), lhs.GetMatrixDx9(), rhs.GetMatrixDx9());
  return *this;
}

Matrix& Matrix::StoreInverse(const Matrix& rhs) {
  ::D3DXMatrixInverse(GetMatrixDx9(), 0, rhs.GetMatrixDx9());
  return *this;
}

Matrix& Matrix::StoreRotationNormal(const Vector3& normal, float angle_in_radian) {
  ::D3DXMatrixRotationAxis(GetMatrixDx9(),
      reinterpret_cast<const D3DXVECTOR3*>(normal.GetFloatsConst()), angle_in_radian);
  return *this;
}

Matrix& Matrix::StoreRotationAxis(const Vector3& axis, float angle_in_radian) {
  ::D3DXMatrixRotationAxis(GetMatrixDx9(),
      reinterpret_cast<const D3DXVECTOR3*>(axis.GetFloatsConst()), angle_in_radian);
  return *this;
}

Matrix& Matrix::StoreScale(float scale_x, float scale_y, float scale_z) {
  ::D3DXMatrixScaling(GetMatrixDx9(), scale_x, scale_y, scale_z);
  return *this;
}

Matrix& Matrix::StoreRotationQuaternion(const Quaternion& rotation) {
  ::D3DXMatrixRotationQuaternion(GetMatrixDx9(),
      reinterpret_cast<const D3DXQUATERNION*>(&rotation));
  return *this;
}

bool Matrix::Decompose(Vector3& out_scale, Quaternion& out_rotation, Vector3& out_translation) {
  if (FAILED(::D3DXMatrixDecompose(
      reinterpret_cast<D3DXVECTOR3*>(&out_scale),
      reinterpret_cast<D3DXQUATERNION*>(&out_rotation),
      reinterpret_cast<D3DXVECTOR3*>(&out_translation),
      GetMatrixDx9()))) {
    return false;
  }
  return true;
}

void Quaternion::StoreRotationMatrix(const Matrix& rotation_matrix) {
  ::D3DXQuaternionRotationMatrix(
      reinterpret_cast<D3DXQUATERNION*>(this), rotation_matrix.GetMatrixDx9());
}

void Quaternion::StoreRotationYawPitchRoll(float yaw, float pitch, float roll) {
  ::D3DXQuaternionRotationYawPitchRoll(
      reinterpret_cast<D3DXQUATERNION*>(this),
      yaw,
      pitch,
      roll);
}

void Quaternion::StoreRotationAxisAngle(const Vector3& axis, float angle) {
  ::D3DXQuaternionRotationAxis(
      reinterpret_cast<D3DXQUATERNION*>(this),
      reinterpret_cast<const D3DXVECTOR3*>(&axis),
      angle);
}

void Quaternion::ToAxisAngle(Vector3& out_axis, float& out_angle) {
  ::D3DXQuaternionToAxisAngle(
      reinterpret_cast<const D3DXQUATERNION*>(this),
      reinterpret_cast<D3DXVECTOR3*>(&out_axis),
      &out_angle);
}

// #storexxx  StoreXXX() methods overwrite the existing values
void Quaternion::StoreMultiply(const Quaternion& lhs, const Quaternion& rhs) {
  ::D3DXQuaternionMultiply(
      reinterpret_cast<D3DXQUATERNION*>(this),
      reinterpret_cast<const D3DXQUATERNION*>(&lhs),
      reinterpret_cast<const D3DXQUATERNION*>(&rhs));
}

// #storexxx
Vector3& Vector3::StoreTransformCoord(const Vector3& point, const Matrix& matrix) {
  ::D3DXVec3TransformCoord(
      reinterpret_cast<D3DXVECTOR3*>(this),
      reinterpret_cast<const D3DXVECTOR3*>(&point),
      matrix.GetMatrixDx9());
  return *this;
}

// #storexxx
Vector3& Vector3::StoreTransformNormal(const Vector3& normal, const Matrix& matrix) {
  ::D3DXVec3TransformNormal(
      reinterpret_cast<D3DXVECTOR3*>(this),
      reinterpret_cast<const D3DXVECTOR3*>(&normal),
      matrix.GetMatrixDx9());
  return *this;
}

const Vector3& Vector3::Normalize() {
  ::D3DXVec3Normalize(
      reinterpret_cast<D3DXVECTOR3*>(this),
      reinterpret_cast<D3DXVECTOR3*>(this));
  return *this;
}

} // namespace dg
