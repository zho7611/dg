// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class Mesh;
class Box;
class Sphere;

class MeshBuilder {
public:
  static void InitIndexBuffer(IndexBuffer& out_index_buffer, int num_indices);
  static void BuildScreenQuad(Mesh& out_mesh);
  static void BuildBox(Mesh& out_mesh, const Vector3& box_size);
  static void BuildSphere(Mesh& out_mesh, float radius = 1.0f, int num_segments = 8);
  static void DrawQuad(
      const Rect& rect,
      const Rect& tex_coord_rect,
      const Size2& texture_size,
      ColorInt color);
  static void DrawLine(
      const Vector3& start,
      const Vector3& end,
      ColorInt color);
  void Init();
  void Clear();

private:
  VertexBuffer quad_vb_;
  IndexBuffer quad_ib_;
  VertexBuffer line_vb_;
  IndexBuffer line_ib_;
};

} // namespace dg
