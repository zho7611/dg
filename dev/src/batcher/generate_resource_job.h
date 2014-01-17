// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {
namespace batcher {

class GenerateResourceJob : public BatchJob {
public:
  void Usage() {
    PopupMessage(TXT("Info"), TXT("Options missing."));
  }

  bool Run(CommandLine* command_line) {
    DG_UNREF(command_line);
    if (!GenerateFont()) {
      return false;
    }
    GenerateDefaultResource();
    GenerateScooterCamera();
    GenerateFiremanCamera();
    GenerateShadowCamera();
    GenerateImageBorder();
    GenerateSphereGridScene();
    return true;
  }

  bool GenerateFont() {
    // First assure that there is no fonts.pak
    const Cstr kFontPackageFileName[] = TXT("res/generated/fonts.pak");
    DG_LOG_LINE(TXT("generatefont: creating:%s"), kFontPackageFileName);
    // Clear the existing font face list
    g_font_manager.ClearFaces();
    AutoDeleteBlob blob;
    Ptr<Package> package = LoadResource<Package>(kFontPackageFileName, LoadResourceOption_Write);
    package->Clear(); // Ignore the existing content
    // Font_a
    {
      Ptr<Font> font = new Font();
      font->set_resource_name(TXT("nanumgothic.ttf"));
      font->resource_owner_ = package;
      Chunk* write_chunk = package->OpenChunk(font->resource_name(), Package::kWrite);
      Ptr<Chunk> read_chunk = LoadResource<Chunk>(TXT("dev/res/fonts/nanumgothic.ttf"));
      font->CopyFrom(read_chunk->data_const(), read_chunk->size());
      font->WriteTo(write_chunk);
      package->CloseChunk(write_chunk);
      FontFace* font_face = new FontFace;
      font_face->Init(font.ptr(), 16, 16);
      g_font_manager.Add(TXT("font_a"), font_face);
    }
    // Font_b
    {
      Ptr<Font> font = new Font();
      font->set_resource_name(TXT("nanumgothic.ttf"));
      font->resource_owner_ = package;
      Chunk* write_chunk = package->OpenChunk(font->resource_name(), Package::kWrite);
      Ptr<Chunk> read_chunk = LoadResource<Chunk>(TXT("dev/res/fonts/nanumgothic.ttf"));
      font->CopyFrom(read_chunk->data_const(), read_chunk->size());
      font->WriteTo(write_chunk);
      package->CloseChunk(write_chunk);
      FontFace* font_face = new FontFace;
      font_face->Init(font.ptr(), 16, 16);
      g_font_manager.Add(TXT("font_b"), font_face);
    }
    // Font_c
    {
      Ptr<Font> font = new Font();
      font->set_resource_name(TXT("nanumgothic.ttf"));
      font->resource_owner_ = package;
      Chunk* write_chunk = package->OpenChunk(font->resource_name(), Package::kWrite);
      Ptr<Chunk> read_chunk = LoadResource<Chunk>(TXT("dev/res/fonts/nanumgothic.ttf"));
      font->CopyFrom(read_chunk->data_const(), read_chunk->size());
      font->WriteTo(write_chunk);
      package->CloseChunk(write_chunk);
      FontFace* font_face = new FontFace;
      font_face->Init(font.ptr(), 13, 13);
      g_font_manager.Add(TXT("font_c"), font_face);
    }
    SaveResource(package.ptr());
    return true;
  }

