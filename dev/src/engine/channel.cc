// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

Channel::Channel() : semantic_(Semantic_None),
  stride_(0),
  num_samples_(0) {}

void Channel::SetChannel(int numSamples, Semantic semantic) {
  Check(numSamples > 0);
  Check(num_samples_ == 0);
  Check(semantic_ == Semantic_None);
  Check(semantic != Semantic_None);
  Check(blob_.IsEmpty());
  semantic_ = semantic;
  num_samples_ = numSamples;
  stride_ = GetStrideFromSemantic(semantic_);
  blob_.Reserve(stride_*num_samples_);
}

void Channel::ReadFrom(Stream* stream) {
  StreamReadClass(stream, name_);
  StreamReadFlat(stream, stride_);
  StreamReadFlat(stream, num_samples_);
  StreamReadClass(stream, blob_);
}

void Channel::WriteTo(Stream* out_stream) {
  StreamWriteClass(out_stream, name_);
  StreamWriteFlat(out_stream, stride_);
  StreamWriteFlat(out_stream, num_samples_);
  StreamWriteClass(out_stream, blob_);
}

int Channel::GetStrideFromSemantic(Semantic semantic) const {
  switch (semantic) {
  case Semantic_None:
    return 0;
  case Semantic_Position:
    return sizeof(Vector3);
  case Semantic_Rotation:
    return sizeof(Quaternion);
  case Semantic_Matrix:
    return sizeof(Matrix);
  }
  return 0;
}

} // namespace dg
