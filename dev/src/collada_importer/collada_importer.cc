// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "collada_importer_first.h"

using namespace dg;
using namespace collada_importer;

ColladaImporter collada_importer::g_collada_importer;

ColladaImporter::ColladaImporter() :
  skeleton_(0),
  engine_skeleton_(0),
  is_reverse_cull_(false) {
}

ColladaImporter::~ColladaImporter() {
  Check(sources_.IsEmpty());
  Check(meshes_.IsEmpty());
  Check(images_.IsEmpty());
  Check(materials_.IsEmpty());
  Check(scene_.nodes_.IsEmpty());
  Check(animations_.IsEmpty());
  Check(!package_);
}

void ColladaImporter::Close() {
  package_ = 0;
  DeleteAll(sources_);
  sources_.Clear();
  DeleteAll(animations_);
  animations_.Clear();
  DeleteAll(meshes_);
  meshes_.Clear();
  DeleteAll(images_);
  images_.Clear();
  DeleteAll(materials_);
  materials_.Clear();
  ReleaseAll(scene_.nodes_);
  scene_.nodes_.Clear();
  document_.Close();
}

collada_importer::Mesh* ColladaImporter::FindMesh(const String& geometry_id) {
  for (int idx = 0; idx < meshes_.Count(); ++idx) {
    if (meshes_.Get(idx)->geometry_id_.IsEqual(geometry_id)) {
      return meshes_.Get(idx);
    }
  }
  return NULL;
}

collada_importer::Mesh& ColladaImporter::GetCurrentMesh() {
  Check(!meshes_.IsEmpty());
  return *meshes_.Last();
}

bool ColladaImporter::Open(const Cstr* path) {
  path_.SetPath(path);
  if (!document_.Open(path)) {
    return false;
  }
  Parse();
  return true;
}

void ColladaImporter::Parse() {
  Element elem = document_.GetDocumentElement();
  DG_FOR_EACH_ELEMENT(elem, sub_elem) {
    if (sub_elem.HasTagName(TXT("library_images"))) {
      DG_FOR_EACH_ELEMENT(sub_elem, SubSubElem) {
        Image* image = new Image;
        image->ReadFromXml(SubSubElem);
      }
    } else if (sub_elem.HasTagName(TXT("library_materials"))) {
      DG_FOR_EACH_ELEMENT(sub_elem, SubSubElem) {
        Material* material = new Material;
        material->ReadFromXml(SubSubElem);
      }
    } else if (sub_elem.HasTagName(TXT("library_effects"))) {
      DG_FOR_EACH_ELEMENT(sub_elem, SubSubElem) {
        //DG_UNIMPLEMENTED();
      }
    } else if (sub_elem.HasTagName(TXT("library_geometries"))) {
      DG_FOR_EACH_ELEMENT(sub_elem, SubSubElem) {
        ReadGeometry(SubSubElem);
      }
    } else if (sub_elem.HasTagName(TXT("library_controllers"))) {
      DG_FOR_EACH_ELEMENT(sub_elem, SubSubElem) {
        ReadControllers(SubSubElem);
      }
    } else if (sub_elem.HasTagName(TXT("library_visual_scenes"))) {
      DG_FOR_EACH_ELEMENT(sub_elem, SubSubElem) {
        ReadVisualScene(SubSubElem);
      }
    } else if (sub_elem.HasTagName(TXT("library_animations"))) {
      DG_FOR_EACH_ELEMENT(sub_elem, SubSubElem) {
        Animation* anim = new Animation;
        anim->ReadFromXml(SubSubElem);
        animations_.PushBack(anim);
      }
    }
  }
}

Cstr ColladaImporter::ConvertHexCharToInt(Cstr ch) {
  return (::isalpha(ch)) ? (ch - 'a' + 10) : (ch - '0');
}

void ColladaImporter::ConvertURIPath(
    const String& uri_path,
    String& out_system_path
) const {
  // Example
  // File:///C:/%bb%f5%20%c6%fa%b4%f5/1%c8%a3%b5%e5%b9%cc%b8%de%bd%ba/viconeater/viconeater_B.dds
  const int MaxPathStringSize = 255;
  Cstr buff[MaxPathStringSize];
  Cstr* target = buff;
  const Cstr* source = uri_path.GetConst();
  // If the source starts with the file tag, skip the filetag part
  {
    const Cstr FileTag[] = TXT("file:///");
    int fileTagLen = sizeof(FileTag) - 1;
    if (MyStrnCmp(source, FileTag, fileTagLen) == 0) {
      source += fileTagLen;
    }
  }
  // For all source characters, convert the % tags into bin
  Cstr ch;
  while (ch = *source++) {
    if (ch == '%') {
      const Cstr HighBits = ConvertHexCharToInt(*source++);
      const Cstr LowBits = ConvertHexCharToInt(*source++);
      ch = (HighBits << 4) + LowBits;
    }
    *target++ = ch;
    Check((target - buff) < MaxPathStringSize);
  }
  // Make it end with null-terminator
  *target++ = TXT('\0');
  out_system_path = buff;
}

