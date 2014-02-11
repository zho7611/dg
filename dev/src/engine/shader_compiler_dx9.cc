// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

class ShaderIncludeDx9 : public ID3DXInclude {
public:
  // Base shader directory name + path separator
  String base_dir_;

  ShaderIncludeDx9(const Cstr* basePath) {
    base_dir_.Set(basePath);
    base_dir_.AddCharRight(TXT('/'));
  }

private:
  STDMETHOD(Open)(
      D3DXINCLUDE_TYPE include_type,
      LPCSTR file_name,
      LPCVOID parent_data,
      LPCVOID * data_pointer,
      UINT * bytes) {
    String include_path;
    include_path.Join(base_dir_, file_name);
    Ptr<Chunk> chunk(CreateReadChunk(include_path.Get()));
    uint8_t* buff = new uint8_t[chunk->size()];
    MyMemCpy(buff, chunk->data(), chunk->size());
    *data_pointer = buff;
    *bytes = chunk->size();
    return ERROR_SUCCESS;
  }

  STDMETHOD(Close)(LPCVOID data) {
    DG_SAFE_DELETE(data);
    return ERROR_SUCCESS;
  }
};

class ShaderMacro {
public:
  ShaderMacro() : count_(0) {
    PushNull();
  }

  D3DXMACRO* GetMacrosDx9() {
    return &items_.Get(0);
  }

  // ShaderMacro doesn't care about the memory deallocation for name and definition
  int Add(const AnsiStr* name, const AnsiStr* definition) {
    D3DXMACRO* item = GetItem(name);
    if (item) {
      item->Definition = definition;
      return count_;
    }
    // Add new
    item = &items_.Last();
    item->Name = name;
    item->Definition = definition;
    ++count_;
    PushNull();
    return count_;
  }

  D3DXMACRO* GetItem(const AnsiStr* name) {
    for (int idx = 0; idx < count_; ++idx) {
      D3DXMACRO& item = items_.Get(idx);
      if (string_util::IsEqual(name, item.Name)) {
        return &item;
      }
    }
    return NULL;
  }

  void PushNull() {
    D3DXMACRO nullItem = { 0, 0 };
    items_.PushBack(nullItem);
  }

  int count_; // Except the last Null value
  Array<D3DXMACRO> items_;
};

static void SetParamForClassObject(ShaderParameter& param, D3DXCONSTANT_DESC& constant_desc) {
  switch (constant_desc.Type) {
  case D3DXPT_SAMPLER2D:
    param.set_value_type(ShaderParameter::kTextureType);
    param.set_register_index(constant_desc.RegisterIndex);
    break;
  default:
    DG_UNIMPLEMENTED();
  }
}

static void SetParamForClassVector(ShaderParameter& param, D3DXCONSTANT_DESC& constant_desc) {
  // For now, only support 1-row vector
  Check(constant_desc.Rows == 1);
  Check(constant_desc.Type == D3DXPT_FLOAT);
  switch (constant_desc.Columns) {
  case 4:
    param.set_value_type(ShaderParameter::kVector4Type);
    if (constant_desc.RegisterCount > 1) {
      param.SetVector4Array(NULL, constant_desc.RegisterCount);
    } else {
      param.SetVector4(&Vector4::kZero);
    }
    break;
  default:
    DG_UNIMPLEMENTED();
    break;
  }
}

// Fill the parameters from the constant table
static bool FillParameters(ID3DXConstantTable* table, ShaderType shader_type, Array<ShaderParameter>& out_parameters) {
  HRESULT hret;
  D3DXCONSTANTTABLE_DESC desc;
  hret = table->GetDesc(&desc);
  if (hret != D3D_OK) {
    return false;
  }
  const int kNumConstants = desc.Constants;
  out_parameters.Clear();
  out_parameters.Resize(kNumConstants);
  for (int idx = 0; idx < kNumConstants; ++idx) {
    D3DXCONSTANT_DESC constant_desc;
    UINT constantCount = 1;
    D3DXHANDLE handle = table->GetConstant(0, idx);
    hret = table->GetConstantDesc(handle, &constant_desc, &constantCount);
    Check(constantCount == 1);
    if (hret != D3D_OK) {
      Check(0);
      return false;
    }
    ShaderParameter& param = out_parameters.Get(idx);
    param.set_name(String(constant_desc.Name).GetConst());
    param.set_shader_type(shader_type);
    param.set_register_index(constant_desc.RegisterIndex);
    param.set_register_count(constant_desc.RegisterCount);
    DG_LOG_LINE(TXT("shadercompilerdx9.fillparameters: name:%s register-index:%d class:%d type:%d count:%d"),
        param.name().GetConst(),
        param.register_index(),
        constant_desc.Class,
        constant_desc.Type,
        param.register_count());
    switch (constant_desc.Class) {
    case D3DXPC_OBJECT:
      SetParamForClassObject(param, constant_desc);
      break;
    case D3DXPC_MATRIX_ROWS:
      param.SetDefaultValueByType(ShaderParameter::kMatrixType);
      // Only 4x4 matrix is supported for now
      Check(constant_desc.Rows == 4);
      Check(constant_desc.Columns == 4);
      break;
    case D3DXPC_MATRIX_COLUMNS:
      param.set_value_type(ShaderParameter::kMatrixType);
      // Only 4x4 matrix is supported for now
      Check(constant_desc.Rows == 4);
      Check(constant_desc.Columns == 4);
      break;
    case D3DXPC_VECTOR:
      SetParamForClassVector(param, constant_desc);
      break;
    default:
      DG_UNIMPLEMENTED();
      break;
    }
  }
  return true;
}

