// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class CameraController : public RefCount {
public:
  Ptr<Camera> camera_;

  virtual void Update() = 0;
};

class CameraModelFollowController : public CameraController {
public:
  CameraModelFollowController();
  void Attach(Camera* camera, Model* model, Name& joint_id);
  virtual void Update();

private:
  Ptr<Model> model_;
  Name joint_id_;
  SkinJoint* skin_joint_;
  Vector3 eye_offset_;
};

} // namespace dg
