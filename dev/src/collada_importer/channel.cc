// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "collada_importer_first.h"

using namespace dg;
using namespace dg::collada_importer;

collada_importer::Channel::Channel() :
  sampler_(0),
  animation_(0),
  engine_channel_(0) {
}

void collada_importer::Channel::ReadFromXml(Element& elem) {
  Check(animation_);
  // Set sampler
  {
    String samplerId;
    elem.GetAttribute(TXT("source"), samplerId);
    TrimReferenceMark(samplerId);
    Check(animation_->sampler_.id_.IsEqual(samplerId));
    sampler_ = &animation_->sampler_;
  }
  // Parse target
  {
    String target;
    elem.GetAttribute(TXT("target"), target);
    // Parse the target address into target_id_ and target_sid_
    {
      Array<String> splited;
      String splitSeparators(string_util::kSlashChar);
      string_util::Split(splited, target.Get(), splitSeparators.GetConst());
      target_id_ = splited.Get(0);
      target_sid_ = splited.Get(1);
      Check(!target_id_.IsEmpty());
      Check(!target_sid_.IsEmpty());
      if (!target_sid_.IsEqual(TXT("transform"))) {
        dg::LogErrorMsg(TXT("collada_importer.channel.loadfromxml.error: msg:'non-transform sid'"));
        DG_UNIMPLEMENTED();
      }
    }
  }
}

int collada_importer::Channel::num_samples() const {
  const Source::FloatValues& timeValues = sampler_->input_->GetFloatValues();
  return timeValues.Count();
}

const String& collada_importer::Channel::GetInterpolationBySampleIdx(int idx) const {
  Source::StringValues& interpolations = sampler_->interpolation_->GetStringValues();
  return interpolations.GetConst(idx);
}

void collada_importer::Channel::Import() {
  Check(engine_channel_);
  // For now, the channel only supports Matrix semantic
  {
    engine_channel_->set_name(target_id_.Get());
    engine_channel_->SetChannel(num_samples(), EngineChannel::Semantic_Matrix);
  }
  // Set the each sample channel
  {
    Source* source = GetOutputSource();
    Check(source);
    Matrix mat;
    Vector3 scale, translation;
    Quaternion rotation;
    // For each sample, fill the matrix
    for (int idx = 0; idx < num_samples(); ++idx) {
      Check(GetInterpolationBySampleIdx(idx).IsEqualNoCase(TXT("LINEAR")));
      source->GetMatrix(idx, mat);
      engine_channel_->SetValue(idx, mat);
    }
  }
}
