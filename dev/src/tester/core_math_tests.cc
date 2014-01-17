// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "tester.h"

using namespace dg;

TEST(Math) {
  // Test Rect
  {
    int left = 10;
    int top = 20;
    int right = 30;
    int bottom = 50;
    dg::Rect rect(left, top, right, bottom);
    EXPECT_TRUE(rect.GetWidth() == (right - left));
    EXPECT_TRUE(rect.GetHeight() == (bottom - top));
    rect.SetLocation(Point2(left, top));
    EXPECT_TRUE(rect.GetLocation() == Point2(left, top));
    EXPECT_TRUE(rect.size() == Size2(right - left, bottom - top));
  }
  // Test Size2/Point2
  {
    Size2 one_by_one(1, 1);
    Size2 temp(2, 2);
    temp = one_by_one;
    EXPECT_TRUE(temp.x == 1);
    EXPECT_TRUE(temp.y == 1);
  }
  // Test PowerOfTwo
  {
    EXPECT_TRUE(MakePowerOf2(0) == 1);
    EXPECT_TRUE(MakePowerOf2(1) == 1);
    EXPECT_TRUE(MakePowerOf2(2) == 2);
    EXPECT_TRUE(MakePowerOf2(3) == 4);
    EXPECT_TRUE(MakePowerOf2(5) == 8);
    EXPECT_TRUE(MakePowerOf2(126) == 128);
  }
  // Test Min/Max
  {
    EXPECT_TRUE(Min(0, 1) == 0);
    EXPECT_TRUE(Min(1, -1) == -1);
    EXPECT_TRUE(Min(1, 1) == 1);
    EXPECT_TRUE(Max(0, 1) == 1);
    EXPECT_TRUE(Max(1, -1) == 1);
    EXPECT_TRUE(Max(1, 1) == 1);
    EXPECT_TRUE(Min(0, 1, 2) == 0);
    EXPECT_TRUE(Min(1, -1, -2) == -2);
    EXPECT_TRUE(Min(1, 1, 1) == 1);
    EXPECT_TRUE(Max(0, 1, 2) == 2);
    EXPECT_TRUE(Max(1, -1, -2) == 1);
    EXPECT_TRUE(Max(1, 1, 1) == 1);
  }
}

static bool IsMatrixSimilar(Matrix& lhs, Matrix rhs, float epsilon = 0.0001f) {
  for (int idx = 0; idx < 16; ++idx) {
    if (fabs(lhs.items_16_[idx] - rhs.items_16_[idx]) > epsilon) {
      return false;
    }
  }
  return true;
}

static bool IsFloatSimilar(float lhs, float rhs, float epsilon = 0.0001f) {
  if (fabs(lhs - rhs) > epsilon) {
    return false;
  }
  return true;
}

static bool IsVectorSimilar(Vector3& lhs, Vector3& rhs, float epsilon = 0.0001f) {
  if (fabs(lhs.x - rhs.x) > epsilon) {
    return false;
  }
  if (fabs(lhs.y - rhs.y) > epsilon) {
    return false;
  }
  if (fabs(lhs.z - rhs.z) > epsilon) {
    return false;
  }
  return true;
}

static bool IsQuaternionSimilar(Quaternion& lhs, Quaternion& rhs, float epsilon = 0.0001f) {
  if (fabs(lhs.x - rhs.x) > epsilon) {
    return false;
  }
  if (fabs(lhs.y - rhs.y) > epsilon) {
    return false;
  }
  if (fabs(lhs.z - rhs.z) > epsilon) {
    return false;
  }
  if (fabs(lhs.w - rhs.w) > epsilon) {
    return false;
  }
  return true;
}

TEST(Vector3) {
  Vector3 v;
  // Default values
  EXPECT_TRUE(v.x == 0);
  EXPECT_TRUE(v.y == 0);
  EXPECT_TRUE(v.z == 0);
  // Set method
  v.Set(1.0f, 2.0f, 3.0f);
  EXPECT_TRUE(v.x == 1.0f);
  EXPECT_TRUE(v.y == 2.0f);
  EXPECT_TRUE(v.z == 3.0f);
  // Normalize
  v.Normalize();
  IsVectorSimilar(v, Vector3(0.26726124f, 0.53452247f, 0.80178368f));
  // Normalize for zero vector
  v.Set(0, 0, 0);
  v.Normalize();
  EXPECT_TRUE(v.x == 0);
  EXPECT_TRUE(v.y == 0);
  EXPECT_TRUE(v.z == 0);
}