  // Generate default resources, like default texture, default material and default sphere mesh
  void GenerateDefaultResource() {
    // Create a default package
    const Cstr kDefualtPackageName[] = TXT("res/engine/default.pak");
    Ptr<Package> package = LoadResource<Package>(kDefualtPackageName, LoadResourceOption_Write);
    package->Clear();
    // Create a default texture
    ResourcePath texture_path(package->GetResourcePath().Get(), TXT("default_texture.tga"));
    Ptr<Texture> default_texture = LoadResource<Texture>(texture_path.Get(), LoadResourceOption_Write);
    // Read the texture
    {
      Ptr<Chunk> read_chunk = LoadResource<Chunk>(TXT("dev/res/engine/check.bmp"));
      // Import from the check texture
      NoDeleteBlob blob(read_chunk.ptr());
      default_texture->CopyFrom(&blob);
    }
    // Write the texture into the package
    {
      // Write the default texture chunk into the package
      Ptr<Chunk> write_chunk = package->OpenChunk(texture_path.GetChunkName(), Package::kWrite);
      default_texture->WriteTo(write_chunk.ptr());
    }
    for (int idx = 0; idx < DG_NUM_RENDERER_DEFAULT_TEXTURES; ++idx) {
      // Create a default gray texture
      String resource_name = g_renderer.default_textures[idx]->resource_name();
      ResourcePath texture_path(package->GetResourcePath().Get(), resource_name.Get());
      //g_renderer.default_textures[idx]->set_resource_name(texture_path.Get());
      {
        Ptr<Chunk> write_chunk = package->OpenChunk(texture_path.GetChunkName(), Package::kWrite);
        g_renderer.default_textures[idx]->BindResource();
        g_renderer.default_textures[idx]->SerializeFromDevice();
        g_renderer.default_textures[idx]->WriteTo(write_chunk.ptr());
      }
      // Set the original name back
      //g_renderer.default_textures[idx]->set_resource_name(resource_name.Get());
    }
    // Material
    {
      Ptr<ShaderEffect> effect = LoadResource<SimpleShaderEffect>(g_engine_config->default_effect_path_.Get(), LoadResourceOption_Write);
      ResourcePath material_path(package->GetResourcePath().Get(), TXT("default_material"));
      Ptr<Material> default_material = LoadResource<Material>(material_path.Get(), LoadResourceOption_Write);
      default_material->effect_ = effect;
      // Set the texture shader parameter
      {
        ShaderParameter* param = default_material->AddParameter(
            TXT("DiffuseTextureSampler"),
            ShaderParameter::kTextureType);
        param->SetTexture(default_texture.ptr());
      }
      Ptr<Chunk> write_chunk = package->OpenChunk(material_path.GetChunkName(), Package::kWrite);
      default_material->WriteTo(write_chunk.ptr());
    }
    // Sphere mesh
    {
      ResourcePath mesh_path(package->GetResourcePath().Get(), TXT("sphere_mesh"));
      Ptr<Mesh> mesh = LoadResource<Mesh>(mesh_path.Get(), LoadResourceOption_Write);
      MeshBuilder::BuildSphere(*mesh);
      Ptr<Chunk> write_chunk = package->OpenChunk(mesh_path.GetChunkName(), Package::kWrite);
      mesh->WriteTo(write_chunk.ptr());
    }
    // Actually write the package resource
    SaveResource(package.ptr());
  }

  void GenerateScooterCamera() {
    Ptr<SphereCamera> camera = new SphereCamera;
    // Set projection parameters
    {
      SphereCamera::ProjDesc& proj_desc = camera->proj_desc;
      proj_desc.aspect_ratio = float(800) / float(600);
      proj_desc.near_plane = 0.1f;
      proj_desc.far_plane = 100.0f;
    }
    // Set view parameters
    {
      SphereCamera::SphereViewDesc sphere_view_desc;
      sphere_view_desc.yaw = DegreeToRadian(30.0f);
      sphere_view_desc.pitch = 0;
      sphere_view_desc.roll = 0;
      sphere_view_desc.radius = 0.7f;
      sphere_view_desc.target.Set(0, 0, 0);
      camera->SetViewParam(sphere_view_desc);
    }
    camera->height_ = g_engine_config->camera_height_;
    // Write to disk
    {
      Ptr<Chunk> chunk = CreateWriteChunk(
          TXT("res/generated/scooter_camera.bin"));
      camera->WriteTo(chunk.ptr());
    }
  }

  void GenerateFiremanCamera() {
    Ptr<SphereCamera> camera = new SphereCamera;
    // Set projection parameters
    {
      SphereCamera::ProjDesc& proj_desc = camera->proj_desc;
      proj_desc.aspect_ratio = float(800) / float(600);
      proj_desc.near_plane = 0.1f;
      proj_desc.far_plane = 100.0f;
    }
    // Set view parameters
    {
      SphereCamera::SphereViewDesc sphere_view_desc;
      sphere_view_desc.yaw = 0;
      sphere_view_desc.pitch = 0;
      sphere_view_desc.roll = 0;
      sphere_view_desc.radius = 2.f;
      sphere_view_desc.target.Set(0, 0, 0);
      camera->SetViewParam(sphere_view_desc);
    }
    camera->height_ = g_engine_config->camera_height_;
    // Write to disk
    {
      Ptr<Chunk> chunk = CreateWriteChunk(
          TXT("res/generated/fireman_camera.bin"));
      camera->WriteTo(chunk.ptr());
    }
  }

