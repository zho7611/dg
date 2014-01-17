// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {
namespace collada_importer {

class Mesh;

class Skin : public RefCount {
public:
  struct SkinVertex {
    void AddJointIndexWeight(int index, float weight);

    Array<int> joint_indices_;
    Array<float> joint_weights_;
  };

  struct JointIndexWeightIndex {
    JointIndexWeightIndex();
    JointIndexWeightIndex(int joint_index, int weight_index);

    int joint_index_;
    int weight_index_;
  };

  String id_;
  int num_vertices_;
  Matrix bind_shape_matrix_;
  Array<SkinVertex> skin_vertices_;
  EnumArray<Source::Format, Ptr<Source> > sources_;
  Array<int> num_joint_per_vertex_;

  Skin(const String& skinId);
  ~Skin();
  void ReadFromXml(Element& elem);
  void BuildSkinVertices();
  Mesh* GetMesh() const {
    return mesh_;
  }
  void StoreSkinMesh(SkinMesh& skinMesh);

private:
  int total_weights_;
  Mesh* mesh_;
  Array<JointIndexWeightIndex> joint_indices_weight_indices_;
  StringMap<Ptr<Source> > source_by_id_;

  void LoadJointsFromXml(Element& elem);
  void LoadVertexWeightsFromXml(Element& elem);
};

} // End of namespace collada_importer
} // End of namespace dg
