// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

Model::Model() : parent_(0) {}

Model::~Model() {
  Check(children_.Count()== 0);
}

void Model::AddPart(Mesh* mesh, Material* material) {
  Ptr<Part> part = new Part;
  part->mesh_ = mesh;
  part->material_ = material;
  parts_.Resize(parts_.Count()+1);
  parts_.Set(parts_.Count()-1, part);
}

void Model::BindResource() {
  const int num_parts = parts_.Count();
  for (int part_idx = 0; part_idx < num_parts; ++part_idx) {
    Ptr<Part>& part = parts_.Get(part_idx);
    if (part) {
      part->BindResource();
    }
  }
}

void Model::Submit() {
  // Set the world transform matrix parameter
  g_renderer.view_state.world_matrix = world_transform_cache_.ToMatrix();
  const int num_parts = parts_.Count();
  for (int part_idx = 0; part_idx < num_parts; ++part_idx) {
    Part* part = parts_.Get(part_idx).ptr();
    if (part) {
      Mesh* mesh = part->mesh_.ptr();
      Material* material = part->material_.ptr();
      ShaderEffect* effect = material->effect_.ptr();
      effect->SubmitMesh(mesh, material);
    }
  }
}

void Model::UpdateTransform() {
  DG_SCOPED_PROFILER(TXT("model.updatetm"), DG_MAKE_COLOR_ARGB(0xff5c9e99));
  if (parent_) {
    world_transform_cache_.Multiply(local_transform_, parent_->world_transform_cache_);
  } else {
    world_transform_cache_ = local_transform_;
  }
  for (int part_idx = 0; part_idx < parts_.Count(); ++part_idx) {
    parts_.Get(part_idx)->UpdateTransform();
  }
  for (int child_idx = 0; child_idx < children_.Count(); ++child_idx) {
    Ptr<Model>& child = children_.Get(child_idx);
    if (child) {
      child->UpdateTransform();
    }
  }
}

void Model::ReadFrom(Stream* stream) {
  StreamReadAuto(stream, local_transform_);
  StreamReadClass(stream, parts_);
  OnReadDone();
}

void Model::WriteTo(Stream* out_stream) {
  StreamWriteAuto(out_stream, local_transform_);
  StreamWriteClass(out_stream, parts_);
  OnWriteDone();
}

void Model::UnlinkChild(int index) {
  Ptr<Model>& child = children_.Get(index);
  if (child) {
    child->parent_ = NULL;
  }
}

void Model::LinkChild(int index, Model* child) {
  children_.Get(index) = child;
  child->parent_ = NULL;
}

void Model::SetNumChild(int num_children) {
  children_.Resize(num_children);
}

void Model::BindAnimation(Animation *animation) {
  animation_ = animation;
}

SkinJoint* Model::FindSkinJointByIdSlow(const Name &joint_id) {
  for (int idx = 0; idx < parts_.Count(); ++idx) {
    SkinMesh* skin_mesh = SafeCast<SkinMesh>(parts_.Get(idx)->mesh_);
    if (skin_mesh) {
      SkinJoint* joint = skin_mesh->FindSkinJointByIdSlow(joint_id);
      if (joint) {
        return joint;
      }
    }
  }
  return 0;
}

Model* model_util::CreateBoxModel(const Vector3* pos, const Vector3* box_size, const Vector4* diffuse_color, const Vector4* ambient_color) {
  Model* model = new Model;
  Mesh* mesh = new Mesh;
  MeshBuilder::BuildBox(*mesh, *box_size);
  Material* mat = new Material;
  material_util::InitSolidColorMaterial(mat, diffuse_color, ambient_color);
  {
    Ptr<Part> part = new Part;
    part->mesh_ = mesh;
    part->material_ = mat;
    part->OnReadDone();
    model->parts_.Resize(1);
    model->parts_.Get(0) = part;
  }
  model->OnReadDone();
  if (pos) model->local_transform_.position_ = *pos;
  else model->local_transform_.position_.Set(0, 0, 0);
  model->UpdateTransform();
  return model;
}

Model* model_util::CreateSphereModel(const Vector3* pos, float radius, const Vector4* diffuse_color, const Vector4* ambient_color) {
  Model* model = new Model;
  Mesh* mesh = new Mesh;
  MeshBuilder::BuildSphere(*mesh, radius);
  Material* mat = new Material;
  material_util::InitSolidColorMaterial(mat, diffuse_color, ambient_color);
  {
    Part* part = new Part;
    part->mesh_ = mesh;
    part->material_ = mat;
    part->OnReadDone();
    model->parts_.Resize(1);
    model->parts_.Get(0) = part;
  }
  model->OnReadDone();
  if (pos) model->local_transform_.position_ = *pos;
  else model->local_transform_.position_.Set(0, 0, 0);
  model->UpdateTransform();
  return model;
}

} // namespace dg
