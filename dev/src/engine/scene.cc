// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

Scene::Scene() {
}

Scene::~Scene() {
}

void Scene::AddModel(Model* model) {
  Check(model);
  nodes_.PushBack(model);
}

void Scene::RemoveModel(Model* model) {
  Check(model);
  int idx = nodes_.FindSlow(model);
  if (idx != -1) {
    nodes_.Erase(idx);
  }
}

void Scene::BindResource() {
  SceneNodes::Iterator it = nodes_.Begin();
  SceneNodes::Iterator ie = nodes_.End();
  Ptr<Model> model;
  for (; it != ie; ++it ) {
    model = *it;
    Check(model);
    model->BindResource();
  }
}

void Scene::Submit() {
  SceneNodes::Iterator it = nodes_.Begin();
  SceneNodes::Iterator ie = nodes_.End();
  Ptr<Model> model;
  for (; it != ie; ++it ) {
    model = *it;
    Check(model);
    model->Submit();
  }
}

void Scene::RemoveAll() {
  nodes_.Clear();
}

void Scene::UpdateTransform() {
  DG_SCOPED_PROFILER(TXT("scene.updatetm"), DG_MAKE_COLOR_ARGB(0xfffda099));
  SceneNodes::Iterator it = nodes_.Begin();
  SceneNodes::Iterator ie = nodes_.End();
  Ptr<Model> model;
  for (; it != ie; ++it ) {
    model = *it;
    Check(model);
    model->UpdateTransform();
  }
}

void Scene::ReadFrom(Stream* stream) {
  int num_nodes = 0;
  StreamReadFlat(stream, num_nodes);
  for (int idx = 0; idx < num_nodes; ++idx) {
    Ptr<Model> model;
    StreamReadPointer(stream, model);
    nodes_.PushBack(model);
  }
  OnReadDone();
}

void Scene::WriteTo(Stream* out_stream) {
  int numNodes = nodes_.Count();
  StreamWriteFlat(out_stream, numNodes);
  for (int idx = 0; idx < numNodes; ++idx) {
    StreamWritePointer(out_stream, nodes_.Get(idx));
  }
  OnWriteDone();
}

} // namespace dg
