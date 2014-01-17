// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class Mesh;
class Material;

class Part : public Resource {
public:
  Ptr<Mesh> mesh_;
  Ptr<Material> material_;

  Part();
  virtual ~Part();
  virtual void BindResource();
  virtual void UpdateTransform();
  virtual void ReadFrom(Stream* stream);
  virtual void WriteTo(Stream* out_stream);
  SkinMesh* GetSkinMesh() {
    return SafeCast<SkinMesh>(mesh_);
  }

  DG_DECLARE_RESOURCE(Part, Resource);
};

} // namespace dg
