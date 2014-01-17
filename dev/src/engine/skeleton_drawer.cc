// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

void SkeletonDrawer::DrawSkinMesh(SkinMesh& skinMesh) {
  ColorInt color = DG_MAKE_COLOR_INT(255, 255, 255, 255);
  LineDrawer& drawer = g_line_drawer;
  Vector3 start_point = Vector3::kZero;
  Vector3 end_point = Vector3::kZero;
  const Transform& model_tm = model_->GetWorldTransform();
  drawer.PreRender();
  for (int idx = 0; idx < skinMesh.skin_joints_.Count(); ++idx) {
    Matrix& cur_matrix = skinMesh.skin_joints_.Get(idx).animation_matrix_;
    cur_matrix.TransformCoord(end_point, Vector3::kZero);
    Vector3 temp = end_point;
    model_tm.TransformCoord(end_point, temp);
    drawer.DrawLine(start_point, end_point, color);
    start_point = end_point;
  }
  drawer.PostRender();
}

void SkeletonDrawer::Draw() {
  if (model_) {
    for (int idx = 0; idx < model_->parts_.Count(); ++idx) {
      SkinMesh* skin_mesh = SafeCast<SkinMesh>(
          model_->parts_.Get(idx)->mesh_);
      if (skin_mesh) {
        DrawSkinMesh(*skin_mesh);
      }
    }
  }
}

} // namespace dg
