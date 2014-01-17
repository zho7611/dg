// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "core_first.h"
#include "ini_file_handler.h"

namespace dg {

IniFile::IniFile() :
    handler_(0) {}

IniFile::~IniFile() {
  DG_SAFE_DELETE(handler_);
}

bool IniFile::Load(const Cstr* file_name) {
  if (!File::IsFound(file_name)) {
    return false;
  }
  file_name_ = file_name;
  Check(!handler_);
  handler_ = new IniFileHandler;
  Ptr<Chunk> chunk(FileChunk::CreateReadChunk(file_name_.Get()));
  handler_->ReadFrom(chunk.ptr());
  return true;
}

bool IniFile::Save() {
  if (!handler_) {
    return false;
  }
  Ptr<Chunk> chunk(FileChunk::CreateWriteChunk(file_name_.Get()));
  handler_->WriteTo(chunk.ptr());
  return true;
}

bool IniFile::GetValue(const Cstr* section, const Cstr* key, String& out_value) {
  Check(handler_);
  return handler_->GetSectionKeyValue(String(section), String(key), out_value);
}

bool IniFile::SetValue(const Cstr* section, const Cstr* key, const String& value) {
  Check(handler_);
  return handler_->SetSectionKeyValue(String(section), String(key), value);
}

} // namespace dg
