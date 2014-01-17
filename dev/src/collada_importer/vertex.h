// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {
namespace collada_importer {

class VertexSources {
public:
  Ptr<Source> positions_;
  Ptr<Source> tex_coords_;
  Ptr<Source> normals_;
  Ptr<Source> tangents_;
  Ptr<Source> binormals_;

  VertexSources(collada_importer::Mesh* mesh);
};

typedef VertexFormat::FormatPosNorTexTan EngineVertex;

void SetEngineVertex(EngineVertex& out_engine_vertex,
    Triangles& triangles, VertexSources& sources, int face_index, int vertex_index);

} // End of namespace collada_importer
} // End of namespace dg
