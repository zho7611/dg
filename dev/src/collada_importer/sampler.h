// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {
namespace collada_importer {

class Animation;

class Sampler : public NodeBase {
public:
  Ptr<Source> input_;
  Ptr<Source> output_;
  Ptr<Source> interpolation_;
  Animation* animation_;

  virtual void ReadFromXml(Element& elem);
};

} // End of namespace collada_importer
} // End of namespace dg
