// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "collada_importer_first.h"

using namespace dg;
using namespace dg::collada_importer;

Skin::Skin(const String& skinId) :
  id_(skinId),
  num_vertices_(0),
  total_weights_(0),
  sources_(Source::Format_Num),
  mesh_(0) {
}

Skin::~Skin() {
}

Skin::JointIndexWeightIndex::JointIndexWeightIndex() {
}

Skin::JointIndexWeightIndex::JointIndexWeightIndex(int joint_index, int weight_index) :
  joint_index_(joint_index),
  weight_index_(weight_index) {
}

void Skin::SkinVertex::AddJointIndexWeight(int index, float weight) {
  joint_indices_.PushBack(index);
  joint_weights_.PushBack(weight);
  //DG_OUTPUT_DEBUG(TXT("[%d:%1.4f]\n"), index, weight);
}

void Skin::BuildSkinVertices() {
  Ptr<Source> weightSource(sources_.Get(Source::Format_Weight));
  int it = 0; // Iterator for JointIndexWeightIndex
  for (int weight_idx = 0; weight_idx < num_vertices_; ++weight_idx) {
    SkinVertex skin_vertex;
    int num_influence = num_joint_per_vertex_.Get(weight_idx);
    for (int inf_idx = 0; inf_idx < num_influence;
        ++inf_idx, ++it) {
      JointIndexWeightIndex& jiw_idx =
          joint_indices_weight_indices_.Get(it);
      int joint_index = jiw_idx.joint_index_;
      int weight_idx = jiw_idx.weight_index_;
      float weight;
      weightSource->GetFloat(weight_idx, weight);
      skin_vertex.AddJointIndexWeight(joint_index, weight);
    }
    skin_vertices_.PushBack(skin_vertex);
  }
}

void Skin::ReadFromXml(Element& elem) {
  Check(elem.HasTagName(TXT("skin")));
  String skin_source_geometry;
  elem.GetAttribute(TXT("source"), skin_source_geometry);
  TrimReferenceMark(skin_source_geometry);
  Mesh* found_mesh = collada_importer::g_collada_importer.FindMesh(skin_source_geometry);
  Check(found_mesh);
  found_mesh->skin_ = this;
  mesh_ = found_mesh;
  DG_FOR_EACH_ELEMENT(elem, sub_elem) {
    if (sub_elem.HasTagName(TXT("bind_shape_matrix"))) {
      ColladaImporter::ReadMatrix(sub_elem, bind_shape_matrix_);
    } else if (sub_elem.HasTagName(TXT("source"))) {
      Ptr<Source> source = new Source;
      source->ReadFromXml(sub_elem);
      source_by_id_.Add(source->id_.Get(), source);
    } else if ((sub_elem.HasTagName(TXT("joints")))) {
      LoadJointsFromXml(sub_elem);
    } else if ((sub_elem.HasTagName(TXT("vertex_weights")))) {
      LoadVertexWeightsFromXml(sub_elem);
    } else {
      DG_UNIMPLEMENTED();
    }
  }
}

void Skin::LoadJointsFromXml(Element& elem) {
  DG_FOR_EACH_ELEMENT(elem, sub_elem) {
    Check(sub_elem.HasTagName(TXT("input")));
    if (sub_elem.HasAttribute(TXT("semantic"), TXT("JOINT"))) {
      String source_id;
      sub_elem.GetAttribute(TXT("source"), source_id);
      TrimReferenceMark(source_id);
      Ptr<Source> curSource(
          source_by_id_.Get(source_id.Get()));
      sources_.Get(Source::Format_Joint) = curSource;
    } else if (sub_elem.HasAttribute(TXT("semantic"), TXT("INV_BIND_MATRIX"))) {
      String source_id;
      sub_elem.GetAttribute(TXT("source"), source_id);
      TrimReferenceMark(source_id);
      Ptr<Source> curSource(
          source_by_id_.Get(source_id.Get()));
      sources_.Get(Source::Format_InvBindMatrix) = curSource;
    }
  }
}

