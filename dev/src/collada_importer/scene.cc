// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "collada_importer_first.h"

using namespace dg;
using namespace dg::collada_importer;

SceneNode::SceneNode() :
    node_type_(NodeType_None),
    parent_(0),
    skin_(0),
    mesh_(0) {
  matrix_.MakeIdentity();
  translation_[0] = translation_[1] = translation_[2] = 0;
  rotation_z_[0] = rotation_z_[1] = rotation_z_[2] = 0;
  rotation_y_[0] = rotation_y_[1] = rotation_y_[2] = 0;
  rotation_x_[0] = rotation_x_[1] = rotation_x_[2] = 0;
  scale_[0] = scale_[1] = scale_[2] = 1.f;
}

void SceneNode::LoadBindMaterialFromXml(Element& elem) {
  Element& tech_common = elem.GetSubElement(TXT("technique_common"));
  DG_FOR_EACH_ELEMENT(tech_common, tech_child) {
    if (tech_child.HasTagName(TXT("instance_material"))) {
      String target_str;
      tech_child.GetAttribute(TXT("target"), target_str);
      TrimReferenceMark(target_str);
      Material* material = collada_importer::g_collada_importer.FindMaterial(target_str);
      Check(material);
      materials_.PushBack(material);
    }
  }
}

void SceneNode::LoadInstanceControllerFromXml(Element& elem) {
  String url_str;
  elem.GetAttribute(TXT("url"), url_str);
  TrimReferenceMark(url_str);
  skin_ = collada_importer::g_collada_importer.FindSkin(url_str);
  mesh_ = skin_->GetMesh();
  Check(skin_);
  DG_FOR_EACH_ELEMENT(elem, sub_elem) {
    if (sub_elem.HasTagName(TXT("skeleton"))) {
      sub_elem.GetValue(skeleton_root_);
      TrimReferenceMark(skeleton_root_);
    } else if (sub_elem.HasTagName(TXT("bind_material"))) {
      LoadBindMaterialFromXml(sub_elem);
    }
  }
}

void SceneNode::LoadInstanceGeometryFromXml(Element& elem) {
  String url_str;
  elem.GetAttribute(TXT("url"), url_str);
  TrimReferenceMark(url_str);
  mesh_ = collada_importer::g_collada_importer.FindMesh(url_str);
  Check(mesh_);
  DG_FOR_EACH_ELEMENT(elem, sub_elem) {
    if (sub_elem.HasTagName(TXT("bind_material"))) {
      LoadBindMaterialFromXml(sub_elem);
    }
  }
}

void SceneNode::ReadFromXml(Element& elem) {
  elem.GetAttribute(TXT("id"), id_);
  String typeStr;
  elem.GetAttribute(TXT("type"), typeStr);
  elem.GetAttribute(TXT("name"), name_);
  elem.GetAttribute(TXT("sid"), sid_);
  if (parent_ && (parent_->node_type_ == SceneNode::NodeType_Joint) &&
      (typeStr == TXT("NODE"))) {
    // The nodes from the joint are ignored
    DG_LOG_LINE(
        TXT("export.scenenode.readfromxml.node.ignored: name:%s"),
        name_.Get());
    return;
  }
  if (typeStr == TXT("NODE")) {
    node_type_ = SceneNode::NodeType_Node;
  } else if (typeStr == TXT("JOINT")) {
    node_type_ = SceneNode::NodeType_Joint;
  } else {
    DG_UNREACHABLE();
  }
  if (parent_) {
    parent_->children_.PushBack(this);
  }
  collada_importer::g_collada_importer.AddSceneNode(id_, this);
  DG_FOR_EACH_ELEMENT(elem, sub_elem) {
    if (sub_elem.HasTagName(TXT("instance_controller"))) {
      LoadInstanceControllerFromXml(sub_elem);
    } else if (sub_elem.HasTagName(TXT("instance_geometry"))) {
      LoadInstanceGeometryFromXml(sub_elem);
    } else if (sub_elem.HasTagName(TXT("translate"))) {
      String subElemStr;
      sub_elem.GetValue(subElemStr);
      StringStream floatStream(subElemStr.Get());
      floatStream.ReadFloat(translation_[0]);
      floatStream.ReadFloat(translation_[1]);
      floatStream.ReadFloat(translation_[2]);
    } else if (sub_elem.HasTagName(TXT("rotate"))) {
      String subElemStr;
      sub_elem.GetValue(subElemStr);
      StringStream floatStream(subElemStr.Get());
      String sidStr;
      sub_elem.GetAttribute(TXT("sid"), sidStr);
      if (sidStr == TXT("RotZ")) {
        floatStream.ReadFloat(rotation_z_[0]);
        floatStream.ReadFloat(rotation_z_[1]);
        floatStream.ReadFloat(rotation_z_[2]);
      } else if (sidStr== TXT("RotX")) {
        floatStream.ReadFloat(rotation_x_[0]);
        floatStream.ReadFloat(rotation_x_[1]);
        floatStream.ReadFloat(rotation_x_[2]);
      } else if (sidStr == TXT("RotY")) {
        floatStream.ReadFloat(rotation_y_[0]);
        floatStream.ReadFloat(rotation_y_[1]);
        floatStream.ReadFloat(rotation_y_[2]);
      }
    } else if (sub_elem.HasTagName(TXT("scale"))) {
      String subElemStr;
      sub_elem.GetValue(subElemStr);
      StringStream floatStream(subElemStr.Get());
      floatStream.ReadFloat(scale_[0]);
      floatStream.ReadFloat(scale_[1]);
      floatStream.ReadFloat(scale_[2]);
    } else if (sub_elem.HasTagName(TXT("node"))) {
      Ptr<SceneNode> child = new SceneNode;
      child->parent_ = this;
      child->ReadFromXml(sub_elem);
    } else if (sub_elem.HasTagName(TXT("matrix"))) {
      collada_importer::g_collada_importer.ReadMatrix(sub_elem, matrix_);
    } else {
      String tag_name;
      sub_elem.GetTagName(tag_name);
      DG_LOG_LINE(TXT("error: export.loadfromxml.unimplemented-node: tag:%s"), tag_name.Get());
    }
  }
}

void collada_importer::Scene::Add(const String& node_id, SceneNode* node) {
  nodes_.Add(node_id.GetConst(), node);
}

SceneNode* collada_importer::Scene::Find(const String& node_id) {
  return nodes_.FindOrNull(node_id);
}

SceneNode* collada_importer::Scene::FindBySidSlow(const String& sid) {
  NodeMap::Iterator it = nodes_.Begin();
  NodeMap::Iterator ie = nodes_.End();
  for (; it != ie; ++it) {
    SceneNode* node = it.Value();
    if (node) {
      if (node->sid_.IsEqual(sid)) {
        return node;
      }
    }
  }
  return NULL;
}

SceneNode* collada_importer::SceneNode::GetSkeletonRoot() {
  return collada_importer::g_collada_importer.scene_.Find(skeleton_root_);
}