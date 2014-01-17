// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

const int kNumShadowCascades = 4; //#csm #numcascades

class Camera : public Resource {
public:
  struct ProjDesc {
    float fov_y; // Vertical field of view in radian
    float aspect_ratio; // Width / height
    float near_plane;
    float far_plane;
  };
  struct OrthoDesc {
    float left;
    float right;
    float bottom;
    float top_;
    float near_plane;
    float far_plane;

    void Init(float near_plane, float far_plane) {
      this->left = -1.0f;
      this->right = 1.0f;
      this->bottom = -1.0f;
      this->top_ = 1.0f;
      this->near_plane = near_plane;
      this->far_plane = far_plane;
    }
  };
  struct ViewDesc {
    Vector3 eye_pos; // Eye position
    Vector3 target_pos; // Target position
    Vector3 up_dir; // Normalized up direction
  };
  ViewDesc view_desc;
  ProjDesc proj_desc;
  OrthoDesc ortho_desc;
  bool is_orthogonal;
  int cascade_index;
  bool is_light_camera;

  Camera();
  virtual ~Camera();
  const Matrix* proj_matrix() const { return &this->proj_matrix_; }
  const Matrix* view_matrix() const { return &this->view_matrix_; }
  const Matrix* view_proj_matrix() const { return &this->view_proj_matrix_; }
  Vector3& GetEyeReference() { return view_desc.eye_pos; }
  static void UpdateAsLightCamera(Camera* eye_camera, Camera* light_camera, Size2 view_size);
  virtual void UpdateTransform();
  virtual Vector3& GetOrigin();
  virtual void ReadFrom(Stream* stream);
  virtual void WriteTo(Stream* out_stream);
  static void MakeViewMatrix(const ViewDesc& desc, Matrix& out_view_matrix);
  static void MakeProjMatrix(const ProjDesc& desc, Matrix& out_proj_matrix);
  static void MakeProjMatrix(const OrthoDesc& desc, Matrix& out_proj_matrix);
  const Matrix* GetLightViewProjMatrix(int cascade_index) const {
    Check(cascade_index >= 0);
    if (cascade_index >= kNumShadowCascades) return NULL;
    return &light_view_proj_matrices[cascade_index];
  }

protected:
  Matrix light_proj_matrices[kNumShadowCascades];
  Matrix light_view_matrices[kNumShadowCascades];
  Matrix light_view_proj_matrices[kNumShadowCascades];
  Matrix proj_matrix_;
  Matrix view_matrix_;
  Matrix view_proj_matrix_;
  Vector3 origin_;

  // Update projection transform and set dirty-flag to false
  void UpdateProjMatrix();
  void UpdateViewProjMatrix();
  // Update view transform and set dirty-flag to false
  virtual void UpdateViewMatrix();

  DG_DECLARE_RESOURCE(Camera, Resource);
};

} // namespace dg
