// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

Camera::Camera() :
    is_orthogonal(false),
    origin_(0, 0, 0) {
  cascade_index = 0;
  is_light_camera = false;
  ortho_desc.left = ortho_desc.top_ =
      ortho_desc.bottom = ortho_desc.right =
      ortho_desc.near_plane = ortho_desc.far_plane = 0;
  proj_desc.aspect_ratio = 4.0f/3.0f;
  proj_desc.far_plane = 100.0f;
  proj_desc.near_plane = .1f;
  proj_desc.fov_y = DegreeToRadian(45.0f);
  view_desc.target_pos = Vector3(0, 0, 0);
  view_desc.eye_pos = Vector3(0, 0, -1.f);
  view_desc.up_dir = Vector3(0, 1.f, 0);
  proj_matrix_.MakeIdentity();
  view_matrix_.MakeIdentity();
  view_proj_matrix_.MakeIdentity();
}

Camera::~Camera() {}

void Camera::MakeViewMatrix(const ViewDesc& desc, Matrix& out_view_matrix) {}

void Camera::MakeProjMatrix(const ProjDesc& desc, Matrix& out_proj_matrix) {
  out_proj_matrix.PerspectiveFovLH(
      desc.fov_y,
      desc.aspect_ratio,
      desc.near_plane,
      desc.far_plane);
}

void Camera::MakeProjMatrix(const OrthoDesc& desc, Matrix& out_proj_matrix) {
  // Get the average distance as the orthogonal height
  out_proj_matrix.OrthoOffCenterLH(
      desc.left,
      desc.right,
      desc.bottom,
      desc.top_,
      desc.near_plane,
      desc.far_plane);
}

void Camera::UpdateProjMatrix() {
  if (is_orthogonal) {
    MakeProjMatrix(ortho_desc, proj_matrix_);
  } else {
    MakeProjMatrix(proj_desc, proj_matrix_);
  }
}

void Camera::UpdateViewProjMatrix() {
  view_proj_matrix_.StoreMultiply(view_matrix_, proj_matrix_);
}

void Camera::UpdateTransform() {
  UpdateProjMatrix();
  UpdateViewMatrix();
  UpdateViewProjMatrix();
}

Vector3& Camera::GetOrigin() {
  return origin_;
}

void Camera::ReadFrom(Stream* stream) {
  StreamReadFlat(stream, proj_desc);
  StreamReadFlat(stream, ortho_desc);
  StreamReadFlat(stream, view_desc);
  StreamReadFlat(stream, is_orthogonal);
  StreamReadFlat(stream, is_light_camera);
  StreamReadFlat(stream, proj_matrix_);
  StreamReadFlat(stream, view_matrix_);
  StreamReadFlat(stream, view_proj_matrix_);
  StreamReadFlat(stream, origin_);
}

void Camera::WriteTo(Stream* out_stream) {
  StreamWriteFlat(out_stream, proj_desc);
  StreamWriteFlat(out_stream, ortho_desc);
  StreamWriteFlat(out_stream, view_desc);
  StreamWriteFlat(out_stream, is_orthogonal);
  StreamWriteFlat(out_stream, is_light_camera);
  StreamWriteFlat(out_stream, proj_matrix_);
  StreamWriteFlat(out_stream, view_matrix_);
  StreamWriteFlat(out_stream, view_proj_matrix_);
  StreamWriteFlat(out_stream, origin_);
}

void Camera::UpdateViewMatrix() {
  view_matrix_.LookAtLH(
      &view_desc.eye_pos,
      &view_desc.target_pos,
      &view_desc.up_dir);
  origin_ = view_desc.eye_pos;
}

