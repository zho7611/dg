// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

template<class T>
const T& Max(const T& value0, const T& value1) {
  return (value0 > value1) ? value0 : value1;
}

template<class T>
const T& Min(const T& value0, const T& value1) {
  return (value0 < value1) ? value0 : value1;
}

template<class T>
const T& Min(const T& value0, const T& value1, const T& value2) {
  return (value0 < value1) ? ((value0 < value2) ? value0 : value2) : ((value1 < value2) ? value1 : value2);
}

template<class T>
const T& Max(const T& value0, const T& value1, const T& value2) {
  return (value0 > value1) ? ((value0 > value2) ? value0 : value2) : ((value1 > value2) ? value1 : value2);
}

// #warning: for d3d, it is equal to D3DCOLOR(0xARGB). but, in OpenGL, it is 0xABGR
typedef unsigned long ColorInt;

#define DG_CONVERT_ARGTB_TO_ABGR(argb) ((argb & 0xFF00FF00) | ((argb & 0xFF) << 16) | ((argb >> 16) & 0xFF))

#if defined(DG_RENDERER_DX9)
#define DG_USE_COLOR_INT_AS_ARGB // Interprete color as 0xARGB for DX9
// Set ColorInt from 4 components. each component value is in range [0,255]
#define DG_MAKE_COLOR_INT(alpha, red, green, blue) (((alpha)<<24)|((red)<<16)|((green)<<8)|(blue))
// Set ColorInt from ARGB hex code. DG_MAKE_COLOR_ARGB(0xffff0000) <= Red
#define DG_MAKE_COLOR_ARGB(argb) (argb)
#else // For OpenGL
#define DG_USE_COLOR_INT_AS_ABGR
// Make 0xABGR color for OpenGL
#define DG_MAKE_COLOR_INT(alpha, red, green, blue) (((alpha)<<24)|((blue)<<16)|((green)<<8)|(red))
#define DG_MAKE_COLOR_ARGB(argb) (DG_CONVERT_ARGTB_TO_ABGR(argb))
#endif

class ColorRGBA {
public:
  float r;
  float g;
  float b;
  float a;

  ColorRGBA() : r(0), g(0), b(0), a(0) {}
  ColorRGBA(float red, float green, float blue, float alpha) :
    r(red), g(green), b(blue), a(alpha) {}
  void Set(float red, float green, float blue, float alpha) {
    r = red; g = green; b = blue; a = alpha;
  }
  void FromColorInt(ColorInt color);
  ColorInt ToColorInt() const;
  void ReadFrom(Stream* stream);
  void WriteTo(Stream* out_stream);
  bool operator==(const ColorRGBA& rhs) {
    return ((r == rhs.r) && (g == rhs.g) && (b == rhs.b) && (a == rhs.a));
  }
};

class Vector3;
class Matrix;

class Quaternion {
public:
  float x;
  float y;
  float z;
  float w;

  static Quaternion kIdentity;

  Quaternion::Quaternion() : x(0), y(0), z(0), w(1) {}
  Quaternion(float in_x, float in_y, float in_z, float in_w);
  void StoreRotationMatrix(const Matrix& rotation_matrix);
  // Yaw - yaw around y-axis in radians
  // Pitch - pitch around x-axis in radians
  // Roll - roll around z-axis in radians
  void StoreRotationYawPitchRoll(float yaw, float pitch, float roll);
  void StoreMultiply(const Quaternion& lhs, const Quaternion& rhs);
  void StoreRotationAxisAngle(const Vector3& axis, float angle);
  void ToAxisAngle(Vector3& out_axis, float& out_angle);
  void ReadFrom(Stream* stream);
  void WriteTo(Stream* stream);
  void CopyFrom(const Quaternion& rhs);
  float* GetFloats() { return &x; }
  const float* GetFloatsConst() const { return &x; }
};

class Vector2 {
public:
  float x;
  float y;

  static Vector2 kZero;
  static Vector2 kOne;

