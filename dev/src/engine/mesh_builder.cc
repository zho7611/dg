// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

void MeshBuilder::InitIndexBuffer(IndexBuffer& out_index_buffer, int num_indices) {
  out_index_buffer.InitIndexBuffer(num_indices);
  IndexBuffer::IndexType* data = (IndexBuffer::IndexType*)(out_index_buffer.blob_.data());
  for (int idx = 0; idx < num_indices; ++idx) {
    data[idx] = idx;
  }
}

void MeshBuilder::BuildScreenQuad(Mesh& out_mesh) {
  Check(out_mesh.prim_type_ == DrawPrimitiveType_None);
  out_mesh.prim_type_ = DrawPrimitiveType_TriangleList;
  const int kNumFaces = 2;
  const int kNumVertices = 3*kNumFaces;
  // Init the index buffer
  InitIndexBuffer(out_mesh.index_buffer_, kNumVertices);
  // Fill the vertex buffer
  {
    out_mesh.vertex_buffer_.InitVertexBuffer(
        VertexDecl::Preset_PosTex, kNumVertices);
    VertexBuffer& vbuffer = out_mesh.vertex_buffer_;
    int kVertexStride = vbuffer.stride();
    VertexFormat::FormatPosTex* vbuff =
        (VertexFormat::FormatPosTex*)vbuffer.blob_.data();
    static const float Positions[][2] = {
      //     1)    4)  5)
      // +---+     +---+
      // |  /|     |  /|
      // | / |     | / |
      // |/  |     |/  |
      // +---+     +---+
      // 0)  2)    3)
      {-1, -1}, {1, 1}, {1, -1}, {-1, -1}, {-1, 1}, {1, 1}
    };
    static const float kTexCoords[][2] = {
#if defined(DG_RENDERER_GL2)
      {0, 0}, {1, 1}, {1, 0}, {0, 0}, {0, 1}, {1, 1}
#else // D3D
      {0, 1}, {1, 0}, {1, 1}, {0, 1}, {0, 0}, {1, 0}
#endif
    };
    for (int idx = 0; idx < kNumVertices; ++idx) {
      vbuff->position = Vector3(Positions[idx][0], Positions[idx][1], 0);
      vbuff->tex_coord = Vector2(kTexCoords[idx][0], kTexCoords[idx][1]);
      ++vbuff;
    }
  }
  out_mesh.OnReadDone();
}

