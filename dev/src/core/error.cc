// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "core_first.h"

namespace dg {

static ErrorCode g_last_error = ErrorCode_None;
static unsigned int kErrorIdx = 0;

void LogErrorMsg(const Cstr* message) {
  g_last_error = ErrorCode_Unknown;
  g_logger->PushFormatLine(TXT("core.error: msg:%s"), message);
  ++kErrorIdx;
  Check(0);
}

void LogErrorMsgFormat(const Cstr* format, ...) {
  va_list va;
  va_start(va, format);
  String buff;
  buff.Format(format, va);
  va_end(va);
  LogErrorMsg(buff.GetConst());
}

void LogErrorCode(ErrorCode code) {
  g_last_error = code;
  g_logger->PushFormatLine(
      TXT("core.error: errorcode:%d msg:%s idx:%d"),
      code,
      GetCodeMsg(code),
      kErrorIdx++);
  Check(0);
}

void LogInfoMsg(const Cstr* message) {
  g_logger->PushFormatLine(TXT("core.info: msg:%s"), message);
}

void LogInfoMsgFormat(const Cstr* format, ...) {
  va_list va;
  va_start(va, format);
  String buff;
  buff.Format(format, va);
  va_end(va);
  LogInfoMsg(buff.GetConst());
}

void LogInfoCode(ErrorCode code) {
  g_logger->PushFormatLine(TXT("core.info: code:%d msg:%s"), code, GetCodeMsg(code));
}

void LogWarnMsg(const Cstr* message) {
  g_logger->PushFormatLine(TXT("core.warn: msg:%s"), message);
}

void LogWarnMsgFormat(const Cstr* format, ...) {
  va_list va;
  va_start(va, format);
  String buff;
  buff.Format(format, va);
  va_end(va);
  LogWarnMsg(buff.GetConst());
}

void LogWarnCode(ErrorCode code) {
  g_logger->PushFormatLine(TXT("core.warn: code:%d msg:%s"), code, GetCodeMsg(code));
}

ErrorCode GetLastErrorCode() {
  return g_last_error;
}

#if defined(DG_SHIP)
const Cstr* GetCodeMsg(ErrorCode code) {
  return TXT("N/A");
}
#else
#define DG_ERROR_CODE_MSG(CODE) case ErrorCode_##CODE: return TXT(#CODE);
const Cstr* GetCodeMsg(ErrorCode code) {
  switch (code) {
    DG_ERROR_CODE_MSG(None);
    DG_ERROR_CODE_MSG(Unknown);
    DG_ERROR_CODE_MSG(FileNotFound);
    DG_ERROR_CODE_MSG(OutOfMemory);
    DG_ERROR_CODE_MSG(ReadFailed);
    DG_ERROR_CODE_MSG(WriteFailed);
    DG_ERROR_CODE_MSG(BindDeviceFailed);
    DG_ERROR_CODE_MSG(UnbindDeviceFailed);
    DG_ERROR_CODE_MSG(SerializeToDeviceFailed);
    DG_ERROR_CODE_MSG(SerializeFromDeviceFailed);
    DG_ERROR_CODE_MSG(CreateDeviceFailed);
    DG_ERROR_CODE_MSG(EffectCompileError);
    DG_ERROR_CODE_MSG(ReferenceCountNotCleared);
    DG_ERROR_CODE_MSG(OutOfIndex);
    DG_ERROR_CODE_MSG(InvalidParameters);

    // #adderrorcodehere [[
    //DG_ERROR_CODE_MSG(XXXX);
    //DG_ERROR_CODE_MSG(XXXX);
    //DG_ERROR_CODE_MSG(XXXX);
    //DG_ERROR_CODE_MSG(XXXX);
    //DG_ERROR_CODE_MSG(XXXX);
    //DG_ERROR_CODE_MSG(XXXX);
    //DG_ERROR_CODE_MSG(XXXX);
    //DG_ERROR_CODE_MSG(XXXX);
    //DG_ERROR_CODE_MSG(XXXX);
    //DG_ERROR_CODE_MSG(XXXX);
    //DG_ERROR_CODE_MSG(XXXX);
    //DG_ERROR_CODE_MSG(XXXX);
    //DG_ERROR_CODE_MSG(XXXX);
    //DG_ERROR_CODE_MSG(XXXX);
    //DG_ERROR_CODE_MSG(XXXX);
    //DG_ERROR_CODE_MSG(XXXX);
    //DG_ERROR_CODE_MSG(XXXX);
    // ]] #adderrorcodehere
  }
  DG_UNREACHABLE();
  return TXT("N/A");
}
#endif // #ifndef DG_SHIP

} // namespace dg
