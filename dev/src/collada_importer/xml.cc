// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "collada_importer_first.h"

using namespace dg;
using namespace dg::collada_importer;

Document::Document() {
  is_opened_ = false;
}

Document::~Document() {
  Check(is_opened_ == false);
}

bool Document::Open(const Cstr* path) {
  Check(is_opened_ == false);
  CoInitialize(0);
  if (FAILED(document_.CreateInstance(
      TXT("Msxml2.DOMDocument.6.0")))) {
    return false;
  }
  VARIANT_BOOL result = document_->load(path);
  is_opened_ = true;
  return (result == VARIANT_TRUE);
}

void Document::Close() {
  Check(is_opened_ == true);
  document_.Release();
  CoUninitialize();
  is_opened_ = false;
}

Element Document::GetDocumentElement() {
  return Element(document_->documentElement);
}

Element& Element::operator=(Element& rhs) {
  CopyFrom(rhs);
  return *this;
}

Element& Element::operator=(ElementPtr elem) {
  elem_ = elem;
  return *this;
}

void Element::CopyFrom(const Element& rhs) {
  if (this == &rhs) {
    return;
  }
  elem_ = rhs.elem_;
}

bool Element::GetSubElement(
    const Cstr* tag_name,
    Element& out_sub_elem_found) {
  NodeListPtr children = elem_->GetchildNodes();
  for (long idx = 0; idx < children->Getlength(); ++idx ) {
    Element sub_elem(children->Getitem(idx));
    if (sub_elem.HasTagName(tag_name)) {
      out_sub_elem_found = sub_elem;
      return true;
    }
  }
  return false;
}

Element Element::GetSubElement(const Cstr* tag_name) {
  Element sub_elem;
  bool result = GetSubElement(tag_name, sub_elem);
  Check(result);
  return sub_elem;
}

bool Element::HasTagName(const Cstr* tag_name) {
  return (elem_->GettagName() == _bstr_t(tag_name));
}

bool Element::HasAttribute(
    const Cstr* key,
    const Cstr* value) {
  _variant_t attribute = elem_->getAttribute(key);
  return (_bstr_t(attribute.bstrVal) == _bstr_t(value));
}

bool Element::GetAttribute(const Cstr* key,
    String& out_value) {
  _variant_t attribute = elem_->getAttribute(key);
  if (attribute.vt == VT_NULL) {
    return false;
  }
  out_value.Set((const Cstr*)_bstr_t(attribute.bstrVal));
  return true;
}

bool Element::GetAttribute(const Cstr* key,
    int& out_value) {
  _variant_t attribute = elem_->getAttribute(key);
  if (attribute.vt == VT_NULL) {
    return false;
  }
  out_value = MyAtoi((const Cstr*)_bstr_t(attribute.bstrVal));
  return true;
}

bool Element::GetTagName(String& out_tag_name) {
  out_tag_name.Set((const Cstr*)elem_->GettagName());
  return true;
}

bool Element::GetValue(String& out_value) {
  _variant_t val = elem_->GetnodeTypedValue();
  if (val.vt == VT_NULL) {
    Check(val.vt != VT_NULL);
    return false;
  }
  out_value.Set((const Cstr*)(_bstr_t(val)));
  return true;
}

ElementPtr Element::GetElementPtr() {
  return elem_;
}

static void TrimReferenceMarkInternal(const String& name, String& OutName) {
  const Cstr* name_in_chars = name.GetConst();
  Check(name.size());
  if (name_in_chars[0] == '#') {
    OutName = &(name_in_chars[1]);
  }
}

void dg::collada_importer::TrimReferenceMark(String& inout_name) {
  String name;
  TrimReferenceMarkInternal(inout_name, name);
  inout_name = name;
}
