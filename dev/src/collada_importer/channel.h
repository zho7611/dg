// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {
namespace collada_importer {

class Sampler;
class Animation;
typedef dg::Channel EngineChannel;

class Channel : public NodeBase {
public:
  Sampler* sampler_;
  Animation* animation_;

  Channel();
  virtual void ReadFromXml(Element& elem);
  virtual void Import();
  int num_samples() const;
  const String& GetInterpolationBySampleIdx(int idx) const;
  const String& GetTargetId() const {
    return target_id_;
  }
  const String& GetTargetSid() const {
    return target_sid_;
  }
  Source* GetOutputSource() {
    return sampler_->output_.ptr();
  }
  Source* GetInputSource() {
    return sampler_->input_.ptr();
  }
  void SetEngineChannel(EngineChannel* channel) {
    engine_channel_ = channel;
  }

private:
  String target_id_; // E.g. "Bip01-Node" from target "Bip01-Node/transform"q
  String target_sid_; // E.g. "transform" from target "Bip01-Node/transform"
  EngineChannel* engine_channel_;
};

} // End of namespace collada_importer
} // End of namespace dg
