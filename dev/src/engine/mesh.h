// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class Mesh : public Resource {
public:
  DrawPrimitiveType prim_type_;
  VertexBuffer vertex_buffer_;
  IndexBuffer index_buffer_;

  Mesh();
  virtual ~Mesh();
  void SubmitMesh();
  int num_vertices() const;
  virtual void UpdateTransform() {}
  virtual void ReadFrom(Stream* stream);
  virtual void WriteTo(Stream* out_stream);
  virtual void BindResource() {
    vertex_buffer_.BindResource();
    index_buffer_.BindResource();
  }

protected:
  void SetCurrent();

  DG_DECLARE_RESOURCE(Mesh, Resource);
};

} // namespace dg