// #warning: box center transform is not applied, only size is applied
void MeshBuilder::BuildBox(Mesh& out_mesh, const Vector3& box_size) {
  Check(out_mesh.prim_type_ == DrawPrimitiveType_None);
  out_mesh.prim_type_ = DrawPrimitiveType_TriangleList;
  static const int kNumFaces = 6*2;
  static const int kNumVertices = kNumFaces*3;
  // Initialize index buffer
  InitIndexBuffer(out_mesh.index_buffer_, kNumVertices);
  // Initialize vertex buffer
  {
    const float hx = box_size.x/2.f; // Half-x
    const float hy = box_size.y/2.f; // Half-y
    const float hz = box_size.z/2.f; // Half-z
    // Default coordinate system:
    //   left-handed, right:+x, up:+y, eye-to-screen:+z
    //   cull-mode: CCW (CW triangle construction)
    // Texture coordinate system:
    //   right:+u, down:+v
    // {x, y, z, nx, ny, nz u, v, tanx, tany, tanz, tanw}
    const float v[24][12] = {
      // Front quad (front-left-top, front-right-top, front-left-bottom, front-right-bottom)
      -hx, +hy, -hz, 0.f, 0.f, -1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f,
      +hx, +hy, -hz, 0.f, 0.f, -1.f, 1.f, 0.f, 1.f, 0.f, 0.f, 1.f,
      -hx, -hy, -hz, 0.f, 0.f, -1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f,
      +hx, -hy, -hz, 0.f, 0.f, -1.f, 1.f, 1.f, 1.f, 0.f, 0.f, 1.f,
      // Right quad (front-right-top, back-right-top, front-right-bottom, back-right-bottom)
      +hx, +hy, -hz, 1.0f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f,
      +hx, +hy, +hz, 1.0f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f,
      +hx, -hy, -hz, 1.0f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 1.f,
      +hx, -hy, +hz, 1.0f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f, 1.f,
      // Back quad (back-right-top, back-left-top, back-right-bottom, back-left-bottom)
      +hx, +hy, +hz, 0.f, 0.f, 1.f, 0.f, 0.f, -1.f, 0.f, 0.f, 1.f,
      -hx, +hy, +hz, 0.f, 0.f, 1.f, 1.f, 0.f, -1.f, 0.f, 0.f, 1.f,
      +hx, -hy, +hz, 0.f, 0.f, 1.f, 0.f, 1.f, -1.f, 0.f, 0.f, 1.f,
      -hx, -hy, +hz, 0.f, 0.f, 1.f, 1.f, 1.f, -1.f, 0.f, 0.f, 1.f,
      // Left quad (back-left-top, front-left-top, back-left-bottom, front-left-bottom)
      -hx, +hy, +hz, -1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, -1.f, 1.f,
      -hx, +hy, -hz, -1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, -1.f, 1.f,
      -hx, -hy, +hz, -1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, -1.f, 1.f,
      -hx, -hy, -hz, -1.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, -1.f, 1.f,
      // Top quad (back-top-left, back-top-right, front-top-left, front-top-right)
      -hx, +hy, +hz, 0.f, 1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f,
      +hx, +hy, +hz, 0.f, 1.f, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f, 1.f,
      -hx, +hy, -hz, 0.f, 1.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f,
      +hx, +hy, -hz, 0.f, 1.f, 0.f, 1.f, 1.f, 1.f, 0.f, 0.f, 1.f,
      // Bottom quad (front-botttom-left, front-bottom-right, back-bottom-left, back-bottom-right)
      -hx, -hy, -hz, 0.f, -1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f,
      +hx, -hy, -hz, 0.f, -1.f, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f, 1.f,
      -hx, -hy, +hz, 0.f, -1.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f,
      +hx, -hy, +hz, 0.f, -1.f, 0.f, 1.f, 1.f, 1.f, 0.f, 0.f, 1.f,
    };
    VertexBuffer& vbuffer = out_mesh.vertex_buffer_;
    vbuffer.InitVertexBuffer(VertexDecl::Preset_PosNorTexTan, kNumVertices);
    VertexFormat::FormatPosNorTexTan* vertex =
        (VertexFormat::FormatPosNorTexTan*)vbuffer.blob_.data();
    Matrix rotationMat;
    Check(sizeof(VertexFormat::FormatPosNorTexTan) == sizeof(float)*12);
    for (int quad_idx = 0; quad_idx < 6; ++quad_idx) {
      MyMemCpy(&vertex[quad_idx*6], &v[quad_idx*4], sizeof(VertexFormat::FormatPosNorTexTan));
      MyMemCpy(&vertex[quad_idx*6+1], &v[quad_idx*4+1], sizeof(VertexFormat::FormatPosNorTexTan));
      MyMemCpy(&vertex[quad_idx*6+2], &v[quad_idx*4+2], sizeof(VertexFormat::FormatPosNorTexTan));
      MyMemCpy(&vertex[quad_idx*6+3], &v[quad_idx*4+2], sizeof(VertexFormat::FormatPosNorTexTan));
      MyMemCpy(&vertex[quad_idx*6+4], &v[quad_idx*4+1], sizeof(VertexFormat::FormatPosNorTexTan));
      MyMemCpy(&vertex[quad_idx*6+5], &v[quad_idx*4+3], sizeof(VertexFormat::FormatPosNorTexTan));
    }
  }
  out_mesh.vertex_buffer_.resource_state().SetLoaded(true);
  out_mesh.resource_state().SetLoaded(true);
}

