// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "collada_importer_first.h"

using namespace dg;
using namespace dg::collada_importer;

VertexSources::VertexSources(collada_importer::Mesh* mesh) {
  Check(mesh);
  positions_ = mesh->sources_.Get(Source::Format_Position);
  tex_coords_ = mesh->sources_.Get(Source::Format_TexCoord);
  normals_ = mesh->sources_.Get(Source::Format_Normal);
  tangents_ = mesh->sources_.Get(Source::Format_TexTangent);
  binormals_ = mesh->sources_.Get(Source::Format_TexBinormal);
}

void collada_importer::SetEngineVertex(
    EngineVertex& out_engine_vertex,
    collada_importer::Triangles& triangles,
    VertexSources& sources,
    int face_index,
    int vertex_index) {
  int positionIdx = triangles.GetPolygonIndex(face_index, vertex_index, sources.positions_->index_);
  int texcoordIdx = triangles.GetPolygonIndex(face_index, vertex_index, sources.tex_coords_->index_);
  int normalIdx = triangles.GetPolygonIndex(face_index, vertex_index, sources.normals_->index_);
  int tangentIdx = triangles.GetPolygonIndex(face_index, vertex_index, sources.tangents_->index_);
  sources.positions_->GetVector(positionIdx, out_engine_vertex.position);
  sources.tex_coords_->GetVector2(texcoordIdx, out_engine_vertex.tex_coord);
  sources.normals_->GetVector(normalIdx, out_engine_vertex.normal);
  Vector3 tangent;
  sources.tangents_->GetVector(tangentIdx, tangent);
  out_engine_vertex.tangent.Set(tangent, 1.0f);
}