  void GenerateShadowCamera() {
    Ptr<Camera> camera = new Camera;
    camera->is_orthogonal = true;
    camera->is_light_camera = true;
    // Set shadow camera view params
    {
      Camera::ViewDesc& desc = camera->view_desc;
      desc.target_pos.Set(0, 0, 0);
      desc.eye_pos.Set(0, -1.f, 1.f);
      desc.up_dir.Set(0, 1.f, 5.f);
    }
    // Set shadow camera projection params
    {
      Camera::OrthoDesc& desc = camera->ortho_desc;
      desc.left = -1.0f;
      desc.right = 1.0f;
      desc.bottom = -1.0f;
      desc.top_ = 1.0f;
      desc.near_plane = .5f;
      desc.far_plane = 100.0f;
    }
    // Write to disk
    {
      Ptr<Chunk> chunk = CreateWriteChunk(
          TXT("res/generated/shadow_camera.bin"));
      camera->WriteTo(chunk.ptr());
    }
  }

  void GenerateImageBorder() {
    Ptr<Texture> border_texture = LoadResource<Texture>(TXT("dev/res/gui/border_shiny.tga"));
    ImageBorder border;
    // Left
    {
      border.left_.draw_rect.Set(0, 0, Size2(16, 32));
      border.left_.tex_coord_rect = border.left_.draw_rect;
      border.left_.texture = border_texture;
    }
    // Center
    {
      border.center_.draw_rect.Set(16, 0, 41, 32);
      border.center_.tex_coord_rect = border.center_.draw_rect;
      border.center_.texture = border_texture;
    }
    // Right
    {
      border.right_.draw_rect.Set(41, 0, 73, 32);
      border.right_.tex_coord_rect = border.right_.draw_rect;
      border.right_.texture = border_texture;
    }
    border.color = DG_MAKE_COLOR_INT(255, 255, 255, 255);
    // Write to disk
    {
      Ptr<Chunk> chunk = CreateWriteChunk(
          TXT("res/generated/image_border.bin"));
      border.WriteTo(chunk.ptr());
    }
  }

  void GenerateSphereGridScene() {
    const int kNumSpheresInX = 5;
    const int kNumSpheresInY = 5;
    const int kNumSpheresInZ = 5;
    const Cstr kSphereGridPackageFileName[] = TXT("res/generated/sphere_grid.pak");
    Ptr<Package> package = LoadResource<Package>(kSphereGridPackageFileName, LoadResourceOption_Write);
    // Mark the package as read-done state. If the package is not marked as read-done, it cannot be serialized
    package->OnReadDone();
    String sceneName = kSphereGridPackageFileName;
    sceneName.AddRight(TXT("#scene"));
    Ptr<Scene> scene = LoadResource<Scene>(sceneName.Get(), LoadResourceOption_Write);
    Ptr<Mesh> mesh = LoadResource<Mesh>(TXT("res/engine/default.pak#sphere_mesh"), LoadResourceOption_Write);
    Ptr<Material> material = LoadResource<Material>(TXT("res/engine/default.pak#default_material"), LoadResourceOption_Write);
    int count = 0;
    for (int idx_z = 0; idx_z < kNumSpheresInZ; ++idx_z) {
      for (int idx_y = 0; idx_y < kNumSpheresInY; ++idx_y) {
        for (int idx_x = 0; idx_x < kNumSpheresInX; ++idx_x) {
          // Assign a unique name for the model
          String unique_model_name;
          unique_model_name.Format(TXT("model_%03d"), count++);
          Ptr<Model> model = new Model;
          model->AddPart(mesh.ptr(), material.ptr());
          model->local_transform_.scale_ = .5f / float(kNumSpheresInX);
          model->local_transform_.position_.Set(
              -0.5f + float(idx_x)/(kNumSpheresInX-1),
              -0.5f + float(idx_y)/(kNumSpheresInY-1),
              -0.5f + float(idx_z)/(kNumSpheresInZ-1));
          model->set_resource_name(unique_model_name.Get());
          scene->AddModel(model.ptr());
          Ptr<Chunk> model_chunk = package->OpenChunk(model->resource_name(), Package::kWrite);
          model->resource_owner_ = package;
          model->WriteTo(model_chunk.ptr());
        }
      }
    }
    {
      Ptr<Chunk> scene_chunk = package->OpenChunk(TXT("scene"), Package::kWrite);
      scene->resource_owner_ = package;
      // Write the shader into the package chunk
      scene->WriteTo(scene_chunk.ptr());
    }
    // Serialize Scene Package
    SaveResource(package.ptr());
  }
};

} // namespace batcher
} // namespace dg