void ColladaImporter::RenameTextures() {
  ColladaImages::Iterator it = images_.Begin();
  ColladaImages::Iterator ie = images_.End();
  for (; it != ie; ++it) {
    Image* image = it.Value();
    ResourcePath imageSource(image->source_.Get());
    // If it is not the package chunk format,
    if (imageSource.GetResourcePathType() == ResourcePathType_File) {
      ResourcePath package_path(package_->resource_name(), image->file_name_.Get());
      Ptr<Texture> source_texture = LoadResource<Texture>(image->source_.Get());
      Check(source_texture);
      image->engine_texture_ = source_texture;
      source_texture->set_resource_name(package_path.Get());
      image->source_.Set(package_path.Get());
    }
  }
}

bool ColladaImporter::IsActiveMaterial(collada_importer::Material* material) {
  for (int idx = 0; idx < meshes_.Count(); ++idx) {
    for (int idx2 = 0; idx2 < meshes_.Get(idx)->triangles_list_.Count(); ++idx2) {
      if (meshes_.Get(idx)->triangles_list_.Get(idx2)->material_id_ == material->id_) {
        return true;
      }
    }
  }
  return false;
}

void ColladaImporter::ImportMaterials() {
  Check(!materials_.IsEmpty());
  ColladaMaterials::Iterator it = materials_.Begin();
  ColladaMaterials::Iterator ie = materials_.End();
  for (; it != ie; ++it) {
    Material* cur_material = it.Value();
    cur_material->Import();
  }
}

Chunk* ColladaImporter::GetPackageStreamWriterFromPath(const Cstr* path) {
  ResourcePath resource_path(path);
  return package_->OpenChunk(resource_path.GetChunkName(), Package::kWrite);
}

void ColladaImporter::ImportMeshes() {
  Check(!meshes_.IsEmpty());
  for (int idx = 0; idx < meshes_.Count(); ++idx) {
    Mesh* collada_mesh = meshes_.Get(idx);
    for (int idx2 = 0; idx2 < collada_mesh->triangles_list_.Count(); ++idx2) {
      Ptr<Triangles> colladaTris = collada_mesh->triangles_list_.Get(idx2);
      colladaTris->ImportMesh();
    }
  }
}

void ColladaImporter::ImportSkeleton() {
  if (skeleton_) {
    // Write the skeleton chunk
    {
      Ptr<Chunk> chunk = GetPackageStreamWriterFromPath(
          engine_skeleton_->resource_name());
      engine_skeleton_->WriteTo(chunk.ptr());
    }
  }
}

Ptr<Model> ColladaImporter::CreateEngineModel(SceneNode* node) {
  Ptr<Model> model = new Model;
  // Set the model path
  {
    String model_name(node->name_);
    model_name.AddRight(TXT(".mod"));
    ResourcePath model_path(package_->resource_name(), model_name.Get());
    model->set_resource_name(model_path.Get());
  }
  // Set the part
  {
    Mesh* mesh = node->skin_ ? node->skin_->GetMesh() : node->mesh_;
    model->parts_.Resize(mesh->triangles_list_.Count());
    for (int idx = 0; idx < mesh->triangles_list_.Count(); ++idx) {
      Ptr<Triangles> triangle = mesh->triangles_list_.Get(idx);
      Ptr<Part> engine_part = new Part;
      Ptr<EngineMesh> engine_mesh = triangle->engine_mesh_;
      Ptr<EngineMaterial> engine_material = triangle->GetCorrespondingEngineMaterial();
      {
        engine_part->mesh_ = engine_mesh;
        engine_part->material_ = engine_material;
        model->parts_.Get(idx) = engine_part;
        node->GetMatrix().GetPosition(model->local_transform_.position_);
      }
    }
  }
  return model;
}

void ColladaImporter::ImportScene() {
  dg::Scene scene;
  Scene::NodeMap::Iterator it = scene_.nodes_.Begin();
  Scene::NodeMap::Iterator ie = scene_.nodes_.End();
  for (; it != ie; ++it) {
    SceneNode* node = it.Value();
    if (node->parent_) {
      continue; // Import root scene node only
    }
    switch (node->node_type_) {
    case SceneNode::NodeType_Joint:
      break;
    case SceneNode::NodeType_Node:
      if (node->mesh_ || node->skin_) {
        Ptr<Model> model = CreateEngineModel(node);
        scene.AddModel(model.ptr());
        Ptr<Chunk> chunk = package_->OpenChunk(model->GetResourcePath().GetChunkName(), Package::kWrite);
        model->WriteTo(chunk.ptr());
      }
      break;
    }
  }
  Ptr<Chunk> chunk = package_->OpenChunk(TXT("scene"), Package::kWrite);
  scene.WriteTo(chunk.ptr());
}

// Skeleton should be created before ImportMeshes()
void ColladaImporter::CreateSkeleton() {
  if (skeleton_) {
    engine_skeleton_ = Skeleton::CreateEngineSkeleton(skeleton_);
    String skeletonName(skeleton_->name_);
    skeletonName.AddRight(TXT(".skel"));
    ResourcePath resource_path(package_->resource_name(), skeletonName.Get());
    engine_skeleton_->set_resource_name(resource_path.Get());
  }
}

