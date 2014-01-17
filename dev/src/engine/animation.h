// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class Animation : public Resource {
public:
  typedef FixedArray<Channel> ChannelsType;

  Animation();
  virtual ~Animation();
  virtual void ReadFrom(Stream* stream);
  virtual void WriteTo(Stream* out_stream);
  const Channel* FindChannelByNameSlow(const Name& name) const;
  int GetNumSamples() const;
  ChannelsType& channels() {
    return channels_;
  }

protected:
  ChannelsType channels_;

  DG_DECLARE_RESOURCE(Animation, Resource);
};

class AnimationController : public RefCount {
public:
  Ptr<Animation> animation_;
  float sample_frames_per_second_;

  AnimationController();
  ~AnimationController();
  virtual void Update(float delta_in_milli_seconds);
  virtual void Play();
  virtual void Pause();
  virtual void FlipPlayPause();
  bool is_playing() const { return is_playing_; }

protected:
  float time_in_milli_seconds_;
  bool is_reversed_;
  bool is_playing_;
};

class AnimationSet : public Resource {
public:
  Array<Ptr<Animation> > animations_;

  virtual void ReadFrom(Stream* stream);
  virtual void WriteTo(Stream* out_stream);
};

} // namespace dg
