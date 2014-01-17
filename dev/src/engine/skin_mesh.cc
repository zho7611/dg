// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

void SkinMesh::ReadFrom(Stream* stream) {
  Check(skin_vertices_.IsEmpty());
  // Read mesh
  Mesh::ReadFrom(stream);
  // Read [num_skin_vertices] and reserve skin_vertices_
  {
    int num_skin_vertices;
    StreamReadFlat(stream, num_skin_vertices);
    Check(num_skin_vertices);
    skin_vertices_.Resize(num_skin_vertices);
  }
  // Read [joint_index/weight] and fill in vertex buff
  const TargetVertex* buff = reinterpret_cast<const TargetVertex*>(
      vertex_buffer_.blob().data_const());
  for (int vertex_idx = 0; vertex_idx < skin_vertices_.Count(); ++vertex_idx) {
    skin_vertices_.Get(vertex_idx).position = buff[vertex_idx].position;
    skin_vertices_.Get(vertex_idx).normal = buff[vertex_idx].normal;
    skin_vertices_.Get(vertex_idx).tex_coord = buff[vertex_idx].tex_coord;
    skin_vertices_.Get(vertex_idx).tangent = buff[vertex_idx].tangent;
    for (int influence_idx = 0;
        influence_idx < VertexFormat::kMaxJointInfluence;
        ++influence_idx) {
      StreamReadFlat(stream, 
          skin_vertices_.Get(vertex_idx).joint_indices[influence_idx]);
      StreamReadFlat(stream, 
          skin_vertices_.Get(vertex_idx).joint_weights[influence_idx]);
    }
  }
  // Read [bind_matrix_]
  StreamReadClass(stream, bind_matrix_);
  // Read [skin_joints_]
  StreamReadClass(stream, skin_joints_);
  // Read [skeleton_]
  StreamReadPointer(stream, skeleton_);
  AttachSkeleton(skeleton_.ptr());
  OnReadDone();
}

void SkinMesh::WriteTo(Stream* out_stream) {
  // Write mesh
  Mesh::WriteTo(out_stream);
  // Write [num_skin_vertices]
  StreamWriteFlat(out_stream, skin_vertices_.Count());
  // Write [joint_index/weight]
  for (int vertex_idx = 0; vertex_idx < skin_vertices_.Count(); ++vertex_idx) {
    for (int influence_idx = 0; influence_idx < VertexFormat::kMaxJointInfluence; ++influence_idx) {
      StreamWriteFlat(out_stream, 
          skin_vertices_.Get(vertex_idx).joint_indices[influence_idx]);
      StreamWriteFlat(out_stream, 
          skin_vertices_.Get(vertex_idx).joint_weights[influence_idx]);
    }
  }
  // Write [bind_matrix_]
  StreamWriteClass(out_stream, bind_matrix_);
  // Write [skin_joints_]
  StreamWriteClass(out_stream, skin_joints_);
  // Write [skeleton_]
  StreamWritePointer(out_stream, skeleton_);
  OnWriteDone();
}

void SkinMesh::AttachSkeleton(Skeleton* skeleton) {
  skeleton_ = skeleton;
}

static void TransformVertex(
    SkinMesh::TargetVertex* target,
    const SkinMesh::SkinVertex* source,
    const Matrix& bind_to_model_matrix) {
  bind_to_model_matrix.TransformCoord(target->position, source->position);
  bind_to_model_matrix.TransformNormal(target->normal, source->normal);
  Vector3 src_tangent(source->tangent.x, source->tangent.y, source->tangent.z);
  Vector3 target_tangent(target->tangent.x, target->tangent.y, target->tangent.z);
  float sign = source->tangent.w;
  bind_to_model_matrix.TransformNormal(target_tangent, src_tangent);
  target->tangent.Set(target_tangent, sign);
}