TEST(Quaternion) {
  Quaternion q;
  q.StoreRotationYawPitchRoll(1.0f, 2.0f, 3.0f);
  Quaternion correct(0.31062245f, -0.71828699f, 0.44443509f, 0.43595284f);
  EXPECT_TRUE(IsQuaternionSimilar(q, correct));
  // To AxisAngle
  {
    Vector3 axis(1.0f, 0, 0);
    float angle(DegreeToRadian(45.0f));
    q.StoreRotationAxisAngle(axis, angle);
    Vector3 axit_test;
    float angle_test;
    q.ToAxisAngle(axit_test, angle_test);
    axit_test.Normalize();
    EXPECT_TRUE(IsVectorSimilar(axit_test, axis));
    EXPECT_TRUE(IsFloatSimilar(angle_test, angle));
  }
}

TEST(Matrix) {
  Matrix m;
  m.MakeIdentity();
  m.Set(0, 3.f);
  m.Set(5, 4.f);
  m.StoreScale(2.f, 2.f, 2.f);
  EXPECT_TRUE(m.Get(0) == 2.f);
  EXPECT_TRUE(m.Get(5) == 2.f);
  // StoreRotationAxis
  {
    m.MakeIdentity();
    m.StoreRotationAxis(Vector3(10.0f, 20.0f, 30.0f), DegreeToRadian(45.0f));
    Matrix correct(
        0.728028f, 0.608789f, -0.315202f, 0.000000f,
        -0.525105f, 0.790791f, 0.314508f, 0.000000f,
        0.440727f, -0.063457f, 0.895395f, 0.000000f,
        0.000000f, 0.000000f, 0.000000f, 1.000000f);
    EXPECT_TRUE(IsMatrixSimilar(m, correct));
  }
  // StoreMultiply
  {
    Matrix m1, m2;
    m1.StoreRotationAxis(Vector3(10.0f, 20.0f, 30.0f), DegreeToRadian(45.0f));
    m2.StoreRotationAxis(Vector3(30.0f, 20.0f, 10.0f), DegreeToRadian(60.0f));
    Matrix m1m2;
    m1m2.StoreMultiply(m1, m2);
    Matrix correct(
        0.407889f, 0.912226f, 0.038340f, 0.000000f,
        -0.265627f, 0.078386f, 0.960884f, 0.000000f,
        0.873538f, -0.402118f, 0.274285f, 0.000000f,
        0.000000f, 0.000000f, 0.000000f, 1.000000f);
    EXPECT_TRUE(IsMatrixSimilar(m1m2, correct));
    m1m2 = m1 * m2;
    EXPECT_TRUE(IsMatrixSimilar(m1m2, correct));
  }
  // StoreRotationQuaternion
  {
    m.MakeIdentity();
    Quaternion q;
    q.StoreRotationYawPitchRoll(1.0f, 2.0f, 3.0f);
    m.StoreRotationQuaternion(q);
    Matrix correct(
        -0.426917f, -0.058727f, 0.902382f, 0.000000f,
        -0.833738f, 0.411982f, -0.367630f, 0.000000f,
        -0.350175f, -0.909297f, -0.224845f, 0.000000f,
        0.000000f, 0.000000f, 0.000000f, 1.000000f);
    EXPECT_TRUE(IsMatrixSimilar(m, correct));
  }
  // StoreRotationMatrix
  {
    Matrix src(
        -0.426917f, -0.058727f, 0.902382f, 0.000000f,
        -0.833738f, 0.411982f, -0.367630f, 0.000000f,
        -0.350175f, -0.909297f, -0.224845f, 0.000000f,
        0.000000f, 0.000000f, 0.000000f, 1.000000f);
    Quaternion q;
    q.StoreRotationMatrix(src);
    EXPECT_TRUE(IsFloatSimilar(q.x, -0.31062278f));
    EXPECT_TRUE(IsFloatSimilar(q.y, 0.71828687f));
    EXPECT_TRUE(IsFloatSimilar(q.z, -0.44443488f));
    EXPECT_TRUE(IsFloatSimilar(q.w, -0.43595293f));
  }
  // AffineTransformation
  {
    float scale = 2.0f;
    Vector3 rotation_center(1.0f, 2.0f, 3.0f);
    Quaternion rotation;
    rotation.StoreRotationYawPitchRoll(1.0f, 2.0f, 3.0f);
    Vector3 translation(100.0f, 200.0f, 300.0f);
    m.AffineTransformation(scale, &rotation_center, &rotation, &translation);
    Matrix correct(
        -0.85383499f, -0.11745334f, 1.8047631f, 0.0f,
        -1.6674752f, 0.82396460f, -0.73526084f, 0.0f,
        -0.70035094f, -1.8185948f, -0.44968998f, 0.0f,
        104.14492f, 203.96265f, 303.50742f, 1.0f);
    EXPECT_TRUE(IsMatrixSimilar(m, correct));
  }
  // Ortho Matrix
  {
    Matrix correct;
    // OrthoLH Test
    {
      m.OrthoLH(100.0f, 200.0f, 1.0f, 100.0f);
      correct.Set16(
          0.020000f, 0.000000f, 0.000000f, 0.000000f,
          0.000000f, 0.010000f, 0.000000f, 0.000000f,
          0.000000f, 0.000000f, 0.010101f, 0.000000f,
          0.000000f, 0.000000f, -0.010101f, 1.000000f);
      EXPECT_TRUE(IsMatrixSimilar(m, correct));
    }
    // OrthoRH Test
    {
      m.OrthoRH(100.0f, 200.0f, 1.0f, 100.0f);
      correct.Set16(
          0.020000f, 0.000000f, 0.000000f, 0.000000f,
          0.000000f, 0.010000f, 0.000000f, 0.000000f,
          0.000000f, 0.000000f, -0.010101f, 0.000000f,
          0.000000f, 0.000000f, -0.010101f, 1.000000f);
      EXPECT_TRUE(IsMatrixSimilar(m, correct));
    }
    // OrthoOffCenterLH Test
    {
      m.OrthoOffCenterLH(-10, 20, -30, 40, .1f, 1000.f);
      correct.Set16(
          0.066667f, 0.000000f, 0.000000f, 0.000000f,
          0.000000f, 0.028571f, 0.000000f, 0.000000f,
          0.000000f, 0.000000f, 0.001000f, 0.000000f,
          -0.333333f, -0.142857f, -0.000100f, 1.000000f);
      //LogMatrix(m);
      EXPECT_TRUE(IsMatrixSimilar(m, correct));
    }
    // OrthoOffCenterRH Test
    {
      m.OrthoOffCenterRH(-10, 20, -30, 40, .1f, 1000.f);
      correct.Set16(
          0.066667f, 0.000000f, 0.000000f, 0.000000f,
          0.000000f, 0.028571f, 0.000000f, 0.000000f,
          0.000000f, 0.000000f, -0.001000f, 0.000000f,
          -0.333333f, -0.142857f, -0.000100f, 1.000000f);
      LogMatrix(m);
      EXPECT_TRUE(IsMatrixSimilar(m, correct));
    }
  }
  // Decompose
  {
    m.Set16(
        0.086512f, -0.988905f, -0.120762f, 0.000000f,
        0.996075f, 0.088137f, -0.008176f, 0.000000f,
        0.018729f, -0.119580f, 0.992648f, 0.000000f,
        974.318970f, -13383.000000f, 7188.310059f, 1.000000f);
    Vector3 scale, translation;
    Quaternion rotation;
    bool ret = m.Decompose(scale, rotation, translation);
    EXPECT_TRUE(ret);
    Vector3 correctScale(1.0f, 1.0f, 1.0f);
    Quaternion correctRotation(0.037836548f, 0.047375824f, -0.67416590f, 0.73608708f);
    Vector3 correctTranslation(974.31897f, -13383.0f, 7188.3101f);
    EXPECT_TRUE(IsVectorSimilar(scale, correctScale));
    EXPECT_TRUE(IsVectorSimilar(translation, correctTranslation));
    EXPECT_TRUE(IsQuaternionSimilar(rotation, correctRotation));
  }
}
