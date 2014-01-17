// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

int VertexElement::GetElementSize() const {
  switch (data_type) {
  case DataType_Float1:
    return 4;
    break;
  case DataType_Float2:
    return 8;
    break;
  case DataType_Float3:
    return 12;
    break;
  case DataType_Float4:
    return 16;
    break;
  case DataType_Color:
    return 4;
    break;
  default:
    DG_UNREACHABLE();
    break;
  }
  return 0;
}

VertexDecl::VertexDecl() :
  decl_preset_(Preset_None) {
}

VertexDecl::~VertexDecl() {
}

int VertexDecl::stride() const {
  int stride = 0;
  for (int idx = 0; idx < elements_.Count(); ++idx) {
    stride += elements_.GetConst(idx).GetElementSize();
  }
  return stride;
}

void VertexDecl::CopyFrom(VertexDecl* decl) {
  decl_preset_ = decl->decl_preset_;
  elements_.CopyFrom(decl->elements_);
  OnReadDone();
}

void VertexDecl::InitByPreset(Preset preset) {
  decl_preset_ = preset;
  elements_.Clear();
  VertexElement pos(VertexElement::DataType_Float3, VertexElement::Usage_Position);
  VertexElement nor(VertexElement::DataType_Float3, VertexElement::Usage_Normal);
  VertexElement col(VertexElement::DataType_Color, VertexElement::Usage_Color);
  VertexElement tex(VertexElement::DataType_Float2, VertexElement::Usage_TexCoord);
  VertexElement tan(VertexElement::DataType_Float4, VertexElement::Usage_Tangent);
  VertexElement ji4(VertexElement::DataType_Float4, VertexElement::Usage_JointIndex);
  VertexElement jw4(VertexElement::DataType_Float4, VertexElement::Usage_TexCoord);
  switch (preset) {
  case Preset_PosCol:
    elements_.PushBack(pos);
    elements_.PushBack(col);
    break;
  case Preset_PosTex:
    elements_.PushBack(pos);
    elements_.PushBack(tex);
    break;
  case Preset_PosNorTexTan:
    elements_.PushBack(pos);
    elements_.PushBack(nor);
    elements_.PushBack(tex);
    elements_.PushBack(tan);
    break;
  case Preset_PosTexCol:
    elements_.PushBack(pos);
    elements_.PushBack(tex);
    elements_.PushBack(col);
    break;
  case Preset_PosNorTexTanJi4Jw4:
    elements_.PushBack(pos);
    elements_.PushBack(nor);
    elements_.PushBack(tex);
    elements_.PushBack(tan);
    elements_.PushBack(ji4);
    elements_.PushBack(jw4);
    break;
  }
  OnReadDone();
}

void VertexDecl::ReadFrom(Stream* stream) {
  Preset preset;
  StreamReadFlat(stream, preset);
  InitByPreset(preset);
  Check(decl_preset_ != Preset_None);
}

void VertexDecl::WriteTo(Stream* out_stream) {
  Check(decl_preset_ != Preset_None);
  StreamWriteFlat(out_stream, decl_preset_);
}

bool VertexDecl::OnInitDevice() {
  if (device_vertex_declaration_handle_) {
    return false;
  }
  RC_CreateVertexDeclaration(&elements_.GetConst(0), elements_.Count(), device_vertex_declaration_handle_);
  return true;
}

bool VertexDecl::OnClearDevice() {
  if (!device_vertex_declaration_handle_) {
    return false;
  }
  device_vertex_declaration_handle_.Release();
  return true;
}

void VertexDecl::SetCurrent() {
  BindResource();
  Check(device_vertex_declaration_handle_);
  RC_SetVertexDeclaration(device_vertex_declaration_handle_);
}

void VertexDecl::SubmitVertexDecl() {
  BindResource();
  Check(device_vertex_declaration_handle_);
  RC_SetVertexDeclaration(device_vertex_declaration_handle_);
}

} // namespace dg
