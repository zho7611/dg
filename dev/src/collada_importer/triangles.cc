// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "collada_importer_first.h"

using namespace dg;
using namespace dg::collada_importer;

collada_importer::Triangles::Triangles() :
  num_triangles_(0),
  max_offset_(0),
  engine_mesh_(0),
  engine_material_(0),
  mesh_(0) {
}

void collada_importer::Triangles::AddPolygonIndex(int index) {
  polygon_indices_.PushBack(index);
}

int collada_importer::Triangles::stride() {
  return (max_offset_+1)*3;
}

int collada_importer::Triangles::GetPolygonIndex(
    int polygon_index, int vertex_index, int offset) {
  const int kStride = stride();
  const int kNumOffsets = max_offset_+1;
  return polygon_indices_.Get(
      polygon_index*kStride + vertex_index*kNumOffsets + offset);
}

void collada_importer::Triangles::ReadFromXml(Element& elem) {
  String count_str, material_id;
  elem.GetAttribute(TXT("count"), count_str);
  num_triangles_ = count_str.ToInt();
  elem.GetAttribute(TXT("material"), material_id_);
  int max_offset = 0;
  DG_FOR_EACH_ELEMENT(elem, sub_elem) {
    if (sub_elem.HasTagName(TXT("input"))) {
      String cur_source;
      sub_elem.GetAttribute(TXT("source"), cur_source);
      TrimReferenceMark(cur_source);
      Ptr<Source> mesh_source;
      if (sub_elem.HasAttribute(TXT("semantic"), TXT("VERTEX"))) {
        mesh_source = mesh_->vertices_source_;
      } else {
        mesh_source = mesh_->source_by_id_.Get(cur_source.Get());
      }
      String offset_str;
      sub_elem.GetAttribute(TXT("offset"), offset_str);
      int offset = offset_str.ToInt();
      max_offset = (offset > max_offset) ? offset : max_offset;
      mesh_source->index_ = offset;
      if (sub_elem.HasAttribute(TXT("semantic"), TXT("VERTEX"))) {
        mesh_->sources_.Get(Source::Format_Position) = mesh_source;
      } else if ((sub_elem.HasAttribute(TXT("semantic"), TXT("NORMAL")))) {
        mesh_->sources_.Get(Source::Format_Normal) = mesh_source;
      } else if ((sub_elem.HasAttribute(TXT("semantic"), TXT("TEXCOORD")))) {
        mesh_->sources_.Get(Source::Format_TexCoord) = mesh_source;
      } else if ((sub_elem.HasAttribute(TXT("semantic"), TXT("TEXTANGENT")))) {
        mesh_->sources_.Get(Source::Format_TexTangent) = mesh_source;
      } else if ((sub_elem.HasAttribute(TXT("semantic"), TXT("TEXBINORMAL")))) {
        mesh_->sources_.Get(Source::Format_TexBinormal) = mesh_source;
      }
    } else if ((sub_elem.HasTagName(TXT("p")))) {
      String sub_str;
      sub_elem.GetValue(sub_str);
      StringStream poly_stream(sub_str.Get());
      max_offset_ = max_offset;
      const int num_indices = this->stride()*num_triangles_;
      int idx;
      for (int idx_count = 0; idx_count < num_indices; ++idx_count) {
        if (poly_stream.ReadInt(idx)) {
          this->AddPolygonIndex(idx);
        } else {
          DG_UNREACHABLE();
        }
      }
    }
  }
}

