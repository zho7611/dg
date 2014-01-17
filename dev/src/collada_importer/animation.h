// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {
namespace collada_importer {

typedef dg::Animation EngineAnimation;

// The current version of the collada_importer exports the animation with a single channel
// Thus, the collada_importer::Animation matches the dg::Channel, not the dg::Animation
class Animation : public NodeBase {
public:
  typedef StringMap<Ptr<Source> > SourceType;

  SourceType sources_;
  Sampler sampler_;
  Channel channel_;

  virtual void ReadFromXml(Element& elem);
  Source* GetSource(const String& source_id);

private:
};

} // End of namespace collada_importer
} // End of namespace dg