// This code originated from DXSDK Cascade Shadow Sample
void Camera::UpdateAsLightCamera(
    Camera* eye_camera, Camera* light_camera, Size2 view_size) {
  // Set the light camera
  {
    Vector3 cam_up_dir;
    Vector3 cam_right_dir;
    Vector3 cam_from_dir;
    eye_camera->view_matrix()->GetColumnAxis(0, cam_right_dir);
    eye_camera->view_matrix()->GetColumnAxis(1, cam_up_dir);
    eye_camera->view_matrix()->GetColumnAxis(2, cam_from_dir);
    // #fixme: remove hard-coded constants
    const float kEyeToTargetDistance = 2.0f;
    const float kLightDistance = 4.f;
    light_camera->view_desc.target_pos = eye_camera->GetOrigin() + cam_from_dir * kEyeToTargetDistance;
    light_camera->view_desc.eye_pos = light_camera->view_desc.target_pos + g_engine.light_manager->global_light_->direction_ * kLightDistance;
    light_camera->view_desc.up_dir = Vector3(1.0f, 0, 0);
    if (fabs(light_camera->view_desc.up_dir.Dot(g_engine.light_manager->global_light_->direction_) - 1.0f) <= kEpsilon) {
      light_camera->view_desc.up_dir = Vector3(0.0f, 1.f, 0);
    }
    Check(fabs(light_camera->view_desc.up_dir.Dot(g_engine.light_manager->global_light_->direction_) - 1.0f) > kEpsilon);
    Vector3 cross = light_camera->view_desc.up_dir.Cross(g_engine.light_manager->global_light_->direction_);
    light_camera->view_desc.up_dir = cross.Cross(g_engine.light_manager->global_light_->direction_);
  }
  light_camera->UpdateViewMatrix();
  const int cascade_index = light_camera->cascade_index;
  light_camera->light_view_matrices[cascade_index] = *light_camera->view_matrix();
  eye_camera->UpdateTransform();
  Matrix cam_proj = *eye_camera->proj_matrix();
  Matrix cam_proj_inv = cam_proj.GetInverse();
  Matrix cam_view_inv = eye_camera->view_matrix()->GetInverse();
  Matrix cam_proj_to_light_space = cam_proj_inv * cam_view_inv * *light_camera->view_matrix();
  Vector2 kEyeProjMinMax(0.f, 1.0f);
  const float kNearPlane = eye_camera->proj_desc.near_plane;
  const float kFarPlane = eye_camera->proj_desc.far_plane;
  Vector3 kNearCornersLightSpace[kNumShadowCascades] = {
    // Cam Space near plane corners
    Vector3::TransformCoord(Vector3(-1, -1, kEyeProjMinMax.x), cam_proj_to_light_space),
    Vector3::TransformCoord(Vector3( 1, -1, kEyeProjMinMax.x), cam_proj_to_light_space),
    Vector3::TransformCoord(Vector3(-1, 1, kEyeProjMinMax.x), cam_proj_to_light_space),
    Vector3::TransformCoord(Vector3( 1, 1, kEyeProjMinMax.x), cam_proj_to_light_space)
  };
  // Get Scene near and far
  Vector2 near_far(light_camera->ortho_desc.near_plane, light_camera->ortho_desc.far_plane);
  for (int idx = 0; idx < kNumShadowCascades; ++idx) {
    // Correcting light's near taking into account light frustum
    near_far.x = MyMin(near_far.x, kNearCornersLightSpace[idx].z);
  }
  // Camera-space near plane corner
  const float kCascadeIdxWeight = (float(cascade_index) + 1.f) / kNumShadowCascades;
  // Float camSpaceZ = a * n * pow( f / n, w ) + ( 1 - a ) * ( n + w * ( f - n ) );
  const float kCamSpaceZ = kNearPlane + kCascadeIdxWeight * ( kFarPlane - kNearPlane );
  const float kDepth = Vector3::TransformCoord(Vector3(0, 0, kCamSpaceZ), cam_proj).z;
  if (cascade_index != (kNumShadowCascades-1)) {
    g_shadow_manager.depths_[cascade_index] = kDepth;
  }
  float min_x(FLT_MAX), min_y(FLT_MAX), min_z(near_far.x);
  float max_x(-FLT_MAX), max_y(-FLT_MAX), max_z(near_far.x);
  Vector3 kSplitCornersLightSpace[kNumShadowCascades] = {
    // Cam-space split plane corners
    Vector3::TransformCoord(Vector3(-1, -1, kDepth), cam_proj_to_light_space),
    Vector3::TransformCoord(Vector3( 1, -1, kDepth), cam_proj_to_light_space),
    Vector3::TransformCoord(Vector3(-1, 1, kDepth), cam_proj_to_light_space),
    Vector3::TransformCoord(Vector3( 1, 1, kDepth), cam_proj_to_light_space)
  };
  for (int corner_idx = 0; corner_idx < kNumShadowCascades; ++corner_idx) {
    min_x = MyMin(min_x, MyMin(kNearCornersLightSpace[corner_idx].x, kSplitCornersLightSpace[corner_idx].x));
    min_y = MyMin(min_y, MyMin(kNearCornersLightSpace[corner_idx].y, kSplitCornersLightSpace[corner_idx].y));
    max_x = MyMax(max_x, MyMax(kNearCornersLightSpace[corner_idx].x, kSplitCornersLightSpace[corner_idx].x));
    max_y = MyMax(max_y, MyMax(kNearCornersLightSpace[corner_idx].y, kSplitCornersLightSpace[corner_idx].y));
    max_z = MyMax(max_z, MyMax(kNearCornersLightSpace[corner_idx].z, kSplitCornersLightSpace[corner_idx].z));
  }
  g_shadow_manager.original_scales_[cascade_index].Set(
      1.0f / (max_x - min_x), 1.0f / (max_y - min_y), 1.0f / (max_z - min_z));
  g_shadow_manager.original_offsets_[cascade_index].Set(
      (max_x + min_x), (max_y + min_y), (max_z + min_z));
  g_shadow_manager.scales_[cascade_index] =
      g_shadow_manager.original_scales_[cascade_index] / g_shadow_manager.original_scales_[0];
  float diff_x = (g_shadow_manager.original_offsets_[0].x - g_shadow_manager.original_offsets_[cascade_index].x);
  float diff_y = (g_shadow_manager.original_offsets_[0].y - g_shadow_manager.original_offsets_[cascade_index].y);
  g_shadow_manager.offsets_[cascade_index].Set(
      diff_x * g_shadow_manager.original_scales_[cascade_index].x,
      diff_y * g_shadow_manager.original_scales_[cascade_index].y,
      0);
  MyMemCpy(&kNearCornersLightSpace, &kSplitCornersLightSpace, sizeof(kNearCornersLightSpace));
  OrthoDesc desc;
  desc.left = min_x;
  desc.right = max_x;
  desc.bottom = min_y;
  desc.top_ = max_y;
  desc.near_plane = min_z;
  desc.far_plane = max_z;
  light_camera->MakeProjMatrix(desc,
      light_camera->light_proj_matrices[cascade_index]);
  Viewport viewport;
  int kWidth = int(view_size.x / kNumShadowCascades);
  viewport.top_left_x = kWidth * cascade_index;
  viewport.top_left_y = 0;
  viewport.width = kWidth;
  viewport.height = view_size.y;
  viewport.min_depth = 0;
  viewport.max_depth = 1.0f;
  RC_SetViewport(viewport);
  light_camera->light_view_proj_matrices[cascade_index] =
      light_camera->light_view_matrices[cascade_index] * light_camera->light_proj_matrices[cascade_index];
}

} // namespace dg