  Vector2() : x(0), y(0) {}
  Vector2(const Vector2& rhs);
  Vector2(const float* in_xy);
  Vector2(float in_x, float in_y);
  Vector2& operator-();
  Vector2& operator=(const Vector2& rhs);
  Vector2& operator*=(const Vector2& rhs);
  Vector2& operator+=(const Vector2& rhs);
  Vector2& operator-=(const Vector2& rhs);
  Vector2& operator/=(const Vector2& rhs);
  Vector2& operator*=(float rhs);
  Vector2& operator/=(float rhs);
  Vector2 operator*(const Vector2& rhs) const;
  Vector2 operator+(const Vector2& rhs) const;
  Vector2 operator-(const Vector2& rhs) const;
  Vector2 operator/(const Vector2& rhs) const;
  Vector2 operator*(float rhs) const;
  Vector2 operator/(float rhs) const;
  bool operator==(const Vector2& rhs) const;
  bool operator!=(const Vector2& rhs) const;
  bool operator<(const Vector2& rhs) const;
  void Add(const Vector2& rhs);
  void Scale(float rhs);
  float GetLength() const { return sqrtf(x*x + y*y); }
  float GetLengthSqared() const { return x*x + y*y; }
  void Set(float in_x, float in_y);
  float* GetFloats() { return &x; }
  const float* GetFloatsConst() const { return &x; }
  void ReadFrom(Stream* stream);
  void WriteTo(Stream* out_stream);
};

class Vector3 {
public:
  float x;
  float y;
  float z;

  static Vector3 kZero;
  static Vector3 kOne;

  Vector3() : x(0), y(0), z(0) {}
  Vector3(float in_value);
  Vector3(const Vector3& rhs);
  Vector3(const float* xyz);
  Vector3(float in_x, float in_y, float in_z);
  Vector3& operator-();
  Vector3& operator=(const Vector3& rhs);
  Vector3& operator*=(const Vector3& rhs);
  Vector3& operator/=(const Vector3& rhs);
  Vector3& operator+=(const Vector3& rhs);
  Vector3& operator-=(const Vector3& rhs);
  Vector3& operator*=(float rhs);
  Vector3& operator/=(float rhs);
  Vector3 operator*(const Vector3& rhs) const;
  Vector3 operator/(const Vector3& rhs) const;
  Vector3 operator+(const Vector3& rhs) const;
  Vector3 operator-(const Vector3& rhs) const;
  Vector3 operator*(float rhs) const;
  Vector3 operator/(float rhs) const;
  bool operator==(const Vector3& rhs) const;
  bool operator!=(const Vector3& rhs) const;
  bool operator<(const Vector3& rhs) const;
  bool IsZero() const {
    return ((x == 0) && (y == 0) && (z == 0));
  }
  float Dot(const Vector3& rhs) const;
  Vector3 Cross(const Vector3& rhs) const;
  static Vector3& GetCross(Vector3& out_vector, const Vector3& lhs, const Vector3& rhs);
  Vector3& StoreTransformCoord(const Vector3& point, const Matrix& matrix);
  Vector3& StoreTransformNormal(const Vector3& normal, const Matrix& matrix);
  static Vector3 TransformCoord(const Vector3& point, const Matrix& matrix) {
    Vector3 tmp;
    tmp.StoreTransformCoord(point, matrix);
    return tmp;
  }
  static Vector3 TransformNormal(const Vector3& normal, const Matrix& matrix) {
    Vector3 tmp;
    tmp.StoreTransformNormal(normal, matrix);
    return tmp;
  }
  void Add(const Vector3& rhs);
  void Scale(float rhs);
  float GetLength() const {
    return sqrtf(x*x + y*y + z*z);
  }
  float GetLengthSqared() const {
    return x*x + y*y + z*z;
  }
  void Set(float in_x, float in_y, float in_z);
  float* GetFloats() { return &x; }
  const float* GetFloatsConst() const { return &x; }
  const Vector3& Normalize();
  const Vector3& NormalizeFrom(const Vector3& value);
  Vector3 CopyNormalized() const;
  void ReadFrom(Stream* stream);
  void WriteTo(Stream* out_stream);
};

inline float Dot(const Vector3& lhs, const Vector3& rhs) {
  return lhs.Dot(rhs);
}

inline Vector3 Cross(const Vector3& lhs, const Vector3& rhs) {
  return lhs.Cross(rhs);
}

class Vector4 {
public:
  float x;
  float y;
  float z;
  float w;

  static Vector4 kZero;
  static Vector4 kOne;

