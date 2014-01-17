// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class SphereCamera : public Camera {
public:
  struct SphereViewDesc {
    float yaw;
    float pitch;
    float roll;
    float radius;
    Vector3 target;
  };

  bool is_damper_enabled_;
  SphereViewDesc sphere_view_desc_stiffness_;
  float height_;

  SphereCamera();
  ~SphereCamera();
  void SetViewParam(const SphereViewDesc& desc);
  void GetViewParam(SphereViewDesc& out_desc);
  void AddYaw(float delta);
  void AddPitch(float delta);
  void AddRoll(float delta);
  void AddRadius(float delta);
  virtual void UpdateTransform();
  virtual void ReadFrom(Stream* stream);
  virtual void WriteTo(Stream* out_stream);

private:
  SphereViewDesc sphere_view_desc_;
  SphereViewDesc sphere_view_desc_goal_;

  virtual void UpdateViewMatrix();

  DG_DECLARE_RESOURCE(SphereCamera, Camera);
};

} // namespace dg
