// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {
namespace collada_importer {

class Mesh;
class Skin;
class SceneNode;

class Triangles : public NodeBase {
public:
  int max_offset_;
  int num_triangles_;
  Ptr<EngineMesh> engine_mesh_;
  Ptr<EngineMaterial> engine_material_;
  Mesh* mesh_;
  Skin* skin_;
  String material_id_;

  Triangles();
  virtual void ReadFromXml(Element& elem);
  void AddPolygonIndex(int index);
  int GetPolygonIndex(int polygon_index, int vertex_index, int offset);
  int stride();
  int GetVertexIndexWithCullMode(int face_index, int face_vertex_index);
  EngineMaterial* GetCorrespondingEngineMaterial() const;
  void ConvertMesh(EngineMesh& out_engine_mesh);
  void ImportMesh();

private:
  Array<int> polygon_indices_;

  void CreateIndexBuffer(
      IndexBuffer& out_index_buffer,
      int num_triangles);
  void CreateVertexBuffer(VertexBuffer& out_vertex_buffer);
  void LoadTrianglesFromXml(Element& elem);
  void StoreSkinVertices(SkinMesh& out_skin_mesh);
};

} // End of namespace collada_importer
} // End of namespace dg
