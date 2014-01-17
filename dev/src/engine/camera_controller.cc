// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

CameraModelFollowController::CameraModelFollowController() : skin_joint_(0) {}

void CameraModelFollowController::Attach(Camera* camera, Model* model, Name& joint_id) {
  camera_ = camera;
  joint_id_ = joint_id;
  model_ = model;
  skin_joint_ = model_->FindSkinJointByIdSlow(joint_id_);
  if (camera) {
    eye_offset_ = camera->view_desc.eye_pos - camera->view_desc.target_pos;
  }
}

void CameraModelFollowController::Update() {
  if (skin_joint_) {
    Vector3 target_pos;
    skin_joint_->animation_matrix_.GetPosition(target_pos);
    model_->UpdateTransform();
    const Transform& model_tm = model_->world_transform_cache();
    model_tm.TransformCoord(target_pos, target_pos);
    if (camera_) {
      camera_->view_desc.target_pos = target_pos;
      camera_->view_desc.eye_pos = target_pos + eye_offset_;
    }
  }
}

} // namespace dg
