// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

enum ErrorCode {
  ErrorCode_None = 0,
  ErrorCode_Unknown,
  // File related
  ErrorCode_FileNotFound,
  ErrorCode_OutOfMemory,
  ErrorCode_ReadFailed,
  ErrorCode_WriteFailed,
  // Device state
  ErrorCode_BindDeviceFailed,
  ErrorCode_UnbindDeviceFailed,
  ErrorCode_SerializeToDeviceFailed,
  ErrorCode_SerializeFromDeviceFailed,
  ErrorCode_CreateDeviceFailed,
  // Effect compiler
  ErrorCode_EffectCompileError,
  ErrorCode_ReferenceCountNotCleared,
  ErrorCode_OutOfIndex,
  ErrorCode_InvalidParameters,
  // #adderrorcodehere [[
  //XXXX,
  //XXXX,
  //XXXX,
  //XXXX,
  //XXXX,
  //XXXX,
  //XXXX,
  //XXXX,
  //XXXX,
  //XXXX,
  //XXXX,
  //XXXX,
  //XXXX,
  //XXXX,
  //XXXX,
  // ]] #adderrorcodehere
  ErrorCode_Max = 0xffffffff,
};

// Log error
void LogErrorCode(ErrorCode code);
void LogErrorMsg(const Cstr* message);
void LogErrorMsgFormat(const Cstr* format, ...);
// Log info
void LogInfoCode(ErrorCode code);
void LogInfoMsg(const Cstr* message);
void LogInfoMsgFormat(const Cstr* format, ...);
// Log warning [[
void LogWarnCode(ErrorCode code);
void LogWarnMsg(const Cstr* message);
void LogWarnMsgFormat(const Cstr* format, ...);
// ]] log warning
ErrorCode GetLastErrorCode();
const Cstr* GetCodeMsg(ErrorCode code);

} // namespace dg
