// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class SkinMesh;

class SkeletonDrawer {
public:
  Ptr<Model> model_;

  void Draw();

private:
  void DrawSkinMesh(SkinMesh& skinMesh);
};

} // namespace dg
