// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {
namespace collada_importer {

typedef dg::Material EngineMaterial;

class Image : public NodeBase {
public:
  String file_name_;
  String source_;
  Ptr<Texture> engine_texture_;

  virtual void ReadFromXml(Element& elem);
};

class Material : public NodeBase {
public:
  enum Channel {
      Channel_DiffuseMap = 0,
      Channel_NormalMap = 1,
      Channel_SpecularMap = 2,
      Channel_AmbientOcclusionMap = 3,
      Channel_Num = 4,
  };
  typedef Array<Ptr<Texture> > EngineTextures;

  EngineTextures engine_texture_;
  Ptr<EngineMaterial> engine_material_;
  String image_ids_[Channel_Num];

  Material();
  virtual void ReadFromXml(Element& elem);
  void Import();

private:
  void ConvertMaterial();
  void AddTextureParam(
      EngineMaterial& out_engine_material,
      ShaderEffect* effect,
      Material::Channel channel_index,
      const Cstr* param_name);
};

} // End of namespace collada_importer
} // End of namespace dg
