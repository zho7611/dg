// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {
namespace collada_importer {

typedef dg::Mesh EngineMesh;
typedef dg::SkinMesh EngineSkinMesh;

class Triangles;
class Skin;

class Mesh : public NodeBase {
public:
  String geometry_id_;
  EnumArray<Source::Format, Ptr<Source> > sources_;
  StringMap<Ptr<Source> > source_by_id_;
  Ptr<Source> vertices_source_;
  Array<Ptr<Triangles> > triangles_list_;
  Ptr<Skin> skin_;

  Mesh();
  ~Mesh();
  virtual void ReadFromXml(Element& elem);
  Source* GetSource(Source::Format& format);
  void SetSource(Source::Format& format, Source* source);

private:
  void LoadVerticesFromXml(Element& elem);
};

} // End of namespace collada_importer
} // End of namespace dg
