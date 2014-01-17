// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "collada_importer_first.h"

using namespace dg;
using namespace dg::collada_importer;

void Sampler::ReadFromXml(Element& elem) {
  Check(animation_);
  elem.GetAttribute(TXT("id"), id_);
  DG_FOR_EACH_ELEMENT(elem, sub_elem) {
    String source_id;
    if (sub_elem.HasAttribute(TXT("semantic"), TXT("INPUT"))) {
      sub_elem.GetAttribute(TXT("source"), source_id);
      TrimReferenceMark(source_id);
      input_ = animation_->GetSource(source_id);
      Check(input_);
    } else if (sub_elem.HasAttribute(TXT("semantic"), TXT("OUTPUT"))) {
      sub_elem.GetAttribute(TXT("source"), source_id);
      TrimReferenceMark(source_id);
      output_ = animation_->GetSource(source_id);
      Check(output_);
    } else if (sub_elem.HasAttribute(TXT("semantic"), TXT("INTERPOLATION"))) {
      sub_elem.GetAttribute(TXT("source"), source_id);
      TrimReferenceMark(source_id);
      interpolation_ = animation_->GetSource(source_id);
      Check(interpolation_);
    }
  }
}
