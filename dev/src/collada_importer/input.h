// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {
namespace collada_importer {

class Input : public NodeBase {
public:
  enum Semantic {
      Semantic_None = 0,
      Semantic_Joint,
      Semantic_Weight,
      Semantic_Input,
      Semantic_Output,
      Semantic_Interpolation,
  };

  Semantic semantic_;
  int offset_;
  Source* GetSource();

  virtual void ReadFromXml(Element& elem);

private:
  String source_id_;
  Ptr<Source> source_;
};

} // End of namespace collada_importer
} // End of namespace dg
