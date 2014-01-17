// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {
namespace collada_importer {

class Parameter : public NodeBase {
public:
  enum ParameterType {
      ParameterType_Float,
      ParameterType_Float4x4,
      ParameterType_Name,
  };

  String name_;
  ParameterType type_;

  virtual void ReadFromXml(Element& elem);
};

class Accessor : public NodeBase {
public:
  Array<Parameter> parameters_;
  int count_;
  int Stride_;

  virtual void ReadFromXml(Element& elem);
};

class TechniqueCommon : public NodeBase {
public:
  Accessor Accessor_;

  virtual void ReadFromXml(Element& elem);
};

class Source : public NodeBase {
public:
  typedef Array<float> FloatValues;
  typedef Array<String> StringValues;

  enum Format {
      Format_Position = 0,
      Format_Normal = 1,
      Format_TexCoord = 2,
      Format_TexTangent = 3,
      Format_TexBinormal = 4,
      Format_Joint = 5,
      Format_Weight = 6,
      Format_InvBindMatrix = 7,
      Format_IdRef = 8,
      Format_Num = 9,
  };

  int index_;
  int NumValues_;

  Source();
  void GetFloat(int index, float& out_value);
  void GetVector(int index, Vector3& out_value);
  void GetVector2(int index, Vector2& out_value);
  void GetMatrix(int index, Matrix& out_value);
  void GetString(int index, String& out_value);
  void SetFloat(int index, float value);
  void SetVector(int index, const Vector3& value);
  void SetVector2(int index, const Vector2& value);
  void SetMatrix(int index, const Matrix& value);
  void SetString(int index, const String& value);
  FloatValues& GetFloatValues();
  StringValues& GetStringValues();
  virtual void ReadFromXml(Element& elem);

private:
  FloatValues float_values_;
  StringValues string_values_;
  TechniqueCommon technique_common_;

  void LoadFloatArrayFromXml(Element& elem);
  void LoadStringArrayFromXml(Element& elem);
};

} // End of namespace collada_importer
} // End of namespace dg
