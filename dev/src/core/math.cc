// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "core_first.h"

namespace dg {

void ColorRGBA::FromColorInt(ColorInt color) {
#if defined(DG_USE_COLOR_INT_AS_ARGB)
  b = float((color >> 0) & 0xff) / 255.f;
  g = float((color >> 8) & 0xff) / 255.f;
  r = float((color >> 16) & 0xff) / 255.f;
  a = float((color >> 24) & 0xff) / 255.f;
#elif defined(DG_USE_COLOR_INT_AS_ABGR)
  r = float((color >> 0) & 0xff) / 255.f;
  g = float((color >> 8) & 0xff) / 255.f;
  b = float((color >> 16) & 0xff) / 255.f;
  a = float((color >> 24) & 0xff) / 255.f;
#endif
}

void ColorRGBA::ReadFrom(Stream* stream) {
  StreamReadFlat(stream, *this);
}

void ColorRGBA::WriteTo(Stream* out_stream) {
  StreamWriteFlat(out_stream, *this);
}

ColorInt ColorRGBA::ToColorInt() const {
#if defined(DG_USE_COLOR_INT_AS_ARGB)
  return
      (int(a*255.f) << 24) +
      (int(r*255.f) << 16) +
      (int(g*255.f) << 8) +
      (int(b*255.f) << 0);
#elif defined(DG_USE_COLOR_INT_AS_ABGR)
  return
      (int(a*255.f) << 24) +
      (int(b*255.f) << 16) +
      (int(g*255.f) << 8) +
      (int(r*255.f) << 0);
#endif
}

Quaternion Quaternion::kIdentity(0, 0, 0, 1);

void Quaternion::CopyFrom(const Quaternion& rhs) {
  x = rhs.x;
  y = rhs.y;
  z = rhs.z;
  w = rhs.w;
}

void Quaternion::ReadFrom(Stream* stream) {
  StreamReadFlat(stream, *this);
}

void Quaternion::WriteTo(Stream* out_stream) {
  StreamWriteFlat(out_stream, *this);
}

Matrix Matrix::kIdentity(
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1);
Point2 Point2::kZero(0, 0);
Point3 Point3::kZero(0, 0, 0);
Vector3 Vector3::kZero(0, 0, 0);
Vector3 Vector3::kOne(1, 1, 1);
Vector2 Vector2::kZero(0, 0);
Vector2 Vector2::kOne(1, 1);
Vector4 Vector4::kZero(0, 0, 0, 0);
Vector4 Vector4::kOne(1, 1, 1, 1);

Matrix::Matrix(bool is_make_identity) {
  if (is_make_identity) {
    MakeIdentity();
  }
}

void Matrix::MakeIdentity() {
  m11_ = m22_ = m33_ = m44_ = 1.f;
  m12_ = m13_ = m14_ = m21_ = m23_ = m24_ = m31_ = m32_ = m34_ = m41_ = m42_ = m43_ = 0;
}

Matrix::Matrix(const float* rhs) {
  MyMemCpy(&items_44_[0][0], rhs, 16*sizeof(float));
}

Matrix::Matrix(const Matrix& rhs) {
  MyMemCpy(&items_44_[0][0], &rhs.items_44_[0][0], 16*sizeof(float));
}

Matrix::Matrix(
    float m11, float m12, float m13, float m14,
    float m21, float m22, float m23, float m24,
    float m31, float m32, float m33, float m34,
    float m41, float m42, float m43, float m44)
  :
  m11_(m11), m12_(m12), m13_(m13), m14_(m14),
  m21_(m21), m22_(m22), m23_(m23), m24_(m24),
  m31_(m31), m32_(m32), m33_(m33), m34_(m34),
  m41_(m41), m42_(m42), m43_(m43), m44_(m44) {}

void Matrix::Set16(
    float m11, float m12, float m13, float m14,
    float m21, float m22, float m23, float m24,
    float m31, float m32, float m33, float m34,
    float m41, float m42, float m43, float m44) {
  m11_ = m11;
  m12_ = m12;
  m13_ = m13;
  m14_ = m14;
  m21_ = m21;
  m22_ = m22;
  m23_ = m23;
  m24_ = m24;
  m31_ = m31;
  m32_ = m32;
  m33_ = m33;
  m34_ = m34;
  m41_ = m41;
  m42_ = m42;
  m43_ = m43;
  m44_ = m44;
}

float Matrix::Get(int idx) const {
  Check((idx >= 0) && (idx < 16));
  return items_16_[idx];
}

float Matrix::Get(int row, int col) const {
  Check((row >= 0) && (row < 4));
  Check((col >= 0) && (col < 4));
  return items_44_[row][col];
}

void Matrix::CopyFrom(const Matrix& rhs) {
  for (int idx = 0; idx < 16; ++idx) {
    items_16_[idx] = rhs.items_16_[idx];
  }
}

void Matrix::Set(int idx, float value) {
  Check((idx >= 0) && (idx < 16));
  items_16_[idx] = value;
}

void Matrix::Set(int row, int col, float value) {
  Check((row >= 0) && (row < 4));
  Check((col >= 0) && (col < 4));
  items_44_[row][col] = value;
}

void Matrix::Translate(float in_x, float in_y, float in_z) {
  items_44_[3][0] += in_x;
  items_44_[3][1] += in_y;
  items_44_[3][2] += in_z;
}

void Matrix::GetPosition(Vector3 &out_position) const {
  out_position.Set(
      items_44_[3][0],
      items_44_[3][1],
      items_44_[3][2]);
}

void Matrix::GetColumnAxis(int column_index, Vector3& out_axis) const {
  Check(column_index < 3);
  Check(column_index >= 0);
  out_axis.Set(items_44_[0][column_index], items_44_[1][column_index], items_44_[2][column_index]);
}

void Matrix::set_position(const Vector3& position) {
  m41_ = position.x;
  m42_ = position.y;
  m43_ = position.z;
}

Matrix& Matrix::StoreTranslation(float in_x, float in_y, float in_z) {
  MakeIdentity();
  m41_ = in_x;
  m42_ = in_y;
  m43_ = in_z;
  return *this;
}

void Matrix::ReadFrom(Stream* stream) {
  StreamReadFlat(stream, *this);
}

void Matrix::WriteTo(Stream* out_stream) {
  StreamWriteFlat(out_stream, *this);
}

Matrix Matrix::operator*(const Matrix& rhs) const {
  Matrix tmp;
  tmp.StoreMultiply(*this, rhs);
  return tmp;
}

Quaternion::Quaternion(float in_x, float in_y, float in_z, float in_w) :
  x(in_x), y(in_y), z(in_z), w(in_w) {}

Vector2::Vector2(const Vector2& rhs) :
  x(rhs.x), y(rhs.y) {}

Vector2::Vector2(float in_x, float in_y) :
  x(in_x), y(in_y) {}

Vector2::Vector2(const float* in_xy) :
  x(in_xy[0]), y(in_xy[1]) {}

Vector2& Vector2::operator-() {
  x = -x;
  y = -y;
  return *this;
}

Vector2& Vector2::operator=(const Vector2& rhs) {
  if (this == &rhs) {
    return *this;
  }
  x = rhs.x;
  y = rhs.y;
  return *this;
}

Vector2& Vector2::operator*=(const Vector2& rhs) {
  x *= rhs.x;
  y *= rhs.y;
  return *this;
}

Vector2& Vector2::operator/=(const Vector2& rhs) {
  x /= rhs.x;
  y /= rhs.y;
  return *this;
}

Vector2& Vector2::operator+=(const Vector2& rhs) {
  x += rhs.x;
  y += rhs.y;
  return *this;
}

Vector2& Vector2::operator-=(const Vector2& rhs) {
  x -= rhs.x;
  y -= rhs.y;
  return *this;
}

Vector2& Vector2::operator*=(float rhs) {
  x *= rhs;
  y *= rhs;
  return *this;
}

Vector2& Vector2::operator/=(float rhs) {
  x /= rhs;
  y /= rhs;
  return *this;
}

Vector2 Vector2::operator*(const Vector2& rhs) const {
  Vector2 value;
  value.x = x * rhs.x;
  value.y = y * rhs.y;
  return value;
}

Vector2 Vector2::operator/(const Vector2& rhs) const {
  Vector2 value;
  value.x = x / rhs.x;
  value.y = y / rhs.y;
  return value;
}

Vector2 Vector2::operator+(const Vector2& rhs) const {
  Vector2 value;
  value.x = x + rhs.x;
  value.y = y + rhs.y;
  return value;
}

Vector2 Vector2::operator-(const Vector2& rhs) const {
  Vector2 value;
  value.x = x - rhs.x;
  value.y = y - rhs.y;
  return value;
}

Vector2 Vector2::operator*(float rhs) const {
  Vector2 value;
  value.x = x * rhs;
  value.y = y * rhs;
  return value;
}

Vector2 Vector2::operator/(float rhs) const {
  Vector2 value;
  value.x = x / rhs;
  value.y = y / rhs;
  return value;
}

bool Vector2::operator==(const Vector2& rhs) const {
  return ((x == rhs.x) && (y == rhs.y));
}

bool Vector2::operator!=(const Vector2& rhs) const {
  return ((x != rhs.x) || (y != rhs.y));
}

bool Vector2::operator<(const Vector2& rhs) const {
  if (x != rhs.x) {
    return (x < rhs.x);
  }
  if (y != rhs.y) {
    return (y < rhs.y);
  }
  return false;
}

void Vector2::Add(const Vector2& rhs) {
  x += rhs.x;
  y += rhs.y;
}

void Vector2::Scale(float rhs) {
  x *= rhs;
  y *= rhs;
}

void Vector2::Set(float in_x, float in_y) {
  x = in_x;
  y = in_y;
}

void Vector2::ReadFrom(Stream* stream) {
  StreamReadFlat(stream, *this);
}

void Vector2::WriteTo(Stream* out_stream) {
  StreamWriteFlat(out_stream, *this);
}

Vector3::Vector3(float in_value) :
  x(in_value), y(in_value), z(in_value) {}

Vector3::Vector3(const Vector3& rhs) :
  x(rhs.x), y(rhs.y), z(rhs.z) {}

Vector3::Vector3(const float* xyz) :
  x(xyz[0]), y(xyz[1]), z(xyz[2]) {}

Vector3::Vector3(float in_x, float in_y, float in_z) :
  x(in_x), y(in_y), z(in_z) {}

Vector3& Vector3::operator-() {
  x = -x;
  y = -y;
  z = -z;
  return *this;
}

Vector3& Vector3::operator=(const Vector3& rhs) {
  if (this == &rhs) {
    return *this;
  }
  x = rhs.x;
  y = rhs.y;
  z = rhs.z;
  return *this;
}

Vector3& Vector3::operator*=(const Vector3& rhs) {
  x *= rhs.x;
  y *= rhs.y;
  z *= rhs.z;
  return *this;
}

Vector3& Vector3::operator/=(const Vector3& rhs) {
  x /= rhs.x;
  y /= rhs.y;
  z /= rhs.z;
  return *this;
}

Vector3& Vector3::operator+=(const Vector3& rhs) {
  x += rhs.x;
  y += rhs.y;
  z += rhs.z;
  return *this;
}

Vector3& Vector3::operator-=(const Vector3& rhs) {
  x -= rhs.x;
  y -= rhs.y;
  z -= rhs.z;
  return *this;
}

Vector3& Vector3::operator*=(float rhs) {
  x *= rhs;
  y *= rhs;
  z *= rhs;
  return *this;
}

Vector3& Vector3::operator/=(float rhs) {
  x /= rhs;
  y /= rhs;
  z /= rhs;
  return *this;
}

Vector3 Vector3::operator*(const Vector3& rhs) const {
  Vector3 value;
  value.x = x * rhs.x;
  value.y = y * rhs.y;
  value.z = z * rhs.z;
  return value;
}

Vector3 Vector3::operator/(const Vector3& rhs) const {
  Vector3 value;
  value.x = x / rhs.x;
  value.y = y / rhs.y;
  value.z = z / rhs.z;
  return value;
}

Vector3 Vector3::operator+(const Vector3& rhs) const {
  Vector3 value;
  value.x = x + rhs.x;
  value.y = y + rhs.y;
  value.z = z + rhs.z;
  return value;
}

Vector3 Vector3::operator-(const Vector3& rhs) const {
  Vector3 value;
  value.x = x - rhs.x;
  value.y = y - rhs.y;
  value.z = z - rhs.z;
  return value;
}

Vector3 Vector3::operator*(float rhs) const {
  Vector3 value;
  value.x = x * rhs;
  value.y = y * rhs;
  value.z = z * rhs;
  return value;
}

Vector3 Vector3::operator/(float rhs) const {
  Vector3 value;
  value.x = x / rhs;
  value.y = y / rhs;
  value.z = z / rhs;
  return value;
}

bool Vector3::operator==(const Vector3& rhs) const {
  return ((x == rhs.x) && (y == rhs.y) && (z == rhs.z));
}

bool Vector3::operator!=(const Vector3& rhs) const {
  return ((x != rhs.x) || (y != rhs.y) || (z != rhs.z));
}

bool Vector3::operator<(const Vector3& rhs) const {
  if (x != rhs.x) {
    return (x < rhs.x);
  }
  if (y != rhs.y) {
    return (y < rhs.y);
  }
  if (z != rhs.z) {
    return (z < rhs.z);
  }
  return false;
}

void Vector3::Add(const Vector3& rhs) {
  x += rhs.x;
  y += rhs.y;
  z += rhs.z;
}

void Vector3::Scale(float rhs) {
  x *= rhs;
  y *= rhs;
  z *= rhs;
}

void Vector3::Set(float in_x, float in_y, float in_z) {
  x = in_x;
  y = in_y;
  z = in_z;
}

float Vector3::Dot(const Vector3& rhs) const {
  return x * rhs.x + y * rhs.y + z * rhs.z;
}

Vector3 Vector3::Cross(const Vector3& rhs) const {
  Vector3 retValue;
  return GetCross(retValue, *this, rhs);
}

Vector3& Vector3::GetCross(Vector3& out_vector, const Vector3& lhs, const Vector3& rhs) {
  out_vector.x = lhs.y*rhs.z - lhs.z*rhs.y;
  out_vector.y = lhs.z*rhs.x - lhs.x*rhs.z;
  out_vector.z = lhs.x*rhs.y - lhs.y*rhs.x;
  return out_vector;
}

Vector3& Matrix::TransformCoord(Vector3& out_to, const Vector3& in_from) const {
  return out_to.StoreTransformCoord(in_from, *this);
}

Vector3& Matrix::TransformNormal(Vector3& out_to, const Vector3& in_from) const {
  return out_to.StoreTransformNormal(in_from, *this);
}

void Vector3::ReadFrom(Stream* stream) {
  StreamReadFlat(stream, *this);
}

void Vector3::WriteTo(Stream* out_stream) {
  StreamWriteFlat(out_stream, *this);
}

const Vector3& Vector3::NormalizeFrom(const Vector3& value) {
  *this = value;
  this->Normalize();
  return *this;
}

Vector3 Vector3::CopyNormalized() const {
  Vector3 normalized(*this);
  normalized.Normalize();
  return normalized;
}

Vector4::Vector4(float in_value) :
  x(in_value), y(in_value), z(in_value), w(in_value) {}

Vector4::Vector4(const Vector4& rhs) :
  x(rhs.x), y(rhs.y), z(rhs.z), w(rhs.w) {}

Vector4::Vector4(float in_x, float in_y, float in_z, float in_w) :
  x(in_x), y(in_y), z(in_z), w(in_w) {}

Vector4::Vector4(const float* xyzw) :
  x(xyzw[0]), y(xyzw[1]), z(xyzw[2]), w(xyzw[3]) {}

Vector4::Vector4(const Vector3& xyz) :
  x(xyz.x), y(xyz.y), z(xyz.z), w(1.0f) {}

Vector4::Vector4(const Vector3& in_xyz, float in_w) :
  x(in_xyz.x), y(in_xyz.y), z(in_xyz.z), w(in_w) {}

Vector4& Vector4::operator=(const Vector3& rhs) {
  return operator=(Vector4(rhs));
}

Vector4& Vector4::operator=(const Vector4& rhs) {
  if (this == &rhs) {
    return *this;
  }
  x = rhs.x;
  y = rhs.y;
  z = rhs.z;
  w = rhs.w;
  return *this;
}

Vector4 Vector4::operator*(float rhs) const {
  Vector4 value;
  value.x = x * rhs;
  value.y = y * rhs;
  value.z = z * rhs;
  value.w = w * rhs;
  return *this;
}

Vector4 Vector4::operator/(float rhs) const {
  Vector4 value;
  value.x = x / rhs;
  value.y = y / rhs;
  value.z = z / rhs;
  value.w = w / rhs;
  return *this;
}

Vector4& Vector4::operator*=(float rhs) {
  x *= rhs;
  y *= rhs;
  z *= rhs;
  w *= rhs;
  return *this;
}

Vector4& Vector4::operator/=(float rhs) {
  x /= rhs;
  y /= rhs;
  z /= rhs;
  w /= rhs;
  return *this;
}

Vector4& Vector4::operator+=(const Vector4& rhs) {
  x += rhs.x;
  y += rhs.y;
  z += rhs.z;
  w += rhs.w;
  return *this;
}

Vector4& Vector4::operator-=(const Vector4& rhs) {
  x -= rhs.x;
  y -= rhs.y;
  z -= rhs.z;
  w -= rhs.w;
  return *this;
}

void Vector4::Add(const Vector4& rhs) {
  x += rhs.x;
  y += rhs.y;
  z += rhs.z;
  w += rhs.w;
}

void Vector4::Set(float in_x, float in_y, float in_z, float in_w) {
  x = in_x;
  y = in_y;
  z = in_z;
  w = in_w;
}

void Vector4::Set(const Vector3& in_xyz, float in_w) {
  x = in_xyz.x;
  y = in_xyz.y;
  z = in_xyz.z;
  w = in_w;
}

void Vector4::Set(const ColorRGBA& color) {
  x = color.r;
  y = color.g;
  z = color.b;
  w = color.a;
}

void Vector4::ReadFrom(Stream* stream) {
  StreamReadFlat(stream, *this);
}

void Vector4::WriteTo(Stream* out_stream) {
  StreamWriteFlat(out_stream, *this);
}

Point2::Point2() :
  x(0), y(0) {}

Point2::Point2(const Point2& size) :
  x(size.x), y(size.y) {}

Point2::Point2(int in_x, int in_y) :
  x(in_x), y(in_y) {}

void Point2::Set(int in_x, int in_y) {
  x = in_x;
  y = in_y;
}

Point2& Point2::operator=(int value) {
  Set(value, value);
  return *this;
}

Point2& Point2::operator=(const Point2& rhs) {
  Set(rhs.x, rhs.y);
  return *this;
}

bool Point2::operator==(const Point2& rhs) {
  return (x == rhs.x) && (y == rhs.y);
}

bool Point2::operator!=(const Point2& rhs) {
  return (x != rhs.x) || (y != rhs.y);
}

Point2& Point2::operator+=(const Point2& rhs) {
  x += rhs.x;
  y += rhs.y;
  return *this;
}

Point2& Point2::operator-=(const Point2& rhs) {
  x -= rhs.x;
  y -= rhs.y;
  return *this;
}

Point2& Point2::operator*=(int rhs) {
  x *= rhs;
  y *= rhs;
  return *this;
}

Point2& Point2::operator/=(int rhs) {
  x /= rhs;
  y /= rhs;
  return *this;
}

Point2 Point2::operator+(const Point2& rhs) const {
  Point2 curSize(*this);
  curSize += rhs;
  return curSize;
}

Point2 Point2::operator-(const Point2& rhs) const {
  Point2 curSize(*this);
  curSize -= rhs;
  return curSize;
}

Point2 Point2::operator*(int rhs) const {
  Point2 curSize(*this);
  curSize *= rhs;
  return curSize;
}

Point2 Point2::operator/(int rhs) const {
  Point2 curSize(*this);
  curSize /= rhs;
  return curSize;
}

bool Point2::IsZero() const {
  return (x == 0) || (y == 0);
}

bool Point2::IsSmallerThan(const Point2& rhs) const {
  if ((x < rhs.x) && (y < rhs.y)) {
    return true;
  }
  return false;
}

void Point2::MakePowerOf2() {
  x = dg::MakePowerOf2(x);
  y = dg::MakePowerOf2(y);
}

void Point2::ReadFrom(Stream* stream) {
  StreamReadFlat(stream, *this);
}

void Point2::WriteTo(Stream* out_stream) {
  StreamWriteFlat(out_stream, *this);
}

Point3::Point3() :
  x(0), y(0), z(0) {}

Point3::Point3(const Point3& value) :
  x(value.x), y(value.y), z(value.z) {}

Point3::Point3(int in_x, int in_y, int in_z) :
  x(in_x), y(in_y), z(in_z) {}

void Point3::Set(int in_x, int in_y, int in_z) {
  x = in_x;
  y = in_y;
  z = in_z;
}

Point3& Point3::operator=(int value) {
  Set(value, value, value);
  return *this;
}

Point3& Point3::operator=(const Point3& rhs) {
  Set(rhs.x, rhs.y, rhs.z);
  return *this;
}

bool Point3::operator==(const Point3& rhs) {
  return (x == rhs.x) && (y == rhs.y) && (z == rhs.z);
}

bool Point3::operator!=(const Point3& rhs) {
  return (x != rhs.x) || (y != rhs.y) || (z != rhs.z);
}

Point3& Point3::operator+=(const Point3& rhs) {
  x += rhs.x;
  y += rhs.y;
  z += rhs.z;
  return *this;
}

Point3& Point3::operator-=(const Point3& rhs) {
  x -= rhs.x;
  y -= rhs.y;
  z -= rhs.z;
  return *this;
}

Point3& Point3::operator*=(int rhs) {
  x *= rhs;
  y *= rhs;
  z *= rhs;
  return *this;
}

Point3& Point3::operator/=(int rhs) {
  x /= rhs;
  y /= rhs;
  z /= rhs;
  return *this;
}

Point3 Point3::operator+(const Point3& rhs) const {
  Point3 curSize(*this);
  curSize += rhs;
  return curSize;
}

Point3 Point3::operator-(const Point3& rhs) const {
  Point3 curSize(*this);
  curSize -= rhs;
  return curSize;
}

Point3 Point3::operator*(int rhs) const {
  Point3 curSize(*this);
  curSize *= rhs;
  return curSize;
}

Point3 Point3::operator/(int rhs) const {
  Point3 curSize(*this);
  curSize /= rhs;
  return curSize;
}

bool Point3::IsEmpty() const {
  return (x == 0) || (y == 0) || (z == 0);
}

bool Point3::IsSmallerThan(const Point3& rhs) const {
  if ((x < rhs.x) && (y < rhs.y) && (z < rhs.z)) {
    return true;
  }
  return false;
}

void Point3::ReadFrom(Stream* stream) {
  StreamReadFlat(stream, *this);
}

void Point3::WriteTo(Stream* out_stream) {
  StreamWriteFlat(out_stream, *this);
}

Rect::Rect() : left(0),
  top(0), right(0), bottom(0) {}

Rect::Rect(int in_left, int in_top, int in_right, int in_bottom) : left(in_left),
  top(in_top), right(in_right), bottom(in_bottom) {}

Rect::Rect(int in_left, int in_top, const Size2& size) : left(in_left),
  top(in_top), right(in_left + size.x), bottom(in_top + size.y) {}

Rect::Rect(const Size2& size) : left(0),
  top(0), right(size.x), bottom(size.y) {}

Rect::Rect(const Point2& location, const Size2& size) {
  SetLocation(location);
  SetSize(size);
}

Rect::Rect(const Rect& rect) {
  if (this == &rect) {
    return;
  }
  left = rect.left;
  top = rect.top;
  right = rect.right;
  bottom = rect.bottom;
}

void Rect::Set(int in_left, int in_top, int in_right, int in_bottom) {
  left = in_left;
  top = in_top;
  right = in_right;
  bottom = in_bottom;
}

void Rect::Set(int in_left, int in_top, const Size2& size) {
  left = in_left;
  top = in_top;
  right = left + size.x;
  bottom = top + size.y;
}

void Rect::SetLocation(int in_left, int in_top) {
  int savedWidth = right - left;
  int savedHeight = bottom - top;
  left = in_left;
  top = in_top;
  SetSize(savedWidth, savedHeight);
}

void Rect::SetLocation(const Point2& location) {
  int savedWidth = right - left;
  int savedHeight = bottom - top;
  left = location.x;
  top = location.y;
  SetSize(savedWidth, savedHeight);
}

void Rect::SetSize(int width, int height) {
  right = left + width;
  bottom = top + height;
}

void Rect::SetSize(const Size2& size) {
  right = left + size.x;
  bottom = top + size.y;
}

Rect& Rect::Translate(const Point2& offset) {
  left += offset.x;
  top += offset.y;
  right += offset.x;
  bottom += offset.y;
  return *this;
}

int Rect::GetWidth() const {
  return (right - left);
}

int Rect::GetHeight() const {
  return (bottom - top);
}

void Rect::ReadFrom(Stream* stream) {
  StreamReadFlat(stream, *this);
}

void Rect::WriteTo(Stream* out_stream) {
  StreamWriteFlat(out_stream, *this);
}

void ImplicitEuler(
    float& out_x,
    float end_x,
    float delta_time,
    const float stiffness,
    const float epsilon) {
  const float kDeltaX = end_x - out_x;
  if (fabsf(kDeltaX) < epsilon) {
    out_x = end_x;
    return;
  }
  if (stiffness <= 0) {
    out_x = end_x;
    return;
  }
  const float kInvStiffness = 1.0f / stiffness;
  out_x += delta_time / (1 + kInvStiffness * delta_time) * kInvStiffness * kDeltaX;
}

void ExplicitEuler(
    float& out_x,
    float end_x,
    float delta_time,
    const float stiffness,
    const float epsilon) {
  const float kDeltaX = end_x - out_x;
  if (::fabsf(kDeltaX) < epsilon) {
    out_x = end_x;
    return;
  }
  if (stiffness <= 0) {
    out_x = end_x;
    return;
  }
  float kInvStiffness = 1.0f / stiffness;
  out_x += kInvStiffness * delta_time * kDeltaX;
}

float DegreeToRadian(float degree) {
  return degree * kDegreeToRadian;
}

float RadianToDegree(float radian) {
  return radian * kRadianToDegree;
}

bool IsPowerOf2(int value) {
  return !(value & (value - 1));
}

int MakePowerOf2(int value) {
  Check(value >= 0);
  int current = 1;
  while (value > current) {
    current <<= 1;
  }
  return current;
}

} // namespace dg
