// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "collada_importer_first.h"

using namespace dg;
using namespace dg::collada_importer;

void collada_importer::Animation::ReadFromXml(Element& elem) {
  Check(elem.HasTagName(TXT("animation")));
  elem.GetAttribute(TXT("id"), id_);
  DG_FOR_EACH_ELEMENT(elem, sub_elem) {
    if (sub_elem.HasTagName(TXT("source"))) {
      Ptr<Source> source = new Source;
      source->ReadFromXml(sub_elem);
      sources_.Add(source->id_, source);
    } else if (sub_elem.HasTagName(TXT("channel"))) {
      // This should be called only once
      channel_.animation_ = this;
      channel_.ReadFromXml(sub_elem);
    } else if (sub_elem.HasTagName(TXT("sampler"))) {
      // This should be called only once
      sampler_.animation_ = this;
      sampler_.ReadFromXml(sub_elem);
    }
  }
}

Source* collada_importer::Animation::GetSource(const String &source_id) {
  return sources_.Get(source_id).ptr();
}