  Vector4() : x(0), y(0), z(0), w(0) {}
  Vector4(float in_value);
  Vector4(const Vector4& rhs);
  Vector4(float in_x, float in_y, float in_z, float in_w);
  Vector4(const float* xyzw);
  Vector4(const Vector3& xyz);
  Vector4(const Vector3& xyz, float in_w);
  Vector4& operator=(const Vector3& rhs);
  Vector4& operator=(const Vector4& rhs);
  Vector4 operator*(float rhs) const;
  Vector4 operator/(float rhs) const;
  Vector4& operator+=(const Vector4& rhs);
  Vector4& operator-=(const Vector4& rhs);
  Vector4& operator*=(float rhs);
  Vector4& operator/=(float rhs);
  void Add(const Vector4& rhs);
  void Set(float in_x, float in_y, float in_z, float in_w);
  void Set(const Vector3& xyz, float in_w);
  void Set(const ColorRGBA& color);
  float* GetFloats() { return &x; }
  const float* GetFloatsConst() const { return &x; }
  void ReadFrom(Stream* stream);
  void WriteTo(Stream* out_stream);
};

typedef Vector4 Position4;

class Point2 {
public:
  int x;
  int y;

  static Point2 kZero;

  Point2();
  Point2(const Point2& size);
  Point2(int in_x, int in_y);
  void Set(int in_x, int in_y);
  Point2& operator=(int value);
  Point2& operator=(const Point2& rhs);
  bool operator==(const Point2& rhs);
  bool operator!=(const Point2& rhs);
  Point2& operator+=(const Point2& rhs);
  Point2& operator-=(const Point2& rhs);
  Point2& operator*=(int rhs);
  Point2& operator/=(int rhs);
  Point2 operator+(const Point2& rhs) const;
  Point2 operator-(const Point2& rhs) const;
  Point2 operator*(int rhs) const;
  Point2 operator/(int rhs) const;
  bool IsSmallerThan(const Point2& rhs) const;
  void MakePowerOf2();
  bool IsZero() const;
  void ReadFrom(Stream* stream);
  void WriteTo(Stream* out_stream);
  int* GetInts() { return &x; }
  const int* GetIntsConst() const { return &x; }
};

typedef Point2 Size2;

class Point3 {
public:
  int x;
  int y;
  int z;

  static Point3 kZero;

  Point3();
  Point3(const Point3& value);
  Point3(int in_x, int in_y, int in_z);
  void Set(int in_x, int in_y, int in_z);
  Point3& operator=(int value);
  Point3& operator=(const Point3& rhs);
  bool operator==(const Point3& rhs);
  bool operator!=(const Point3& rhs);
  Point3& operator+=(const Point3& rhs);
  Point3& operator-=(const Point3& rhs);
  Point3& operator*=(int rhs);
  Point3& operator/=(int rhs);
  Point3 operator+(const Point3& rhs) const;
  Point3 operator-(const Point3& rhs) const;
  Point3 operator*(int rhs) const;
  Point3 operator/(int rhs) const;
  bool IsSmallerThan(const Point3& rhs) const;
  bool IsEmpty() const;
  void ReadFrom(Stream* stream);
  void WriteTo(Stream* out_stream);
  int* GetInts() { return &x; }
  const int* GetIntsConst() const { return &x; }
};

typedef Point3 Size3;

// Row-major 4x4 matrix
class Matrix {
public:
  static Matrix kIdentity;

