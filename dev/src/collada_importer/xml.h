// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

#import "msxml6.dll" no_auto_exclude

namespace dg {
namespace collada_importer {

typedef MSXML2::IXMLDOMNodeListPtr NodeListPtr;
typedef MSXML2::IXMLDOMDocumentPtr DocumentPtr;
typedef MSXML2::IXMLDOMElementPtr ElementPtr;

class Element {
public:
  Element() {}
  Element(ElementPtr elem) : elem_(elem) {}
  Element(Element& rhs) {
    CopyFrom(rhs);
  }
  ~Element() {}
  Element& operator=(Element& rhs);
  bool GetSubElement(const Cstr* tag_name, Element& out_sub_elem_found);
  Element GetSubElement(const Cstr* tag_name);
  bool HasTagName(const Cstr* tag_name);
  bool HasAttribute(const Cstr* key, const Cstr* value);
  bool GetAttribute(const Cstr* key, dg::String& out_value);
  bool GetAttribute(const Cstr* key, int& out_value);
  bool GetTagName(dg::String& out_tag_name);
  Element& operator=(ElementPtr elem);
  bool GetValue(dg::String& out_value);
  ElementPtr GetElementPtr();

private:
  void CopyFrom(const Element& rhs);

  ElementPtr elem_;
};

class Document {
public:
  Document();
  ~Document();
  bool Open(const Cstr* path);
  void Close();
  Element GetDocumentElement();

private:
  DocumentPtr document_;
  bool is_opened_;
};

void TrimReferenceMark(dg::String& inout_name);

#define DG_FOR_EACH_ELEMENT(ParentElem, ChildElem) \
    NodeListPtr ParentElem##NodeList = \
      ParentElem.GetElementPtr()->GetchildNodes(); \
    long ParentElem##Iterator; \
    Element ChildElem; \
    for( ParentElem##Iterator = 0, \
      ChildElem = ParentElem##NodeList->Getitem(0); \
    ParentElem##Iterator < ParentElem##NodeList->Getlength(); \
    ChildElem = ParentElem##NodeList->Getitem(++ParentElem##Iterator))

} // End of namespace collada_importer
} // End of namespace dg
