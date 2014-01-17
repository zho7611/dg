// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {
namespace collada_importer {

class ColladaImporter {
public:
  typedef Array<Mesh*> ColladaMeshes;
  typedef Array<Skin*> ColladaSkins;
  typedef StringMap<Material*> ColladaMaterials;
  typedef StringMap<Image*> ColladaImages;
  typedef Array<Animation*> ColladaAnimations;
  typedef StringMap<Source*> ColladaSources;

  Ptr<Package> package_;
  ColladaMeshes meshes_;
  ColladaMaterials materials_;
  ColladaSkins skins_;
  ColladaImages images_;
  ColladaAnimations animations_;
  ColladaSources sources_;
  SceneNode* skeleton_; // The first joint node in the scene
  EngineSkeleton* engine_skeleton_;
  Scene scene_;
  bool is_reverse_cull_;

  ColladaImporter();
  ~ColladaImporter();
  bool Open(const Cstr* path);
  void Close();
  void Import(const Cstr* package_name);
  void ConvertURIPath(const String& uri_path, String& out_system_path) const;
  void AddImage(const String& image_id, Image* image);
  void AddMaterial(const String& material_id, Material* material);
  Mesh& GetCurrentMesh();
  Mesh* FindMesh(const String& geometry_id);
  Material* FindMaterial(const String& material_id);
  Skin* FindSkin(const String& skin_name);
  void AddSceneNode(const String& node_id, SceneNode* node);
  SceneNode* FindSceneNode(const String& node_id);
  void RenameTextures();
  bool IsActiveMaterial(Material* material);
  Chunk* GetPackageStreamWriterFromPath(const Cstr* path);
  const FilePath& GetFilePath() const {
    return path_;
  }
  static void ReadMatrix(Element& elem, Matrix& out_matrix);

private:
  Document document_;
  FilePath path_;

  void Parse();
  void ReadGeometry(Element& elem);
  void ReadControllers(Element& elem);
  void ReadVisualScene(Element& elem);
  void ReadImage(Element& elem);
  void ReadAnimation(Element& elem);
  Image* GetImage(const String& image_id);
  Image* GetImageFromMaterial(Material& material);
  void ImportMaterials();
  void ImportSkeleton();
  void ImportMeshes();
  Ptr<Model> CreateEngineModel(SceneNode* node);
  void ImportAnimations();
  void CreateSkeleton();
  void ImportScene();
  static Cstr ConvertHexCharToInt(Cstr ch);
};

extern ColladaImporter g_collada_importer;

} // End of namespace collada_importer
} // End of namespace dg