void MeshBuilder::BuildSphere(Mesh& out_mesh, float radius, int num_segments) {
  Check(out_mesh.prim_type_ == DrawPrimitiveType_None);
  out_mesh.prim_type_ = DrawPrimitiveType_TriangleList;
  const int kNumLongitudeSegments = num_segments;
  const int kNumLatitudeSegments = num_segments;
  const int kNumFaces = kNumLongitudeSegments * kNumLatitudeSegments * 2;
  const int kNumVertices = kNumFaces*3;
  const float kLongitudeDelta = kTwoPi / kNumLongitudeSegments;
  const float kLatitudeDelta = kPi / kNumLatitudeSegments;
  const float kRadius = radius;
  // Fill the index buffer
  InitIndexBuffer(out_mesh.index_buffer_, kNumVertices);
  VertexBuffer& vbuffer = out_mesh.vertex_buffer_;
  vbuffer.InitVertexBuffer(VertexDecl::Preset_PosNorTexTan, kNumVertices);
  VertexFormat::FormatPosNorTexTan* vbuff =
      (VertexFormat::FormatPosNorTexTan*)vbuffer.blob_.data();
  const int kNumPositions = (kNumLongitudeSegments+1)*(kNumLatitudeSegments+1);
  Vector3* positions = new Vector3[kNumPositions];
  Vector3* normals = new Vector3[kNumPositions];
  Vector3* tangents = new Vector3[kNumPositions];
  for (int idx_theta = 0; idx_theta <= kNumLatitudeSegments; ++idx_theta) {
    for (int idx_phi = 0; idx_phi <= kNumLongitudeSegments; ++idx_phi) {
      const float kTheta = -kPi + idx_theta * kLatitudeDelta;
      const float kPhi = idx_phi * kLongitudeDelta;
      const int idx = idx_theta * kNumLongitudeSegments + idx_phi;
      positions[idx].x = kRadius * sin(kTheta) * cos(kPhi);
      positions[idx].y = kRadius * sin(kTheta) * sin(kPhi);
      positions[idx].z = kRadius * cos(kTheta);
    }
  }
  for (int idx_theta = 0; idx_theta <= kNumLatitudeSegments; ++idx_theta) {
    for (int idx_phi = 0; idx_phi <= kNumLongitudeSegments; ++idx_phi) {
      const float kTheta = -kPi + idx_theta * kLatitudeDelta;
      const float kPhi = idx_phi * kLongitudeDelta;
      const int idx = idx_theta * kNumLongitudeSegments + idx_phi;
      normals[idx] = positions[idx].CopyNormalized();
    }
  }
  for (int idx_theta = 0; idx_theta <= kNumLatitudeSegments; ++idx_theta) {
    for (int idx_phi = 0; idx_phi <= kNumLongitudeSegments; ++idx_phi) {
      const float kTheta = -kPi + idx_theta * kLatitudeDelta;
      const float kPhi = idx_phi * kLongitudeDelta;
      const int idx = idx_theta * kNumLongitudeSegments + idx_phi;
      int right_idx = idx_theta * kNumLongitudeSegments + idx_phi + 1;
      if (idx_phi == kNumLongitudeSegments) right_idx = idx_theta * kNumLongitudeSegments + 1;
      tangents[idx] = positions[right_idx] - positions[idx];
      tangents[idx].Normalize();
    }
  }
  for (int idx_theta = 0; idx_theta < kNumLatitudeSegments; ++idx_theta) {
    for (int idx_phi = 0; idx_phi < kNumLongitudeSegments; ++idx_phi) {
      const int kIdxLeft = idx_phi;
      const int kIdxBottom = idx_theta + 1;
      const int kIdxTop = idx_theta;
      const int kIdxRight = idx_phi + 1;
      const float kUvs[4] = {
        float(idx_phi) / kNumLongitudeSegments, // Left
        float(idx_phi + 1) / kNumLongitudeSegments, // Right
        float(idx_theta) / kNumLatitudeSegments, // Top
        float(idx_theta + 1) / kNumLatitudeSegments // Bottom
      };
      // Left-bottom
      int pos_idx = kIdxBottom * kNumLongitudeSegments + kIdxLeft;
      Check(pos_idx < kNumPositions);
      vbuff->position = positions[pos_idx];
      vbuff->normal = normals[pos_idx];
      vbuff->tangent = tangents[pos_idx];
      vbuff->tex_coord.Set(kUvs[0], kUvs[3]);
      vbuff++;
      // Left-top
      pos_idx = kIdxTop * kNumLongitudeSegments + kIdxLeft;
      Check(pos_idx < kNumPositions);
      vbuff->position = positions[pos_idx];
      vbuff->normal = normals[pos_idx];
      vbuff->tangent = tangents[pos_idx];
      vbuff->tex_coord.Set(kUvs[0], kUvs[2]);
      vbuff++;
      // Right-bottom
      pos_idx = kIdxBottom * kNumLongitudeSegments + kIdxRight;
      Check(pos_idx < kNumPositions);
      vbuff->position = positions[pos_idx];
      vbuff->normal = normals[pos_idx];
      vbuff->tangent = tangents[pos_idx];
      vbuff->tex_coord.Set(kUvs[1], kUvs[3]);
      vbuff++;
      // Right-bottom
      pos_idx = kIdxBottom * kNumLongitudeSegments + kIdxRight;
      Check(pos_idx < kNumPositions);
      vbuff->position = positions[pos_idx];
      vbuff->normal = normals[pos_idx];
      vbuff->tangent = tangents[pos_idx];
      vbuff->tex_coord.Set(kUvs[1], kUvs[3]);
      vbuff++;
      // Left-top
      pos_idx = kIdxTop * kNumLongitudeSegments + kIdxLeft;
      Check(pos_idx < kNumPositions);
      vbuff->position = positions[pos_idx];
      vbuff->normal = normals[pos_idx];
      vbuff->tangent = tangents[pos_idx];
      vbuff->tex_coord.Set(kUvs[0], kUvs[2]);
      vbuff++;
      // Right-top
      pos_idx = kIdxTop * kNumLongitudeSegments + kIdxRight;
      Check(pos_idx < kNumPositions);
      vbuff->position = positions[pos_idx];
      vbuff->normal = normals[pos_idx];
      vbuff->tangent = tangents[pos_idx];
      vbuff->tex_coord.Set(kUvs[1], kUvs[2]);
      vbuff++;
    }
  }
  DG_SAFE_DELETE_ARRAY(positions);
  DG_SAFE_DELETE_ARRAY(normals);
  DG_SAFE_DELETE_ARRAY(tangents);
  out_mesh.resource_state().SetLoaded(true);
}

