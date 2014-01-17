// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

SphereCamera::SphereCamera() :
  Camera(),
  is_damper_enabled_(true),
  height_(0) {
  sphere_view_desc_.yaw = 0;
  sphere_view_desc_.pitch = 0;
  sphere_view_desc_.roll = 0;
  sphere_view_desc_.radius = 5.0f;
  sphere_view_desc_.target.Set(0, 0, 0);
  sphere_view_desc_goal_ = sphere_view_desc_;
  sphere_view_desc_stiffness_.yaw = 0.1f;
  sphere_view_desc_stiffness_.pitch = 0.1f;
  sphere_view_desc_stiffness_.roll = 0.1f;
  sphere_view_desc_stiffness_.radius = 0.1f;
  sphere_view_desc_stiffness_.target.Set(0, 0, 0);
}

SphereCamera::~SphereCamera() {
}

void SphereCamera::SetViewParam(const SphereViewDesc& desc) {
  sphere_view_desc_ = sphere_view_desc_goal_ = desc;
}

void SphereCamera::GetViewParam(SphereCamera::SphereViewDesc& out_desc) {
  out_desc = sphere_view_desc_goal_;
}

void SphereCamera::UpdateTransform() {
  if (is_damper_enabled_) {
    float delta_time = g_renderer.timer.GetDeltaInSeconds();
    ImplicitEuler(
        sphere_view_desc_.yaw,
        sphere_view_desc_goal_.yaw,
        delta_time, sphere_view_desc_stiffness_.yaw, 0.001f);
    ImplicitEuler(
        sphere_view_desc_.pitch,
        sphere_view_desc_goal_.pitch,
        delta_time, sphere_view_desc_stiffness_.pitch, 0.001f);
    ImplicitEuler(
        sphere_view_desc_.roll,
        sphere_view_desc_goal_.roll,
        delta_time, sphere_view_desc_stiffness_.roll, 0.001f);
    ImplicitEuler(
        sphere_view_desc_.radius,
        sphere_view_desc_goal_.radius,
        delta_time, sphere_view_desc_stiffness_.radius, 0.001f);
  } else {
    sphere_view_desc_.yaw = sphere_view_desc_goal_.yaw;
    sphere_view_desc_.pitch = sphere_view_desc_goal_.pitch;
    sphere_view_desc_.roll = sphere_view_desc_goal_.roll;
    sphere_view_desc_.radius = sphere_view_desc_goal_.radius;
  }
  Camera::UpdateTransform();
}

void SphereCamera::UpdateViewMatrix() {
  Matrix world_to_camera;
  // Get the world_to_camera matrix without yz-swap
  {
    Transform tm;
    tm.rotation_.StoreRotationYawPitchRoll(
        sphere_view_desc_.yaw,
        sphere_view_desc_.pitch,
        sphere_view_desc_.roll);
    tm.LocalTranslate(Vector3(0, height_, -sphere_view_desc_.radius));
    tm.Translate(sphere_view_desc_.target);
    tm.ToMatrix(world_to_camera);
  }
  // Yz-swap if needed
  const bool is_yz_swapped = true;
  if (is_yz_swapped) {
    Transform yz_swap;
    // To make the x axis directs to the left, y axis to screen, and z axis to up
    // Left-handed coordinate system
    yz_swap.rotation_.StoreRotationYawPitchRoll(
        DegreeToRadian(-180.0f),
        DegreeToRadian(-90.0f),
        DegreeToRadian(0));
    Matrix yz_swap_matrix;
    yz_swap.ToMatrix(yz_swap_matrix);
    world_to_camera.StoreMultiply(world_to_camera, yz_swap_matrix);
  }
  // Set the camera origin from the translation of the world-to-camera
  origin_.Set(world_to_camera.m41_, world_to_camera.m42_, world_to_camera.m43_);
  view_matrix_.StoreInverse(world_to_camera);
}

void SphereCamera::AddYaw(float delta) {
  sphere_view_desc_goal_.yaw += delta;
}

void SphereCamera::AddPitch(float delta) {
  sphere_view_desc_goal_.pitch += delta;
}

void SphereCamera::AddRoll(float delta) {
  sphere_view_desc_goal_.roll += delta;
}

void SphereCamera::AddRadius(float delta) {
  sphere_view_desc_goal_.radius += delta;
}

void SphereCamera::ReadFrom(Stream* stream) {
  Camera::ReadFrom(stream);
  StreamReadFlat(stream, is_damper_enabled_);
  StreamReadFlat(stream, sphere_view_desc_stiffness_);
  StreamReadFlat(stream, height_);
  StreamReadFlat(stream, sphere_view_desc_);
  StreamReadFlat(stream, sphere_view_desc_goal_);
  OnReadDone();
}

void SphereCamera::WriteTo(Stream* out_stream) {
  Camera::WriteTo(out_stream);
  StreamWriteFlat(out_stream, is_damper_enabled_);
  StreamWriteFlat(out_stream, sphere_view_desc_stiffness_);
  StreamWriteFlat(out_stream, height_);
  StreamWriteFlat(out_stream, sphere_view_desc_);
  StreamWriteFlat(out_stream, sphere_view_desc_goal_);
  OnWriteDone();
}

} // namespace dg
