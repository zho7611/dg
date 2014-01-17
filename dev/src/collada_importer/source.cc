// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "collada_importer_first.h"

using namespace dg;
using namespace dg::collada_importer;

void Parameter::ReadFromXml(Element& elem) {
  elem.GetAttribute(TXT("name"), name_);
  String typeStr;
  elem.GetAttribute(TXT("type"), typeStr);
  if (typeStr.IsEqualNoCase(TXT("float"))) {
    type_ = ParameterType_Float;
  } else if (typeStr.IsEqualNoCase(TXT("Name"))) {
    type_ = ParameterType_Name;
  } else if (typeStr.IsEqualNoCase(TXT("float4x4"))) {
    type_ = ParameterType_Float4x4;
  } else {
    DG_UNIMPLEMENTED();
  }
}

void Accessor::ReadFromXml(Element& elem) {
  Check(elem.HasTagName(TXT("accessor")));
  String sourceIdStr;
  elem.GetAttribute(TXT("source"), sourceIdStr);
  TrimReferenceMark(sourceIdStr);
  elem.GetAttribute(TXT("count"), count_);
  elem.GetAttribute(TXT("stride"), Stride_);
  DG_FOR_EACH_ELEMENT(elem, sub_elem) {
    if (sub_elem.HasTagName(TXT("param"))) {
      collada_importer::Parameter curParam;
      curParam.ReadFromXml(sub_elem);
      parameters_.PushBack(curParam);
    }
  }
}

void TechniqueCommon::ReadFromXml(Element& elem) {
  Check(elem.HasTagName(TXT("technique_common")));
  Element sub_elem;
  if (elem.GetSubElement(TXT("accessor"), sub_elem)) {
    Accessor_.ReadFromXml(sub_elem);
  }
}

Source::Source() :
  index_(0),
  NumValues_(0) {
}

void Source::ReadFromXml(Element& elem) {
  Element sub_elem;
  elem.GetAttribute(TXT("id"), id_);
  if (elem.GetSubElement(TXT("float_array"), sub_elem)) {
    LoadFloatArrayFromXml(sub_elem);
  }
  if (elem.GetSubElement(TXT("IDREF_array"), sub_elem)) {
    LoadStringArrayFromXml(sub_elem);
  }
  if (elem.GetSubElement(TXT("Name_array"), sub_elem)) {
    LoadStringArrayFromXml(sub_elem);
  }
  if (elem.GetSubElement(TXT("technique_common"), sub_elem)) {
    technique_common_.ReadFromXml(sub_elem);
  }
}

void Source::LoadFloatArrayFromXml(Element& elem) {
  String count_str;
  elem.GetAttribute(TXT("count"), count_str);
  int count = MyAtoi(count_str.Get());
  NumValues_ = count;
  String floatsStr;
  elem.GetValue(floatsStr);
  StringStream theStream(floatsStr.Get());
  float_values_.GrowTo(count);
  float value;
  while (count--) {
    if (theStream.ReadFloat(value)) {
      float_values_.PushBack(value);
    }
  }
}

void Source::LoadStringArrayFromXml(Element& elem) {
  String count_str;
  elem.GetAttribute(TXT("count"), count_str);
  int count = MyAtoi(count_str.Get());
  NumValues_ = count;
  String idRefStr;
  elem.GetValue(idRefStr);
  StringStream theStream(idRefStr.Get());
  string_values_.GrowTo(count);
  String value;
  while (count--) {
    if (theStream.ReadClassTo(value)) {
      string_values_.PushBack(value.Get());
    }
  }
}

void Source::GetFloat(int index, float& out_value) {
  out_value = float_values_.GetConst(index);
}

void Source::GetVector(int index, Vector3& out_value) {
  out_value.Set(
      float_values_.Get(index*3),
      float_values_.Get(index*3+1),
      float_values_.Get(index*3+2));
}

void Source::GetVector2(int index, Vector2& out_value) {
  // It's for Vector2, but the index increases with 3
  out_value.Set(
      float_values_.Get(index*3),
      float_values_.Get(index*3+1));
}

void Source::GetMatrix(int index, Matrix& out_value) {
  for (int idx = 0; idx < 4; ++idx) {
    for (int Idx2 = 0; Idx2 < 4; ++Idx2) {
      // Collada(OpenGL, column-major) to Matrix(D3D, row-major) conversion
      out_value.Set(
          Idx2,
          idx,
          float_values_.Get(index*16 + idx*4 + Idx2));
    }
  }
}

void Source::GetString(int index, String& out_value) {
  out_value = string_values_.GetConst(index);
}

void Source::SetFloat(int index, float value) {
  float_values_.Get(index) = value;
}

void Source::SetVector(int index, const Vector3& value) {
  float_values_.Get(index*3) = value.x;
  float_values_.Get(index*3+1) = value.y;
  float_values_.Get(index*3+2) = value.z;
}

void Source::SetVector2(int index, const Vector2& value) {
  // It's for Vector2, but the index increases with 3
  float_values_.Get(index*3) = value.x;
  float_values_.Get(index*3+1) = value.y;
}

void Source::SetMatrix(int index, const Matrix& value) {
  for (int idx = 0; idx < 4; ++idx) {
    for (int Idx2 = 0; Idx2 < 4; ++Idx2) {
      // Column-major(Matrix, D3d) to row-major(Collada, OpenGL)
      float_values_.Get(index*16 + idx*4 + Idx2) = value.Get(Idx2, idx);
    }
  }
}

void Source::SetString(int index, const String& value) {
  string_values_.Get(index) = value;
}

Source::FloatValues& Source::GetFloatValues() {
  return float_values_;
}

Source::StringValues& Source::GetStringValues() {
  return string_values_;
}