void ColladaImporter::Import(const Cstr* package_name) {
  package_ = LoadResource<Package>(package_name, LoadResourceOption_Write);
  RenameTextures();
  ImportMaterials();
  CreateSkeleton();
  ImportMeshes();
  ImportSkeleton();
  ImportScene();
  ImportAnimations();
  SaveResource(package_.ptr());
}

void ColladaImporter::ReadMatrix(Element& elem, Matrix& out_matrix) {
  String strMatrix;
  elem.GetValue(strMatrix);
  StringStream floatStream(strMatrix.Get());
  float value;
  for (int idx = 0; idx < 16; ++idx) {
    if (floatStream.ReadFloat(value)) {
      out_matrix.Set(idx, value);
    }
  }
  // Column-major matrix(collada, opengl) to row-major matrix(d3d)
  out_matrix.StoreTranspose(out_matrix);
}

void ColladaImporter::ReadGeometry(Element& elem) {
  String geometry_id;
  elem.GetAttribute(TXT("id"), geometry_id);
  DG_FOR_EACH_ELEMENT(elem, sub_elem) {
    if (sub_elem.HasTagName(TXT("mesh"))) {
      Mesh* mesh = new Mesh;
      meshes_.PushBack(mesh);
      mesh->geometry_id_ = geometry_id;
      mesh->ReadFromXml(sub_elem);
    }
  }
}

void ColladaImporter::ReadControllers(Element& elem) {
  String controllerName;
  elem.GetAttribute(TXT("id"), controllerName);
  DG_FOR_EACH_ELEMENT(elem, sub_elem) {
    if (sub_elem.HasTagName(TXT("skin"))) {
      Skin* theSkin = new Skin(controllerName);
      skins_.PushBack(theSkin);
      theSkin->ReadFromXml(sub_elem);
    }
  }
}

void ColladaImporter::ReadVisualScene(Element& elem) {
  String sceneId;
  elem.GetAttribute(TXT("id"), sceneId);
  DG_FOR_EACH_ELEMENT(elem, sub_elem) {
    if (sub_elem.HasTagName(TXT("node"))) {
      Ptr<SceneNode> node = new SceneNode;
      node->parent_ = NULL;
      node->ReadFromXml(sub_elem);
      if (node->node_type_ == SceneNode::NodeType_Joint) {
        Check(!skeleton_);
        skeleton_ = node.ptr();
      }
    }
  }
}

void ColladaImporter::AddImage(const String& image_id, Image* image) {
  images_.Add(image_id.GetConst(), image);
}

Image* ColladaImporter::GetImage(const String& image_id) {
  return images_.FindOrNull(image_id.GetConst());
}

void ColladaImporter::AddMaterial(
    const String& material_id,
    collada_importer::Material* material) {
  materials_.Add(material_id.GetConst(), material);
}

collada_importer::Material* ColladaImporter::FindMaterial(const String& material_id) {
  return materials_.FindOrNull(material_id.GetConst());
}

Skin* ColladaImporter::FindSkin(const String& skin_name) {
  for (int idx = 0; idx < skins_.Count(); ++idx) {
    if (skins_.Get(idx)->id_== skin_name) {
      return skins_.Get(idx);
    }
  }
  return NULL;
}

collada_importer::Image* ColladaImporter::GetImageFromMaterial(
    collada_importer::Material& material) {
  DG_UNIMPLEMENTED();
  String ImageId = material.image_ids_[Material::Channel_DiffuseMap];
  return GetImage(ImageId);
}

void ColladaImporter::AddSceneNode(const String& node_id, SceneNode* node) {
  // This node will be released in collada_importer::Close()
  node->AddRef();
  scene_.Add(node_id, node);
}

SceneNode* ColladaImporter::FindSceneNode(const String& node_id) {
  return scene_.Find(node_id);
}

void ColladaImporter::ImportAnimations() {
  if (animations_.IsEmpty()) {
    return;
  }
  Ptr<EngineAnimation> engine_animation;
  // Create the engine anmation
  {
    String animation_name(TXT("animation.anim"));
    ResourcePath referencePath(collada_importer::g_collada_importer.package_->resource_name(), animation_name.Get());
    engine_animation = LoadResource<EngineAnimation>(referencePath.Get(), LoadResourceOption_Write);
  }
  // Maps collada_importer::animations_ into channels_
  engine_animation->channels().Resize(animations_.Count());
  for (int idx = 0; idx < animations_.Count(); ++idx) {
    Animation* cur_anim = animations_.Get(idx);
    cur_anim->channel_.SetEngineChannel(
        &engine_animation->channels().Get(idx));
    cur_anim->channel_.Import();
  }
  // Write the animation
  {
    Ptr<Chunk> chunk =
        collada_importer::g_collada_importer.GetPackageStreamWriterFromPath(
        engine_animation->resource_name());
    engine_animation->WriteTo(chunk.ptr());
  }
}
