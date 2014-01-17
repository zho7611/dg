// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class SkinMesh : public Mesh {
public:
  typedef FixedArray<SkinJoint> SkinJoints;
  typedef VertexFormat::FormatPosNorTexTan TargetVertex;
  typedef VertexFormat::FormatPosNorTexTanJi4Jw4 SkinVertex;
  typedef FixedArray<SkinVertex> SkinVertices;

  SkinJoints skin_joints_; // Selected set of joints from the skeleton joints
  Matrix bind_matrix_; // Base bind shape matrix
  SkinVertices skin_vertices_;

  SkinMesh();
  virtual ~SkinMesh();
  virtual void UpdateTransform();
  void AttachSkeleton(Skeleton* skeleton);
  virtual void ReadFrom(Stream* stream);
  virtual void WriteTo(Stream* out_stream);
  const Skeleton* skeleton() const {
    return skeleton_.ptr();
  }
  const Joint& GetSkeletonJointBySkinJoint(int skin_joint_index) const;
  SkinJoint* FindSkinJointByIdSlow(const Name& joint_id);

private:
  Ptr<Skeleton> skeleton_;

  void UpdateBindToModelMatrices();

  DG_DECLARE_RESOURCE(SkinMesh, Mesh);
};

} // namespace dg
