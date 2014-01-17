// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class Model;

// Scene is a collection of Models
class Scene : public Resource {
public:
  Scene();
  virtual ~Scene();
  void Bind();
  void UpdateTransform();
  void AddModel(Model* model);
  void RemoveModel(Model* model);
  int GetNumModels() const {
    return nodes_.Count();
  }
  Model* GetModel(int idx) {
    return nodes_.Get(idx).ptr();
  }
  void BindResource();
  void Submit();
  void RemoveAll();
  virtual void ReadFrom(Stream* stream);
  virtual void WriteTo(Stream* out_stream);

private:
  typedef Array<Ptr<Model> > SceneNodes;
  SceneNodes nodes_;

  DG_DECLARE_RESOURCE(Scene, Resource)
};

} // namespace dg
