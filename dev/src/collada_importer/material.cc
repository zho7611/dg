// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "collada_importer_first.h"

using namespace dg;
using namespace dg::collada_importer;

void Image::ReadFromXml(Element& elem) {
  String image_id;
  elem.GetAttribute(TXT("id"), image_id);
  String uri_path;
  elem.GetSubElement(TXT("init_from")).GetValue(uri_path);
  String systemPath;
  g_collada_importer.ConvertURIPath(uri_path, systemPath);
  FilePath imagePath(systemPath.Get());
  if (!imagePath.IsAbsolute()) {
    // Make absolute path based on the export file path
    imagePath.AddLeft(g_collada_importer.GetFilePath());
  }
  source_ = imagePath.GetPath();
  file_name_ = imagePath.GetFileName();
  g_collada_importer.AddImage(image_id, this);
}

collada_importer::Material::Material() : engine_material_(NULL) {
}

void collada_importer::Material::ReadFromXml(Element& elem) {
  String material_id;
  elem.GetAttribute(TXT("id"), material_id);
  Element instanceEffect;
  id_= material_id;
  if (!elem.GetSubElement(TXT("instance_effect"), instanceEffect)) {
    return;
  }
  DG_FOR_EACH_ELEMENT(instanceEffect, param) {
    String image_id;
    Element surface;
    String refStr;
    param.GetAttribute(TXT("ref"), refStr);
    if (refStr == TXT("GlobalDiffuseTexture")) {
      if (param.GetSubElement(TXT("surface"), surface)) {
        Element init_from;
        if (surface.GetSubElement(TXT("init_from"), init_from)) {
          init_from.GetValue(image_id);
          image_ids_[Channel_DiffuseMap] = image_id;
        }
      }
    } else if ((refStr == TXT("GlobalNormalTexture"))) {
      if (param.GetSubElement(TXT("surface"), surface)) {
        Element init_from;
        if (surface.GetSubElement(TXT("init_from"), init_from)) {
          init_from.GetValue(image_id);
          image_ids_[Channel_NormalMap] = image_id;
        }
      }
    } else if ((refStr == TXT("GlobalSpecularTexture"))) {
      if (param.GetSubElement(TXT("surface"), surface)) {
        Element init_from;
        if (surface.GetSubElement(TXT("init_from"), init_from)) {
          init_from.GetValue(image_id);
          image_ids_[Channel_SpecularMap] = image_id;
        }
      }
    } else if ((refStr == TXT("GlobalAmbientOcclusionTexture"))) {
      if (param.GetSubElement(TXT("surface"), surface)) {
        Element init_from;
        if (surface.GetSubElement(TXT("init_from"), init_from)) {
          init_from.GetValue(image_id);
          image_ids_[Channel_AmbientOcclusionMap] = image_id;
        }
      }
    }
  }
  collada_importer::g_collada_importer.AddMaterial(material_id, this);
}

void collada_importer::Material::ConvertMaterial() {
  Ptr<ShaderEffect> effect(LoadResource<SimpleShaderEffect>(g_engine_config->default_effect_path_.Get()));
  // Change to pack path
  {
    String material_name(id_);
    material_name.AddRight(TXT(".mat"));
    ResourcePath referencePath(
        collada_importer::g_collada_importer.package_->resource_name(),
        material_name.Get());
    engine_material_ = LoadResource<EngineMaterial>(referencePath.Get(), LoadResourceOption_Write);
  }
  engine_material_->effect_ = effect;
  // Set default material property
  material_util::SetMaterialDiffuse(engine_material_.ptr(), 1.0f, 1.0f, 1.0f);
  material_util::SetMaterialAmbient(engine_material_.ptr(), .3f, .3f, .3f);
  // Rgb: intensity, a: power
  material_util::SetMaterialSpecular(engine_material_.ptr(), 1.0f, 1.0f, 1.0f, 32.0f);
  // Set ambient occlusion texture
  AddTextureParam(
      *engine_material_, effect.ptr(),
      collada_importer::Material::Channel_AmbientOcclusionMap,
      TXT("AmbientOcclusionTextureSampler"));
  // Set normal texture
  AddTextureParam(
      *engine_material_, effect.ptr(),
      collada_importer::Material::Channel_NormalMap,
      TXT("NormalTextureSampler"));
  // Set specular texture
  AddTextureParam(
      *engine_material_, effect.ptr(),
      collada_importer::Material::Channel_SpecularMap,
      TXT("SpecularTextureSampler"));
  // Set diffuse texture
  AddTextureParam(
      *engine_material_, effect.ptr(),
      collada_importer::Material::Channel_DiffuseMap,
      TXT("DiffuseTextureSampler"));
  collada_importer::g_collada_importer.RenameTextures();
}

void collada_importer::Material::AddTextureParam(
    EngineMaterial& out_engine_material,
    ShaderEffect* /* effect */,
    collada_importer::Material::Channel channel_index,
    const Cstr* param_name) {
  // Check if the dg material uses proper effect
  if (!collada_importer::g_collada_importer.images_.IsFound(image_ids_[channel_index].Get())) {
    DG_LOG_LINE(TXT("warning: collada_importer.material.addtextureparam: no-channel-image:%d"), channel_index);
    return;
  }
  Image* image = collada_importer::g_collada_importer.images_.Get(image_ids_[channel_index].Get());
  Check(image);
  Ptr<Texture> texture = image->engine_texture_;
  Check(texture);
  {
    ShaderParameter* param = out_engine_material.AddParameter(
        param_name,
        ShaderParameter::kTextureType);
    param->SetTexture(texture.ptr());
    engine_texture_.PushBack(texture);
  }
}

void collada_importer::Material::Import() {
  if (!collada_importer::g_collada_importer.IsActiveMaterial(this)) {
    return;
  }
  ConvertMaterial();
  for (int idx = 0; idx < engine_texture_.Count(); ++idx) {
    Ptr<Texture>& texture = engine_texture_.Get(idx);
    Ptr<Chunk> chunk =
        collada_importer::g_collada_importer.GetPackageStreamWriterFromPath(
        texture->resource_name());
    texture->BindResource();
    if (!texture->HasBlob()) {
      texture->SerializeFromDevice();
    }
    texture->WriteTo(chunk.ptr());
  }
  Ptr<Chunk> mat_chunk =
      collada_importer::g_collada_importer.GetPackageStreamWriterFromPath(
      engine_material_->resource_name());
  engine_material_->WriteTo(mat_chunk.ptr());
}
