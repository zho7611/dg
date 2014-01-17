// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class Channel {
public:
  enum Semantic {
    Semantic_None = 0,
    Semantic_Position,
    Semantic_Rotation,
    Semantic_Matrix,
  };

  Channel();
  void SetChannel(int num_samples, Semantic semantic);
  int GetStrideFromSemantic(Semantic semantic) const;
  template<class T> void GetValue(int sample, T& out_value) const;
  template<class T> void SetValue(int sample, const T& value);
  void ReadFrom(Stream* stream);
  void WriteTo(Stream* out_stream);
  const Name& name() const { return name_; }
  void set_name(const Name& name) { name_.Set(name); }
  int num_samples() const { return num_samples_; }
  int stride() const { return stride_; }

private:
  // Should be unique for an animation
  Name name_;
  Semantic semantic_;
  int stride_;
  int num_samples_;
  AutoDeleteBlob blob_;
};

template<class T>
void Channel::GetValue(int sample, T& out_value) const {
  Check(blob_.size() >= (int)sizeof(out_value)*(sample+1));
  MyMemCpy(
      &out_value,
      (T*)(blob_.data_const()) + sample,
      sizeof(out_value));
}

template<class T>
void Channel::SetValue(int sample, const T& value) {
  Check(blob_.size() >= (int)sizeof(value)*(sample+1));
  MyMemCpy(
      (T*)(blob_.data()) + sample,
      &value,
      sizeof(value));
}

} // namespace dg
