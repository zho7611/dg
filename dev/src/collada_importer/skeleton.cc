// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "collada_importer_first.h"

using namespace dg;
using namespace dg::collada_importer;

bool collada_importer::Skeleton::GetJointIndexBySid(
    SceneNode* joint_node,
    const String& sid,
    int& joint_index) {
  Check(joint_index >= -1);
  if (joint_node->sid_.IsEqual(sid)) {
    return true;
  }
  for (int idx = 0; idx < joint_node->children_.Count(); ++idx) {
    if (GetJointIndexBySid(
        joint_node->children_.Get(idx), sid, ++joint_index)) {
      return true;
    }
  }
  return false;
}

collada_importer::SceneNode* collada_importer::Skeleton::FindRootJoint(Skin* skin) {
  Ptr<Source> joint_source = skin->sources_.Get(Source::Format_Joint);
  String joint_id;
  joint_source->GetString(0, joint_id);
  SceneNode* cur_joint_node =
      collada_importer::g_collada_importer.scene_.FindBySidSlow(joint_id.Get());
  Check(cur_joint_node);
  while (cur_joint_node->parent_) {
    cur_joint_node = cur_joint_node->parent_;
  }
  Check(cur_joint_node->node_type_ == SceneNode::NodeType_Joint);
  return cur_joint_node;
}

int collada_importer::Skeleton::GetNumChildRecursive(
    SceneNode* node,
    int& out_num_children_total) {
  int num_children = node->children_.Count();
  for (int idx = 0; idx < num_children; ++idx) {
    GetNumChildRecursive(node->children_.Get(idx), out_num_children_total);
  }
  out_num_children_total += num_children;
  return out_num_children_total;
}

EngineSkeleton* collada_importer::Skeleton::CreateEngineSkeleton(SceneNode* root_joint) {
  int num_joints = 1; // Count itself first
  GetNumChildRecursive(root_joint, num_joints);
  EngineSkeleton* engine_skeleton = new EngineSkeleton;
  engine_skeleton->Resize(num_joints);
  int joint_index = 0;
  StoreJointsRecursive(root_joint, root_joint, *engine_skeleton, joint_index);
  return engine_skeleton;
}

// Store joint recursively
int collada_importer::Skeleton::StoreJointsRecursive(
    SceneNode* root_joint_node,
    SceneNode* joint_node,
    EngineSkeleton& engine_skeleton,
    int& joint_index) {
  Joint& joint = engine_skeleton.GetJoint(joint_index);
  joint.id_= joint_node->id_;
  joint.index_ = joint_index;
  joint.parent_index_ = -1;
  if (joint_node->parent_) {
    // Set the parent index
    int parent_idx = 0;
    GetJointIndexBySid(
        root_joint_node,
        joint_node->parent_->sid_,
        parent_idx);
    joint.parent_index_ = parent_idx;
    Check(joint.parent_index_ >= 0);
  }
  int num_children = joint_node->children_.Count();
  for (int idx = 0; idx < num_children; ++idx) {
    StoreJointsRecursive(
        root_joint_node,
        joint_node->children_.Get(idx),
        engine_skeleton,
        ++joint_index);
  }
  return joint_index;
}
