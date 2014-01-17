// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {
namespace collada_importer {

class NodeBase : public RefCount {
public:
  String id_;

  virtual void ReadFromXml(Element& elem) = 0;
};

} // End of namespace collada_importer
} // End of namespace dg