void MeshBuilder::Init() {
  // Init for line index/vertex buffer
  InitIndexBuffer(line_ib_, 2);
  line_ib_.BindResource();
  line_vb_.InitVertexBuffer(VertexDecl::Preset_PosCol, 2);
  line_vb_.BindResource();
  // Init for quad index/vertex buffer
  InitIndexBuffer(quad_ib_, 6);
  quad_ib_.BindResource();
  quad_vb_.InitVertexBuffer(VertexDecl::Preset_PosTexCol, 6);
  quad_vb_.BindResource();
}

void MeshBuilder::Clear() {
  quad_ib_.UnbindResource();
  quad_vb_.UnbindResource();
  line_vb_.UnbindResource();
  line_ib_.UnbindResource();
}

void MeshBuilder::DrawQuad(
    const Rect& rect,
    const Rect& tex_coord_rect,
    const Size2& texture_size,
    const ColorInt color) {
  // Vertex shader has to transform the screen coordinates to the normalized device coordinates
  const int kPositions[6][2] = {
    { rect.left, rect.top },
    { rect.right, rect.top },
    { rect.right, rect.bottom },
    { rect.left, rect.top },
    { rect.right, rect.bottom },
    { rect.left, rect.bottom }
  };
  const float kTexCoords[6][2] = {
    {float(tex_coord_rect.left)/texture_size.x, float(tex_coord_rect.top)/texture_size.y}, // [0,0]
    {float(tex_coord_rect.right)/texture_size.x, float(tex_coord_rect.top)/texture_size.y}, // [1,0]
    {float(tex_coord_rect.right)/texture_size.x, float(tex_coord_rect.bottom)/texture_size.y}, // [1,1]
    {float(tex_coord_rect.left)/texture_size.x, float(tex_coord_rect.top)/texture_size.y}, // [0,0]
    {float(tex_coord_rect.right)/texture_size.x, float(tex_coord_rect.bottom)/texture_size.y}, // [1,1]
    {float(tex_coord_rect.left)/texture_size.x, float(tex_coord_rect.bottom)/texture_size.y}, // [0,1]
  };
  {
    VertexFormat::FormatPosTexCol* vb = NULL;
    VertexBuffer::ScopedLock locker(g_renderer.mesh_builder.quad_vb_, vb);
    for (int idx = 0; idx < 6; ++idx) {
      vb[idx].position = Vector3(float(kPositions[idx][0]), float(kPositions[idx][1]), 0);
      vb[idx].tex_coord = Vector2(kTexCoords[idx][0], kTexCoords[idx][1]);
      vb[idx].color = color;
    }
  }
  g_renderer.mesh_builder.quad_ib_.SubmitIndexBuffer();
  g_renderer.mesh_builder.quad_vb_.SubmitVertexBuffer();
  RC_DrawIndexedPrimitive(DrawPrimitiveType_TriangleList, 2);
}

void MeshBuilder::DrawLine(
    const Vector3& start,
    const Vector3& end,
    ColorInt color) {
  {
    VertexFormat::FormatPosCol* vb = NULL;
    VertexBuffer::ScopedLock locker(g_renderer.mesh_builder.line_vb_, vb);
    vb[0].position = start;
    vb[0].color = color;
    vb[1].position = end;
    vb[1].color = color;
  }
  g_renderer.mesh_builder.line_ib_.SubmitIndexBuffer();
  g_renderer.mesh_builder.line_vb_.SubmitVertexBuffer();
  RC_DrawIndexedPrimitive(DrawPrimitiveType_LineList, 1);
}

} // namespace dg
