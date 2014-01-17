// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {
namespace collada_importer {

typedef dg::Skeleton EngineSkeleton;

class Skeleton {
public:
  static SceneNode* FindRootJoint(Skin* skin);
  static EngineSkeleton* CreateEngineSkeleton(SceneNode* root_joint);
  static bool GetJointIndexBySid(
      SceneNode* joint_node,
      const String& sid,
      int& joint_index);

private:
  static int GetNumChildRecursive(
      SceneNode* node,
      int& out_num_children_total);
  static int StoreJointsRecursive(
      SceneNode* root_joint_node,
      SceneNode* joint_node,
      EngineSkeleton& engine_skeleton,
      int& joint_index);
};

} // End of namespace collada_importer
} // End of namespace dg
