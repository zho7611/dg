// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "collada_importer_first.h"

using namespace dg;
using namespace dg::collada_importer;

collada_importer::Mesh::Mesh() :
  sources_(Source::Format_Num) {
}

collada_importer::Mesh::~Mesh() {
}

void collada_importer::Mesh::ReadFromXml(Element& elem) {
  Check(elem.HasTagName(TXT("mesh")));
  DG_FOR_EACH_ELEMENT(elem, sub_elem) {
    if (sub_elem.HasTagName(TXT("source"))) {
      Ptr<Source> source = new Source;
      source->ReadFromXml(sub_elem);
      source_by_id_.Add(source->id_.Get(), source);
    } else if ((sub_elem.HasTagName(TXT("vertices")))) {
      LoadVerticesFromXml(sub_elem);
    } else if ((sub_elem.HasTagName(TXT("triangles")))) {
      Triangles* new_triangles = new Triangles;
      new_triangles->mesh_ = this;
      new_triangles->ReadFromXml(sub_elem);
      triangles_list_.PushBack(new_triangles);
    } else if ((sub_elem.HasTagName(TXT("polygons")))) {
      DG_UNIMPLEMENTED(); // Deprecated
    } else if ((sub_elem.HasTagName(TXT("polylist")))) {
      // Check "Triangles" in the export dialog box
      DG_UNIMPLEMENTED();
    }
  }
}

void collada_importer::Mesh::LoadVerticesFromXml(Element& elem) {
  Element sub_elem;
  if (elem.GetSubElement(TXT("input"), sub_elem)) {
    if (sub_elem.HasAttribute(TXT("semantic"), TXT("POSITION"))) {
      String source_id;
      sub_elem.GetAttribute(TXT("source"), source_id);
      TrimReferenceMark(source_id);
      vertices_source_ = source_by_id_.Get(source_id.Get());
    }
    // Remap source id to vertex id
    elem.GetAttribute(TXT("id"), vertices_source_->id_);
  }
}
