// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class Mesh;
class Material;
class Animation;

class Model : public Resource {
public:
  typedef FixedArray<Ptr<Part> > Parts;
  typedef FixedArray<Ptr<Model> > Children;

  Ptr<Model> parent_;
  Parts parts_;
  Children children_;
  Transform local_transform_;

  Model();
  virtual ~Model();
  void AddPart(Mesh* mesh, Material* material);
  void UnlinkChild(int index);
  void LinkChild(int index, Model* child);
  void SetNumChild(int num_children);
  void BindAnimation(Animation* animation);
  virtual void UpdateTransform();
  virtual void ReadFrom(Stream* stream);
  virtual void WriteTo(Stream* out_stream);
  SkinJoint* FindSkinJointByIdSlow(const Name& joint_id);
  // This is valid only after UpdateTransform()
  const Transform& world_transform_cache() const {
    return world_transform_cache_;
  }
  const Transform& GetWorldTransform() {
    UpdateTransform();
    return world_transform_cache_;
  }
  void Submit();
  void BindResource();

private:
  Transform world_transform_cache_;
  Ptr<Animation> animation_;

  DG_DECLARE_RESOURCE(Model, Resource);
};

namespace model_util {

Model* CreateBoxModel(const Vector3* pos = NULL, const Vector3* box_size = NULL, const Vector4* diffuse_color = NULL, const Vector4* ambient_color = NULL);
Model* CreateSphereModel(const Vector3* pos = NULL, float radius = 1.0f, const Vector4* diffuse_color = NULL, const Vector4* ambient_color = NULL);

} // namespace model_util

} // namespace dg
