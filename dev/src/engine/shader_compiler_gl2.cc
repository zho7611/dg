// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

static void SkipToLineEnd(const Cstr* source, int& out_index) {
  bool isLineEnd = false;
  while (source[out_index] != TXT('\0')) {
    if (source[out_index] == string_util::kCarriageReturnChar) {
      isLineEnd = true;
      out_index++;
    }
    if (source[out_index] == string_util::kLineFeedChar) {
      isLineEnd = true;
      out_index++;
    }
    if (isLineEnd) {
      return;
    }
    out_index++;
  }
}

static void SkipToEndBlockComment(const Cstr* source, int& out_index) {
  while (source[out_index] != TXT('\0')) {
    if (source[out_index] == TXT('*')) {
      out_index++;
      if (source[out_index] == TXT('/')) {
        out_index++;
        return;
      }
    }
    out_index++;
  }
}

static void SkipWhiteSpaces(const Cstr* source, int& out_index) {
  while (source[out_index] != TXT('\0')) {
    if ((source[out_index] == string_util::kTabChar) ||
        (source[out_index] == string_util::kSpaceChar)) {
      out_index++;
    } else {
      return;
    }
  }
}

// Returns true only if it successfully parse a word
// That is composed of alphanumeric characters
static bool ReadWord(const Cstr* source, int& out_index, String& outWord) {
  SkipWhiteSpaces(source, out_index);
  bool isFound = false;
  while (source[out_index] != TXT('\0')) {
    Cstr ch = source[out_index];
    if (string_util::IsAlphaNumeric(ch)) {
      isFound = true;
      outWord.AddCharRight(ch);
    } else {
      // If it starts with non-alphanumeric, then just skip the first character in this time
      if (!isFound) {
        out_index++;
      }
      break;
    }
    out_index++;
  }
  SkipWhiteSpaces(source, out_index);
  return isFound;
}

static int GetShaderParameterRegisterCountByType(ShaderParameter::ValueType value_type, int arrayCount) {
  switch (value_type) {
  //case ShaderParameter::kFloatType:
  //  return (arrayCount + 3) / 4;
  //case ShaderParameter::kVector2Type:
  //  return arrayCount / 2;
  //case ShaderParameter::kVector3Type:
  //  return (arrayCount * 3 + 3) / 4;
  case ShaderParameter::kVector4Type:
    return arrayCount;
  case ShaderParameter::kMatrixType:
    return arrayCount * 4;
  case ShaderParameter::kTextureType:
    return 1;
  default:
    DG_UNIMPLEMENTED();
  }
  // The default value is 1
  return 1;
}

static ShaderParameter::ValueType GetShaderParameterValueTypeByName(const String& typeName) {
  if (string_util::IsEqual(typeName.GetConst(), TXT("vec4"))) {
    return ShaderParameter::kVector4Type;
  } else if (string_util::IsEqual(typeName.GetConst(), TXT("mat4"))) {
    return ShaderParameter::kMatrixType;
  } else if (string_util::IsEqual(typeName.GetConst(), TXT("sampler2D"))) {
    return ShaderParameter::kTextureType;
  }
  return ShaderParameter::kUnknownType;
}

static void FillParameters(const Cstr* source, int source_size, ShaderType shader_type, Array<ShaderParameter>& out_parameters) {
  int idx = 0;
  while (source[idx] != TXT('\0')) {
    // Skip line comment
    if ((source[idx] == TXT('/')) && (source[idx+1] == TXT('/'))) {
      SkipToLineEnd(source, idx); // Idx is updated
    }
    // Skip block comment
    if ((source[idx] == TXT('/')) && (source[idx+1] == TXT('*'))) {
      SkipToEndBlockComment(source, idx); // Idx is updated
    }
    SkipWhiteSpaces(source, idx);
    String keyword;
    if (ReadWord(source, idx, keyword)) {
      if (string_util::IsEqual(keyword.GetConst(), TXT("uniform"))) {
        String type_name;
        String parameter_name;
        int array_count = 1;
        bool is_array = false;
        if (!ReadWord(source, idx, type_name)) {
          DG_UNREACHABLE();
          return;
        }
        if (!ReadWord(source, idx, parameter_name)) {
          DG_UNREACHABLE();
          return;
        }
        if (source[idx] == TXT('[')) {
          is_array = true;
          String temp;
          // Eat out '['
          ReadWord(source, idx, temp);
          // Read the count
          ReadWord(source, idx, temp);
          array_count = temp.ToInt();
          Check(array_count >= 1);
        }
        ShaderParameter::ValueType value_type = GetShaderParameterValueTypeByName(type_name);
        Check(value_type != ShaderParameter::kUnknownType);
        ShaderParameter param;
        param.set_name(parameter_name.GetConst());
        param.set_value_type(value_type);
        // Set default value for the float_array type
        switch (value_type) {
        case ShaderParameter::kVector4Type:
          param.SetVector4(&Vector4::kZero);
          break;
        case ShaderParameter::kMatrixType:
          param.SetMatrix(&Matrix::kIdentity);
          break;
        }          
        // Register index will be set at runtime by querying the index using the program
        Check(param.register_index() == -1);
        param.set_register_count(GetShaderParameterRegisterCountByType(value_type, array_count));
        out_parameters.PushBack(param);
      }
    }
  }
}

bool ShaderCompiler::CompileShader(
    ShaderProfile profile,
    const Cstr* include_base_dir,
    const BlobBase* source_blob,
    Shader* out_shader) {
  // Set the shader profile
  out_shader->profile_ = profile;
#if defined(_UNICODE)
  const AnsiStr* ansi_str = reinterpret_cast<const AnsiStr*>(source_blob->data_const());
  const int kAnsiStrSize = MyStrLenAnsi(ansi_str);
  int src_size = string_util::AnsiToUnicode(ansi_str, kAnsiStrSize, 0, 0);
  UnicodeStr* src_buffer = new UnicodeStr[src_size+1];
  string_util::AnsiToUnicode(ansi_str, kAnsiStrSize, src_buffer, src_size);
  src_buffer[src_size] = TXT('\0');
#else
  const Cstr* src_buffer = source_blob->data_const();
  int src_size = source_blob->size();
#endif
  // Parse the "uniform" parameters
  FillParameters(
      src_buffer,
      src_size,
      out_shader->type_,
      out_shader->parameters_);
  // Set the blob member
  out_shader->blob_.CopyFrom(
      (uint8_t*)(source_blob->data_const()),
      source_blob->size());
#if defined(_UNICODE)
  delete [] src_buffer;
#endif
  return true;
}

} // namespace dg
