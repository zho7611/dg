// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class SamplerStateSet : public List<Ptr<SamplerStateBase> > {
public:
  SamplerStateBase* Find(SamplerStateType type);
  void Apply();
  void ReadFrom(Stream* stream);
  void WriteTo(Stream* out_stream);
};

} // namespace dg