// Calculate the tangent vector and the binormal vector from the normal vector and UVs
// Reference: http://www.terathon.com/code/tangent.html
static void CalcTangentBinormal(EngineVertex* vertices, int num_vertices) {
  const int kNumFaces = num_vertices/3;
  Vector3* binormals = new Vector3[num_vertices];
  for (int face_idx = 0; face_idx < kNumFaces; ++face_idx) {
    EngineVertex& v0 = vertices[face_idx*3+0];
    EngineVertex& v1 = vertices[face_idx*3+1];
    EngineVertex& v2 = vertices[face_idx*3+2];
    Vector3& binormal0 = binormals[face_idx*3+0];
    Vector3& binormal1 = binormals[face_idx*3+1];
    Vector3& binormal2 = binormals[face_idx*3+2];
    float x1 = v1.position.x - v0.position.x;
    float x2 = v2.position.x - v0.position.x;
    float y1 = v1.position.y - v0.position.y;
    float y2 = v2.position.y - v0.position.y;
    float z1 = v1.position.z - v0.position.z;
    float z2 = v2.position.z - v0.position.z;
    float s1 = v1.tex_coord.x - v0.tex_coord.x;
    float s2 = v2.tex_coord.x - v0.tex_coord.x;
    float t1 = v1.tex_coord.y - v0.tex_coord.y;
    float t2 = v2.tex_coord.y - v0.tex_coord.y;
    float r = 1.0F / (s1 * t2 - s2 * t1);
    Vector3 sdir(
        (t2 * x1 - t1 * x2) * r,
        (t2 * y1 - t1 * y2) * r,
        (t2 * z1 - t1 * z2) * r);
    Vector3 tdir(
        (s1 * x2 - s2 * x1) * r,
        (s1 * y2 - s2 * y1) * r,
        (s1 * z2 - s2 * z1) * r);
    v0.tangent = sdir;
    v1.tangent = sdir;
    v2.tangent = sdir;
    binormal0 = tdir;
    binormal1 = tdir;
    binormal2 = tdir;
  }
  for (int idx = 0; idx < num_vertices; idx++) {
    Vector3 n = vertices[idx].normal.Normalize();
    Vector3 t;
    t.Set(
        vertices[idx].tangent.x,
        vertices[idx].tangent.y,
        vertices[idx].tangent.z);
    // Gram-Schmidt orthogonalize
    Vector4 tangent = (t - n * Dot(n, t)).Normalize();
    // Calculate handedness
    tangent.w = (Dot(Cross(n, t), binormals[idx]) < 0.0f) ? -1.0f : 1.0f;
    // Save the tangent
    vertices[idx].tangent = tangent;
  }
  delete [] binormals;
}

static void InvertTexCoordV(EngineVertex* vertices, int num_vertices) {
  const int kNumFaces = num_vertices/3;
  for (int idx = 0; idx < num_vertices; idx++) {
    vertices[idx].tex_coord.y = 1.0f - vertices[idx].tex_coord.y;
  }
}

void collada_importer::Triangles::CreateVertexBuffer(VertexBuffer& out_vertex_buffer) {
  const int kNumVertices = num_triangles_*3;
#if !defined(DG_USE_32BIT_INDEX)
  const int kMaxVertexCount = 65535;
  Check(kNumVertices < kMaxVertexCount);
  if (kNumVertices >= kMaxVertexCount) {
    DG_LOG_LINE(TXT("collada_importer.triangles.createvertexbuffer.exceededmaxvertexcount: vertexcount:%d maxvertexcount:%d"),
        kNumVertices,
        kMaxVertexCount);
    return;
  }
#endif
  out_vertex_buffer.InitVertexBuffer(
      VertexDecl::Preset_PosNorTexTan,
      kNumVertices,
      GpuUsage_None);
  out_vertex_buffer.BindResource();
  {
    EngineVertex* vertices = NULL;
    VertexBuffer::ScopedLock locker(
        out_vertex_buffer,
        vertices);
    Check(vertices);
    VertexSources sources(mesh_);
    EngineVertex source_vertices[3];
    for (int face_idx = 0; face_idx < num_triangles_; ++face_idx) {
      SetEngineVertex(source_vertices[0], *this, sources, face_idx, 0);
      SetEngineVertex(source_vertices[1], *this, sources, face_idx, 1);
      SetEngineVertex(source_vertices[2], *this, sources, face_idx, 2);
      for (int face_vertex_idx = 0; face_vertex_idx < 3; ++face_vertex_idx) {
        EngineVertex& vertex = source_vertices[face_vertex_idx];
        int vertex_idx = GetVertexIndexWithCullMode(face_idx, face_vertex_idx);
        // Copy the vertex
        vertices[vertex_idx] = vertex;
      }
    }
    CalcTangentBinormal(vertices, kNumVertices);
    // Inverting texcoord must be after the CalcTangentBinormal() call
    InvertTexCoordV(vertices, kNumVertices);
  }
  out_vertex_buffer.SerializeFromDevice();
  out_vertex_buffer.UnbindResource();
}

void collada_importer::Triangles::CreateIndexBuffer(
    IndexBuffer& out_index_buffer,
    int num_triangles) {
  out_index_buffer.InitIndexBuffer(num_triangles*3, GpuUsage_None);
  out_index_buffer.BindResource();
  {
    IndexBuffer::IndexType* ibuff = (IndexBuffer::IndexType*)out_index_buffer.blob().data();
    for (int idx = 0; idx < 3*num_triangles; ++idx) {
      ibuff[idx] = idx;
    }
  }
  out_index_buffer.UnbindResource();
}