static bool GetProfileDx9(ShaderProfile profile, const AnsiStr*& outProfileDx9) {
  switch (profile) {
  case ShaderProfile_VS_11:
    outProfileDx9 = ANSI_TXT("vs_1_1");
    break;
  case ShaderProfile_VS_30:
    outProfileDx9 = ANSI_TXT("vs_3_0");
    break;
  case ShaderProfile_PS_20:
    outProfileDx9 = ANSI_TXT("ps_2_0");
    break;
  case ShaderProfile_PS_30:
    outProfileDx9 = ANSI_TXT("ps_3_0");
    break;
  default:
    return false;
  };
  return true;
}

// Function:
//  Compile the shader text into the shader binary
// Parameters:
//  include_base_dir - the shader include base directory
//  source_blob - the shader text buffer
//  out_shader - the shader with compiled binary
// Returns:
//  true if succeeded, false if failed
//
bool ShaderCompiler::CompileShader(
    ShaderProfile profile,
    const Cstr* include_base_dir,
    const BlobBase* source_blob,
    Shader* out_shader) {
  // The main-entry point function should be "Main" for now
  AnsiStr main_func_name[] = ANSI_TXT("Main");
  ShaderMacro defines;
  // #todo Add feature for definition
  defines.Add(ANSI_TXT("Name1"), ANSI_TXT("Definition1"));
  DWORD flags = D3DXSHADER_PACKMATRIX_ROWMAJOR;
#if defined(DG_DEBUG)
  flags |= D3DXSHADER_DEBUG;
#endif
  // Shader include path
  ShaderIncludeDx9 shader_include(include_base_dir);
  ScopedRelease<ID3DXBuffer> buffer;
  ScopedRelease<ID3DXBuffer> compile_errors;
  ScopedRelease<ID3DXConstantTable> table;
  const AnsiStr* profile_dx9 = NULL;
  // Convert to dx9 profile
  GetProfileDx9(profile, profile_dx9);
  // Compile the shader using dx9 shader compiler
  HRESULT ret = ::D3DXCompileShader(
      (LPCSTR)(source_blob->data_const()),
      source_blob->size(),
      defines.GetMacrosDx9(),
      &shader_include,
      main_func_name,
      profile_dx9,
      flags,
      &buffer.ref(),
      &compile_errors.ref(),
      &table.ref());
  // If failed, log the error message
  if (ret != D3D_OK) {
    const AnsiStr* cur_buff = compile_errors->GetBufferPointer();
    String message(cur_buff);
    DG_LOG_LINE(TXT("error: shadercompile.compileshader.failed: %s"), message.Get());
    PopupMessage(TXT("Shader Compile Error"), TXT("shadercompiler.compileshader.failed: file_name:%s content:<<<%s>>>"), include_base_dir, message.Get());
    Check(0);
    return false;
  }
  DG_LOG_LINE(TXT("shadercompiler.compileshader.compiling: includebasedir:%s"), include_base_dir);
  // Fill the parameters
  if (!FillParameters(table.ptr(), out_shader->type_, out_shader->parameters_)) {
    Check(0);
    return false;
  }
  // Set the shader profile
  out_shader->profile_ = profile;
  // Set the blob member
  out_shader->blob_.CopyFrom(
      (uint8_t*)(buffer->GetBufferPointer()),
      buffer->GetBufferSize());
  return true;
}

} // namespace dg