void Skin::LoadVertexWeightsFromXml(Element& elem) {
  String count_str;
  elem.GetAttribute(TXT("count"), count_str);
  num_vertices_ = MyAtoi(count_str.Get());
  DG_FOR_EACH_ELEMENT(elem, sub_elem) {
    if (sub_elem.HasTagName(TXT("vcount"))) {
      int num_joints;
      int count = num_vertices_;
      String sum_elem_str;
      sub_elem.GetValue(sum_elem_str);
      StringStream int_stream(sum_elem_str.Get());
      while (count--) {
        if (int_stream.ReadInt(num_joints)) {
          total_weights_ += num_joints;
          num_joint_per_vertex_.PushBack(num_joints);
        }
      }
    } else if (sub_elem.HasTagName(TXT("v"))) {
      String sum_elem_str;
      sub_elem.GetValue(sum_elem_str);
      StringStream int_stream(sum_elem_str.Get());
      int joint_index, weight_idx;
      int count = total_weights_;
      Check(count);
      while (count--) {
        int_stream.ReadInt(joint_index);
        int_stream.ReadInt(weight_idx);
        // Not support (-1) joint_index. The (-1) index stands
        // For the bind_shape_matrix_
        Check(joint_index >= 0);
        joint_indices_weight_indices_.PushBack(
            JointIndexWeightIndex(joint_index, weight_idx));
      }
    } else if (sub_elem.HasAttribute(TXT("semantic"), TXT("WEIGHT"))) {
      String source_id;
      sub_elem.GetAttribute(TXT("source"), source_id);
      TrimReferenceMark(source_id);
      Check(sub_elem.HasAttribute(TXT("offset"), TXT("1")));
      Ptr<Source> curSource(source_by_id_.Get(source_id.Get()));
      sources_.Get(Source::Format_Weight) = curSource;
    }
  }
  BuildSkinVertices();
}

void collada_importer::Skin::StoreSkinMesh(SkinMesh& skinMesh) {
  Ptr<Source> joint_source = sources_.Get(Source::Format_Joint);
  Ptr<Source> inv_bind_source = sources_.Get(Source::Format_InvBindMatrix);
  int num_skin_joints = joint_source->NumValues_;
  SceneNode* the_root = Skeleton::FindRootJoint(this);
  skinMesh.skin_joints_.Resize(num_skin_joints);
  skinMesh.bind_matrix_ = bind_shape_matrix_;
  // For each skin joint, joint index of the skin_mesh_out
  for (int idx = 0; idx < num_skin_joints; ++idx) {
    int joint_index = 0; // Idx of Engine.SkinMesh.skeleton_
    String sid_str; // Joint sid_str(e.g. "Bone01") from the collada_importer.Skin.sources_
    // Get the joint id from the selected joint source
    joint_source->GetString(idx, sid_str);
    // Get the joint index from
    bool is_found = Skeleton::GetJointIndexBySid(the_root, sid_str, joint_index);
    if (!is_found) {
      DG_UNREACHABLE();
      continue;
    }
    // Set the inverse bind matrix for the skeleton as well
    Joint& joint = collada_importer::g_collada_importer.engine_skeleton_->GetJoint(joint_index);
    inv_bind_source->GetMatrix(idx, joint.inverse_bind_matrix_);
    skinMesh.skin_joints_.Get(idx).skeleton_joint_index_ = joint_index;
    // Initialize the skin joint animation matrix
    skinMesh.skin_joints_.Get(idx).animation_matrix_.StoreInverse(joint.inverse_bind_matrix_);
    skinMesh.skin_joints_.Get(idx).bind_to_model_matrix_.MakeIdentity();
    // Set the parent index
    {
      int parent_idx = -1;
      // Search only through the previous joints
      for (int idx2 = 0; idx2 < idx; ++idx2) {
        if (joint.parent_index_ == skinMesh.skin_joints_.Get(idx2).skeleton_joint_index_) {
          parent_idx = idx2;
        }
      }
      skinMesh.skin_joints_.Get(idx).skin_parent_index_ = parent_idx;
    }
  }
}
