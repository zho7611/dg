// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {
namespace collada_importer {

class Skin;
class Mesh;
class Material;

class SceneNode : public NodeBase {
public:
  enum NodeType {
      NodeType_None = 0,
      NodeType_Node,
      NodeType_Joint,
  };

  String name_; // E.g. "Bip01_L_Foot"
  String sid_; // E.g. "Bone01"
  NodeType node_type_;
  SceneNode* parent_;
  Skin* skin_;
  Mesh* mesh_;
  Array<Material*> materials_;
  Array<SceneNode*> children_;

  SceneNode();
  virtual void ReadFromXml(Element& elem);
  SceneNode* GetSkeletonRoot();
  const Matrix& GetMatrix() const {
    return matrix_;
  }

private:
  float translation_[3];
  float rotation_x_[3];
  float rotation_y_[3];
  float rotation_z_[3];
  float scale_[3];
  Matrix matrix_;
  String skeleton_root_;

  void LoadBindMaterialFromXml(Element& elem);
  void LoadInstanceControllerFromXml(Element& elem);
  void LoadInstanceGeometryFromXml(Element& elem);
};

class Scene {
public:
  typedef StringMap<SceneNode*> NodeMap;
  NodeMap nodes_;

  SceneNode* Find(const String& node_id);
  SceneNode* FindBySidSlow(const String& sid);
  void Add(const String& node_id, SceneNode* node);
};

} // End of namespace collada_importer
} // End of namespace dg