int collada_importer::Triangles::GetVertexIndexWithCullMode(int face_index, int face_vertex_index) {
  if (collada_importer::g_collada_importer.is_reverse_cull_) {
    return (face_index*3 + (2-face_vertex_index));
  }
  return face_index*3 + face_vertex_index;
}

void collada_importer::Triangles::StoreSkinVertices(SkinMesh& out_skin_mesh) {
  static const int NumInfluences = VertexFormat::kMaxJointInfluence;
  Ptr<Skin> skin = mesh_->skin_;
  Check(skin);
  Check(skin->skin_vertices_.Count() == skin->num_vertices_);
  out_skin_mesh.skin_vertices_.Resize(out_skin_mesh.num_vertices());
  Ptr<Source> position_source(mesh_->sources_.Get(Source::Format_Position));
  for (int face_idx = 0; face_idx < num_triangles_; ++face_idx) {
    for (int face_vertex_idx = 0; face_vertex_idx < 3; ++face_vertex_idx) {
      const int kSourceIdx = GetPolygonIndex(face_idx, face_vertex_idx,
              position_source->index_);
      const int kNumJoints = skin->num_joint_per_vertex_.Get(kSourceIdx);
      Check(kNumJoints <= VertexFormat::kMaxJointInfluence);
      const int target_idx = GetVertexIndexWithCullMode(face_idx, face_vertex_idx);
      for (int influence_idx = 0; influence_idx < NumInfluences; ++influence_idx) {
        int joint_index = -1;
        float joint_weight = 0;
        Skin::SkinVertex& source_skin_vertex = skin->skin_vertices_.Get(kSourceIdx);
        EngineSkinMesh::SkinVertex& target_skin_vertex = out_skin_mesh.skin_vertices_.Get(target_idx);
        if (influence_idx < kNumJoints) {
          joint_index = source_skin_vertex.joint_indices_.Get(influence_idx);
          joint_weight = source_skin_vertex.joint_weights_.Get(influence_idx);
        }
        target_skin_vertex.joint_indices[influence_idx] = joint_index;
        target_skin_vertex.joint_weights[influence_idx] = joint_weight;
      }
    }
  }
}

void collada_importer::Triangles::ConvertMesh(EngineMesh& out_engine_mesh) {
  String mesh_name(mesh_->geometry_id_.Get());
  if (mesh_->triangles_list_.Count() > 1) {
    // Multi/Sub material support
    for (int idx = 0; idx < mesh_->triangles_list_.Count(); ++idx) {
      if (mesh_->triangles_list_.Get(idx).ptr() == this) {
        String index_str;
        index_str.FromInt(idx);
        mesh_name.AddRight(index_str);
      }
    }
  }
  mesh_name.AddRight(TXT(".mesh"));
  ResourcePath resource_path(collada_importer::g_collada_importer.package_->resource_name(), mesh_name.Get());
  out_engine_mesh.set_resource_name(resource_path.Get());
  out_engine_mesh.prim_type_ = DrawPrimitiveType_TriangleList;
  CreateIndexBuffer(out_engine_mesh.index_buffer_, num_triangles_);
  CreateVertexBuffer(out_engine_mesh.vertex_buffer_);
}

void collada_importer::Triangles::ImportMesh() {
  Ptr<Skin> skin = mesh_->skin_;
  engine_mesh_ = (skin) ? new EngineSkinMesh : new EngineMesh;
  ConvertMesh(*engine_mesh_);
  if (skin) {
    SkinMesh* skin_mesh = SafeCast<SkinMesh>(engine_mesh_);
    StoreSkinVertices(*skin_mesh);
    skin->StoreSkinMesh(*skin_mesh);
    skin_mesh->AttachSkeleton(collada_importer::g_collada_importer.engine_skeleton_);
  }
  Ptr<Chunk> mesh_chunk =
      collada_importer::g_collada_importer.GetPackageStreamWriterFromPath(
      engine_mesh_->resource_name());
  engine_mesh_->WriteTo(mesh_chunk.ptr());
}

EngineMaterial* collada_importer::Triangles::GetCorrespondingEngineMaterial() const {
  collada_importer::Material* collada_material = collada_importer::g_collada_importer.FindMaterial(
      material_id_.GetConst());
  Check(collada_material);
  return collada_material->engine_material_.ptr();
}