  Matrix(bool is_make_identity = true);
  Matrix(const float* rhs);
  Matrix(const Matrix& rhs);
  Matrix(
      float m11, float m12, float m13, float m14,
      float m21, float m22, float m23, float m24,
      float m31, float m32, float m33, float m34,
      float m41, float m42, float m43, float m44);
  Matrix& StoreTranspose(const Matrix& lhs);
  Matrix& StoreMultiply(const Matrix& lhs, const Matrix& rhs);
  Matrix& StoreInverse(const Matrix& rhs);
  Matrix& StoreRotationNormal(const Vector3& normal, float angle_in_radian);
  Matrix& StoreRotationAxis(const Vector3& axis, float angle_in_radian);
  Matrix& StoreScale(float scale_x, float scale_y, float scale_z);
  Matrix& StoreTranslation(float in_x, float in_y, float in_z);
  Matrix& StoreRotationQuaternion(const Quaternion& rotation);
  void Invert() {
    StoreInverse(*this);
  }
  Matrix GetInverse() const {
    Matrix tmp(*this);
    tmp.Invert();
    return tmp;
  }
  float* GetFloats() { return static_cast<float*>(items_16_); }
  const float* GetFloatsConst() const { return static_cast<const float*>(items_16_); }
  float Get(int idx) const;
  float Get(int row, int col) const;
  void Set(int idx, float value);
  void Set(int row, int col, float value);
  void Set16(
      float m11, float m12, float m13, float m14,
      float m21, float m22, float m23, float m24,
      float m31, float m32, float m33, float m34,
      float m41, float m42, float m43, float m44);
  void Translate(float in_x, float in_y, float in_z);
  void MakeIdentity();
  Vector3 GetPosition() const {
    Vector3 pos;
    GetPosition(pos);
    return pos;
  }
  void GetPosition(Vector3& out_position) const;
  void set_position(const Vector3& position);
  Vector3& TransformCoord(Vector3& out_to, const Vector3& in_from) const;
  Vector3& TransformNormal(Vector3& out_to, const Vector3& in_from) const;
  void OrthoLH(float width, float height, float near_plane, float far_plane);
  void OrthoRH(float width, float height, float near_plane, float far_plane);
  void OrthoOffCenterLH(float in_left, float in_right, float in_bottom, float in_top, float near_plane, float far_plane);
  void OrthoOffCenterRH(float in_left, float in_right, float in_bottom, float in_top, float near_plane, float far_plane);
  void PerspectiveFovLH(float fov_y, float aspect_ratio, float near_plane, float far_plane);
  void PerspectiveFovRH(float fov_y, float aspect_ratio, float near_plane, float far_plane);
  void AffineTransformation(
      float scale, const Vector3* rotation_center,
      const Quaternion* rotation, const Vector3* translation);
  void LookAtLH(const Vector3* eye, const Vector3* at, const Vector3* up);
  void LookAtRH(const Vector3* eye, const Vector3* at, const Vector3* up);
  void ReadFrom(Stream* stream);
  void WriteTo(Stream* out_stream);
  void CopyFrom(const Matrix& rhs);
  bool Decompose(Vector3& out_scale, Quaternion& out_rotation, Vector3& out_translation);
  void GetColumnAxis(int column_index, Vector3& out_axis) const;
  Matrix operator*(const Matrix& rhs) const;

#if defined(DG_RENDERER_DX9)
public:
  const D3DXMATRIX* GetMatrixDx9() const { return (D3DXMATRIX*)(this); }
  D3DXMATRIX* GetMatrixDx9() { return (D3DXMATRIX*)(this); }

#endif
  union {
    struct {
      float m11_, m12_, m13_, m14_;
      float m21_, m22_, m23_, m24_;
      float m31_, m32_, m33_, m34_;
      float m41_, m42_, m43_, m44_;
    };
    float items_44_[4][4];
    float items_16_[16];
  };
};

class Rect {
public:
  int left;
  int top;
  int right;
  int bottom;

  Rect();
  Rect(int in_left, int in_top, int in_right, int in_bottom);
  Rect(int in_left, int in_top, const Size2& size);
  Rect(const Point2& location, const Size2& size);
  Rect(const Size2& size);
  Rect(const Rect& rect);
  Rect& Translate(const Point2& offset);
  void Set(int in_left, int in_top, int in_right, int in_bottom);
  void Set(int in_left, int in_top, const Size2& size);
  // Set the location and change the size as well according to the previous size
  void SetLocation(int in_left, int in_top);
  // Set the location and change the size as well according to the previous size
  void SetLocation(const Point2& location);
  // Set the size only
  void SetSize(int width, int height);
  // Set the size only
  void SetSize(const Size2& size);
  int GetWidth() const;
  int GetHeight() const;
  Point2 GetLocation() const {
    return Point2(left, top);
  }
  Size2 size() const {
    return Size2(GetWidth(), GetHeight());
  }
  void ReadFrom(Stream* stream);
  void WriteTo(Stream* out_stream);
};

void ImplicitEuler(float& out_x, float end_x, float delta_time, const float stiffness, const float epsilon);
void ExplicitEuler(float& out_x, float end_x, float delta_time, const float stiffness, const float epsilon);
float DegreeToRadian(float degree);
float RadianToDegree(float radian);
bool IsPowerOf2(int value);
int MakePowerOf2(int value);

const float kPi = 3.14159265358979323846264338327950288419716939937510582f;
const float kTwoPi = 2.f * kPi;
const float kEpsilon = 0.0000001f;

const float kDegreeToRadian = (kPi / 180.f);
const float kRadianToDegree = (180.f / kPi);

} // namespace dg
