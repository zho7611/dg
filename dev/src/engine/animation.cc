// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

Animation::Animation() {}

Animation::~Animation() {}

void Animation::ReadFrom(Stream* stream) {
  StreamReadClass(stream, channels_);
  OnReadDone();
}

void Animation::WriteTo(Stream* out_stream) {
  StreamWriteClass(out_stream, channels_);
  OnWriteDone();
}

const Channel* Animation::FindChannelByNameSlow(const Name& name) const {
  const int kNumChannels = channels_.Count();
  for (int idx = 0; idx < kNumChannels; ++idx) {
    if (channels_.GetConst(idx).name().IsEqual(name)) {
      return &channels_.GetConst(idx);
    }
  }
  return NULL;
}

int Animation::GetNumSamples() const {
  if (channels_.IsEmpty()) {
    return 0;
  }
  return channels_.GetConst(0).num_samples();
}

void AnimationSet::ReadFrom(Stream* stream) {}

void AnimationSet::WriteTo(Stream* out_stream) {}

AnimationController::AnimationController() :
  time_in_milli_seconds_(0.f),
  is_reversed_(false),
  is_playing_(false),
  sample_frames_per_second_(15.f) {}

AnimationController::~AnimationController() {}

void AnimationController::Update(float delta_in_milli_seconds) {
  if (is_playing_) {
    time_in_milli_seconds_ += (is_reversed_) ? -delta_in_milli_seconds : delta_in_milli_seconds;
  }
}

void AnimationController::Play() {
  is_playing_ = true;
}

void AnimationController::Pause() {
  is_playing_ = false;
}

void AnimationController::FlipPlayPause() {
  if (is_playing_) {
    Pause();
  } else {
    Play();
  }
}

} // namespace dg
