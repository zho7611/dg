// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class Model;
class SkinMesh;

class ModelAnimationController : public AnimationController {
public:
  bool is_root_height_only_;
  Ptr<Model> model_;

  ModelAnimationController();
  virtual void Update(float delta_in_milli_seconds);
  virtual void Play();
  virtual void Pause();

private:
  void UpdateSkinMesh(SkinMesh& skinMesh, float delta_in_milli_seconds);
};

} // namespace dg
