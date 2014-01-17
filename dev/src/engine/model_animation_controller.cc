// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

ModelAnimationController::ModelAnimationController() : is_root_height_only_(true) {}

void ModelAnimationController::UpdateSkinMesh(SkinMesh& skin_mesh, float delta_in_milli_seconds) {
  AnimationController::Update(delta_in_milli_seconds);
  const int kNumSkinJoints = skin_mesh.skin_joints_.Count();
  const int kNumSamples = animation_->GetNumSamples();
  const int kCurrentFrame = (int)(sample_frames_per_second_*time_in_milli_seconds_/1000.f) % kNumSamples;
  for (int idx = 0; idx < kNumSkinJoints; ++idx) {
    Matrix& joint_mat = skin_mesh.skin_joints_.Get(idx).animation_matrix_;
    const Joint& joint = skin_mesh.GetSkeletonJointBySkinJoint(idx);
    // #todo make it faster
    const Channel* channel = animation_->FindChannelByNameSlow(joint.id_);
    channel->GetValue(kCurrentFrame, joint_mat);
    if (is_root_height_only_ && (joint.index_ == 0)) {
      joint_mat.m41_ = 0; // Clear the x-movement
      joint_mat.m42_ = 0; // Clear the y-movement
    }
    // #todo accumulate the parent animation in the importer
    if (joint.parent_index_ >= 0) { // If it has a parent, accumulate from the parent
      const int kParentIdx = skin_mesh.skin_joints_.Get(idx).skin_parent_index_;
      const Matrix& kParentAnimation =
          skin_mesh.skin_joints_.Get(kParentIdx).animation_matrix_;
      joint_mat.StoreMultiply(joint_mat, kParentAnimation);
    }
  }
}

void ModelAnimationController::Update(float delta_in_milli_seconds) {
  for (int idx = 0; idx < model_->parts_.Count(); ++idx) {
    SkinMesh* skin_mesh = SafeCast<SkinMesh>(model_->parts_.Get(idx)->mesh_);
    if (skin_mesh) {
      UpdateSkinMesh(*skin_mesh, delta_in_milli_seconds);
    }
  }
}

void ModelAnimationController::Play() {
  AnimationController::Play();
}

void ModelAnimationController::Pause() {
  AnimationController::Pause();
}

} // namespace dg