// Update the vertex buffer with the skinning
// It does not update the blob_. It directly update the vertex buffer resource
void SkinMesh::UpdateTransform() {
  DG_SCOPED_PROFILER(TXT("skinmesh.updatetm"), DG_MAKE_COLOR_ARGB(0xffadc9a5));
  UpdateBindToModelMatrices();
  Check(skin_vertices_.Count() == vertex_buffer_.num_vertices());
  Check(vertex_buffer_.GetDeclPreset() == VertexDecl::Preset_PosNorTexTan);
  TargetVertex* target_vertices;
  VertexBuffer::ScopedLock locker(vertex_buffer_, target_vertices);
  {
    TargetVertex transformed;
    SkinVertex* skin_vertex;
    TargetVertex* target_vertex;
    float weight_sum = 0;
    int influence_idx = 0;
    float joint_weight = 0;
    int joint_index = 0;
    const int num_vertices = skin_vertices_.Count();
    int vertex_idx = 0;
    DG_SCOPED_PROFILER(TXT("skinmesh.updatetm.transform"), DG_MAKE_COLOR_ARGB(0xffebda50));
    for (vertex_idx = 0; vertex_idx < num_vertices; ++vertex_idx) {
      target_vertex = &target_vertices[vertex_idx];
      skin_vertex = &skin_vertices_.Get(vertex_idx);
      // Init the target vertex
      MyMemSet(target_vertex, 0, sizeof(*target_vertex));
      target_vertex->tex_coord = skin_vertex->tex_coord;
      weight_sum = 0;
      for (influence_idx = 0; influence_idx< VertexFormat::kMaxJointInfluence; ++influence_idx) {
        joint_weight = skin_vertex->joint_weights[influence_idx];
        if (joint_weight <= 0) {
          continue;
        }
        weight_sum += joint_weight;
        joint_index = skin_vertex->joint_indices[influence_idx];
        if (joint_index < 0) {
          // Copy vertex
          transformed.position = skin_vertex->position;
          transformed.normal = skin_vertex->normal;
          transformed.tangent = skin_vertex->tangent;
        } else {
          TransformVertex(&transformed, skin_vertex, skin_joints_.Get(joint_index).bind_to_model_matrix_);
        }
        // Add weighted vertex
        target_vertex->position += transformed.position*joint_weight;
        target_vertex->normal += transformed.normal*joint_weight;
        target_vertex->tangent += transformed.tangent*joint_weight;
      }
    }
  }
}

void SkinMesh::UpdateBindToModelMatrices() {
  DG_SCOPED_PROFILER(TXT("skinmesh.updatematrix"), DG_MAKE_COLOR_ARGB(0xff242874));
  const int kNumJoints = skin_joints_.Count();
  for (int idx = 0; idx < kNumJoints; ++idx) {
    int joint_index = skin_joints_.Get(idx).skeleton_joint_index_;
    Joint& joint = skeleton_->GetJoint(joint_index);
    Matrix& bind_to_model = skin_joints_.Get(idx).bind_to_model_matrix_;
    const Matrix& inv_bind = joint.inverse_bind_matrix_;
    // Bind_to_model = bind_matrix_ * inv_bind * animation_matrix_
    bind_to_model.StoreMultiply(bind_matrix_, inv_bind);
    bind_to_model.StoreMultiply(bind_to_model, skin_joints_.Get(idx).animation_matrix_);
  }
}

const Joint& SkinMesh::GetSkeletonJointBySkinJoint(int skin_joint_index) const {
  int skel_joint_idx = skin_joints_.GetConst(skin_joint_index).skeleton_joint_index_;
  return skeleton_->GetJoint(skel_joint_idx);
}

SkinJoint* SkinMesh::FindSkinJointByIdSlow(const Name& joint_id) {
  const Joint* joint = skeleton_->FindJointByIdSlow(joint_id);
  if (!joint) {
    return 0;
  }
  for (int idx = 0; idx < skin_joints_.Count(); ++idx) {
    if (skin_joints_.Get(idx).skeleton_joint_index_ == joint->index_) {
      return &skin_joints_.Get(idx);
    }
  }
  return 0;
}

} // namespace dg
