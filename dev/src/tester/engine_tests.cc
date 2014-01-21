// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "tester.h"
#include "../engine/engine_first.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../externals/stb_image_write/stb_image_write.h"

using namespace dg;

bool TestRenderTarget(const Cstr* testname) {
#if defined(DG_RENDERER_GL2)
  return true;
#endif
  RenderTarget* target = g_renderer.view_state.target.ptr();
  Tester& tester = Tester::Instance();
  String path = testname;
  path.AddRight(TXT(".raw"));
  path.AddLeft(TXT("dev/res/test/result/"));
  // Writing Test
  {
    Ptr<Chunk> chunk = CreateWriteChunk(path.Get());
    target->WriteTo(chunk.ptr());
    // Also writes to png
    String png_path = testname;
    png_path.AddRight(TXT(".png"));
    png_path.AddLeft(TXT("dev/res/test/result/"));
    // Change BGRA(0xARGB) to RGBA(0xABGR)
    uint8_t* buff = new uint8_t[chunk->size()];
    uint8_t* cur = NULL;
    MyMemCpy(buff, chunk->data(), chunk->size());
    for (int y = 0; y < target->height(); ++y) {
      for (int x = 0; x < target->width(); ++x) {
        cur = buff + (y*target->width() + x)*4;
        uint8_t b = cur[0];
        uint8_t g = cur[1];
        uint8_t r = cur[2];
        uint8_t a = cur[3];
        cur[0] = r;
        cur[1] = g;
        cur[2] = b;
        cur[3] = a;
      }
    }
    Check(stbi_write_png(png_path.GetAnsiStr(), target->width(), target->height(), 4, buff, target->width()*4));
    delete buff;
  }
  const bool IsImageTest = true;
  return tester.TestFileCompareOrCopyToOriginal(path.Get(), IsImageTest);
}

TEST(Box) {
  Box box;
  box.center_.Normalize();
  box.size_ = Vector3::kOne;
}

TEST(Sphere) {
  Ptr<Mesh> mesh = new Mesh;
  Sphere sphere;
  sphere.radius_ = 1.0f;
  sphere.num_segments_ = 10;
}

TEST(PackageReadWrite) {
  const Cstr kPackagePath[] = TXT("dev/res/test/result/package_read_write.pak");
  const Cstr kModelPath[] = TXT("dev/res/test/result/package_read_write.pak#1234");
  // Write package
  {
    Ptr<Package> package = LoadResource<Package>(kPackagePath, LoadResourceOption_Write);
    Model model;
    ResourcePath resource_path(kModelPath);
    Ptr<Chunk> chunk = package->OpenChunk(resource_path.GetChunkName(), Package::kWrite);
    model.WriteTo(chunk.ptr());
    chunk->Flush();
    SaveResource(package.ptr());
  }
  // Read a Model
  {
    Ptr<Model> model = LoadResource<Model>(kModelPath);
    EXPECT_TRUE(model);
  }
  // Read a Package
  {
    Ptr<Package> package = LoadResource<Package>(kPackagePath, LoadResourceOption_Read);
    EXPECT_TRUE(package);
    int kNumChunks = package->chunks_.Count();
    EXPECT_TRUE(kNumChunks == 1);
    int kNumOpenResources = g_resource_manager->open_resources_.Count();
    CloseResource(package.ptr());
    EXPECT_TRUE(g_resource_manager->open_resources_.Count() == (kNumOpenResources-1));
  }
}

TEST(Mesh) {
  ScopedRelease<Mesh> mesh = new Mesh;
  mesh->AddRef();
  Ptr<Mesh> mesh2(mesh.ptr());
  Ptr<Mesh> mesh3(mesh2.ptr());
  Mesh* mesh4 = mesh2.ptr();
  EXPECT_TRUE(mesh3 == mesh2);
  EXPECT_TRUE(mesh3.ptr() == mesh3.ptr());
}

TEST(MeshBuild) {
  Ptr<Mesh> mesh = new Mesh;
  mesh->prim_type_ = DrawPrimitiveType_TriangleList;
  // Test index buffer
  mesh->index_buffer_.InitIndexBuffer(3);
  mesh->index_buffer_.BindResource();
  {
    IndexBuffer::IndexType* ibuff = NULL;
    IndexBuffer::ScopedLock locker(
      mesh->index_buffer_, ibuff);
    ibuff[0] = 0;
    ibuff[1] = 1;
    ibuff[2] = 2;
  }
  mesh->index_buffer_.UnbindResource();
  // Test vertex buffer
  const int num_vertices = 3;
  mesh->vertex_buffer_.InitVertexBuffer(
    VertexDecl::Preset_PosTex, num_vertices);
  mesh->vertex_buffer_.BindResource();
  {
    VertexFormat::FormatPosTex* vbuff = NULL;
    VertexBuffer::ScopedLock locker(mesh->vertex_buffer_, vbuff);
    vbuff->position = Vector3(0, 0, 0);
    vbuff->tex_coord = Vector2(0, 0);
    ++vbuff;
    vbuff->position = Vector3(0, 1, 0);
    vbuff->tex_coord = Vector2(0, 1);
    ++vbuff;
    vbuff->position = Vector3(1, 0, 0);
    vbuff->tex_coord = Vector2(1, 0);
    ++vbuff;
  }
  mesh->vertex_buffer_.UnbindResource();
}

TEST(CustomVertexDecl) {
  struct CustomVertexFormat {
    Vector2 position;
    Vector2 texcoord;
    Vector4 color;
  };
  VertexBuffer vb;
  vb.InitVertexBuffer(VertexDecl::Preset_Custom, 1);
  VertexDecl& decl = vb.vertex_decl();
  decl.elements().Clear();
  VertexElement pos(VertexElement::DataType_Float2, VertexElement::Usage_Position);
  VertexElement tex(VertexElement::DataType_Float2, VertexElement::Usage_TexCoord);
  VertexElement col(VertexElement::DataType_Float4, VertexElement::Usage_Color);
  decl.elements().PushBack(pos);
  decl.elements().PushBack(tex);
  decl.elements().PushBack(col);
  EXPECT_TRUE(decl.stride() == 32);
}

TEST(RenderTarget) {
  Ptr<RenderTarget> target = new RenderTarget;
  target->Init(
      Size2(32, 32),
      GpuFormat_A16B16G16R16F,
      GpuFormat_D24S8);
}

TEST(SphereCameraWrite) {
  const float AspectRatio = float(800) / float(600);
  // Write a SphereCamera
  {
    Ptr<SphereCamera> camera = new SphereCamera;
    SphereCamera::ProjDesc& proj_desc = camera->proj_desc;
    proj_desc.aspect_ratio = AspectRatio;
    camera->height_ = 1.f;
    Ptr<Chunk> chunk = CreateWriteChunk(
        TXT("dev/res/test/result/sphere_camera.pak"));
    camera->WriteTo(chunk.ptr());
  }
  // Read the SphereCamera
  {
    Ptr<SphereCamera> camera = LoadResource<SphereCamera>(
        TXT("dev/res/test/result/sphere_camera.pak"));
    EXPECT_TRUE(camera->proj_desc.aspect_ratio == AspectRatio);
    EXPECT_TRUE(camera->height_ == 1.f);
  }
}

TEST(VectorShadowCameraWrite) {
  // Write a camera
  {
    Ptr<Camera> camera = new Camera;
    camera->is_orthogonal = true;
    // Set the camera view params
    {
      Camera::ViewDesc& view_desc = camera->view_desc;
      view_desc.target_pos.Set(0, 0, 0);
      view_desc.eye_pos.Set(0.0f, 1.0f, 0.0f);
      view_desc.up_dir.Set(0, 0, 1.f);
    }
    // Set the camera projection params
    {
      Camera::ProjDesc& proj_desc = camera->proj_desc;
      proj_desc.fov_y = DegreeToRadian(45.0f);
      proj_desc.aspect_ratio = 1;
      proj_desc.near_plane = 1.0f;
      proj_desc.far_plane = 50.0f;
    }
    Ptr<Chunk> chunk = CreateWriteChunk(
        TXT("dev/res/test/result/vector_shadow_camera.pak"));
    camera->WriteTo(chunk.ptr());
  }
  // Read the camera
  {
    Ptr<Camera> camera = LoadResource<Camera>(
        TXT("dev/res/test/result/vector_shadow_camera.pak"));
    EXPECT_TRUE(camera->proj_desc.far_plane == 50.0f);
  }
}

TEST(TextureLockA8) {
  Ptr<Texture> tex = new Texture;
  tex->Init(GpuUsage_None, Size2(256, 256), GpuFormat_A8);
  uint8_t* buff = NULL;
  int pitch = 0;
  tex->LockTexture(buff, pitch, GpuLockType_None);
  for (int y = 0; y < 256; ++y) {
    for (int x = 0; x < 256; ++x) {
      buff[y*pitch + x] = x;
    }
  }
  tex->UnlockTexture();
}

TEST(TextureLockA8R8G8B8) {
  Ptr<Texture> tex = new Texture;
  tex->Init(GpuUsage_None, Size2(256, 256), GpuFormat_A8R8G8B8);
  uint8_t* buff = NULL;
  int pitch = 0;
  Texture::ScopedLock locker(*tex, buff, pitch);
  for (int y = 0; y < 256; ++y) {
    for (int x = 0; x < 256; ++x) {
      buff[y*pitch + x*4] = DG_MAKE_COLOR_INT(255, x, y, 0);
    }
  }
}

TEST(TextureWrite) {
  const Cstr* path = TXT("dev/res/test/result/texture.dds");
  Ptr<Texture> test_texture = LoadResource<Texture>(path, LoadResourceOption_Write);
  test_texture->Init(GpuUsage_None, Size2(256, 256), GpuFormat_A8R8G8B8);
  struct Texel {
    uint8_t b;
    uint8_t g;
    uint8_t r;
    uint8_t a;
  };
  Check(sizeof(Texel) == 4);
  uint8_t* buffer = NULL;
  int pitch = 0;
  const int kTexelSize = sizeof(Texel);
  {
    Texture::ScopedLock locker(*test_texture, buffer, pitch);
    Check(buffer);
    const Size2& size = test_texture->size();
    for (int y = 0; y < size.y; ++y) {
      for (int x = 0; x < size.x; ++x) {
        Texel* texel = (Texel*)(buffer + pitch*y + kTexelSize*x);
        texel->a = 255;
        texel->r = x;
        texel->g = (x + y) / 2;
        texel->b = y;
      }
    }
  }
  // #fixme: gl is not doing well the following
#if defined(DG_RENDERER_DX9)
  EXPECT_TRUE(test_texture->SerializeFromDevice());
  {
    Ptr<Chunk> chunk = CreateWriteChunk(path);
    test_texture->WriteTo(chunk.ptr());
  }
  EXPECT_TRUE(Tester::Instance().TestFileCompareOrCopyToOriginal(path, true /* is_image_test */));
  {
    Ptr<Texture> test_tex = LoadResource<Texture>(path);
    EXPECT_TRUE(test_tex);
  }
#endif
}

TEST(MaterialWrite) {
  TEST_DEPENDS_ON(TextureWrite);
  const Cstr* effect_path = g_engine_config->default_effect_path_.Get();
  Ptr<ShaderEffect> effect = LoadResource<SimpleShaderEffect>(effect_path);
  Ptr<Material> material = new Material;
  EXPECT_TRUE(effect);
  EXPECT_TRUE(material);
  material->effect_ = effect;
  // Set the texture shader parameter
  {
    Ptr<Texture> default_tex = LoadResource<Texture>(TXT("res/engine/default.pak#default_texture.tga"));
    EXPECT_TRUE(default_tex);
    // To make the blob have data
    default_tex->BindResource();
    default_tex->SerializeFromDevice();
    Ptr<Texture> normal_purple_tex = LoadResource<Texture>(TXT("res/engine/default.pak#normalpurple.tga"));
    EXPECT_TRUE(normal_purple_tex);
    // To make the blob have data
    normal_purple_tex->BindResource();
    normal_purple_tex->SerializeFromDevice();
    Ptr<Texture> white_tex = LoadResource<Texture>(TXT("res/engine/default.pak#white.tga"));
    EXPECT_TRUE(white_tex);
    // To make the blob have data
    white_tex->BindResource();
    white_tex->SerializeFromDevice();
    EXPECT_TRUE(material->GetNumParameters() == 0);
    ShaderParameter* param = material->AddParameter(TXT("DiffuseTextureSampler"), ShaderParameter::kTextureType);
    param->SetTexture(default_tex.ptr());
    param = material->AddParameter(TXT("NormalTextureSampler"), ShaderParameter::kTextureType);
    param->SetTexture(normal_purple_tex.ptr());
    param = material->AddParameter(TXT("SpecularTextureSampler"), ShaderParameter::kTextureType);
    param->SetTexture(white_tex.ptr());
    param = material->AddParameter(TXT("GlobalMaterialDiffuse"), ShaderParameter::kVector4Type);
    Vector4 material_diffuse(.5f, .5f, .5f, 1.0f);
    param->SetVector4(&material_diffuse);
    material_util::SetMaterialAmbient(material.ptr(), 0.5f, 0.5f, 0.5f);
    material_util::SetMaterialSpecular(material.ptr(), .5f, .5f, 1.0f, 10.0f);
    EXPECT_TRUE(param);
    EXPECT_TRUE(material->GetNumParameters() == 6);
  }
  Ptr<Chunk> chunk = CreateWriteChunk(TXT("dev/res/test/result/material.mat"));
  material->WriteTo(chunk.ptr());
}

TEST(Animation) {
  Ptr<Animation> animation = new Animation;
  animation->channels().Resize(1);
  Channel& channel = animation->channels().Get(0);
  const int kNumSamples = 10;
  const Channel::Semantic semantic = Channel::Semantic_Matrix;
  channel.SetChannel(kNumSamples, semantic);
  Matrix idenity;
  idenity.MakeIdentity();
  for (int idx = 0; idx < kNumSamples; ++idx) {
    channel.SetValue(idx, idenity);
    channel.GetValue(idx, idenity);
  }
}

TEST(PrepareRender) {
  TEST_DEPENDS_ON(SphereCameraWrite);
  TEST_DEPENDS_ON(VectorShadowCameraWrite);
  Ptr<View> view = new View;
  view->view_size.Set(1024, 768);
  view->OnReadDone();
  view->set_camera(LoadResource<SphereCamera>(
      TXT("dev/res/test/result/sphere_camera.pak")));
  view->set_light_camera(LoadResource<Camera>(
      TXT("dev/res/test/result/vector_shadow_camera.pak")));
  g_app->view_ = view;
}

TEST(ShaderEffect) {
  DefaultShaderEffect effect;
  Ptr<Chunk> chunk = LoadResource<Chunk>(TXT("dev/src/shaders/test_effect.json"), LoadResourceOption_Read);
  bool ret = ShaderEffectParser::ParseChunk(*chunk, &effect);
  EXPECT_TRUE(ret);
  EXPECT_TRUE(effect.effect_name_.IsEqual(TXT("TestEffect")));
  EXPECT_TRUE(effect.techniques_.Count() == 3);
  EXPECT_TRUE(effect.techniques_.Get(2).passes_.Count() == 2);
  // ShaderPass test
  {
    // Depth pass test
    ShaderPass& pass = effect.techniques_.Get(0).passes_.Get(0);
    const int kNumRenderStates = pass.render_states_.Count();
    EXPECT_TRUE(kNumRenderStates == 5);
    RenderState_AlphaBlendEnable* state0 = static_cast<RenderState_AlphaBlendEnable*>(
        pass.render_states_.Find(RenderStateType_AlphaBlendEnable));
    EXPECT_TRUE(state0->value_ == false);
    RenderState_AlphaTestEnable* state1 = static_cast<RenderState_AlphaTestEnable*>(
        pass.render_states_.Find(RenderStateType_AlphaTestEnable));
    EXPECT_TRUE(state1->value_ == true);
    RenderState_ZFunc* state2 = static_cast<RenderState_ZFunc*>(
        pass.render_states_.Find(RenderStateType_ZFunc));
    EXPECT_TRUE(state2->value_ == RenderStateCompareType_LessEqual);
    RenderState_ScissorTestEnable* state_scissor = static_cast<RenderState_ScissorTestEnable*>(
      pass.render_states_.Find(RenderStateType_ScissorTestEnable));
    EXPECT_TRUE(state_scissor->value_ == false);
    // Specular pass test
    ShaderPass& passSpecular = effect.techniques_.Get(2).passes_.Get(1);
    RenderState_ZWriteEnable* state3 = static_cast<RenderState_ZWriteEnable*>(
        passSpecular.render_states_.Find(RenderStateType_ZWriteEnable));
    EXPECT_TRUE(state3->value_ == false);
    RenderState_SrcBlend* state4 = static_cast<RenderState_SrcBlend*>(
        passSpecular.render_states_.Find(RenderStateType_SrcBlend));
    EXPECT_TRUE(state4->value_ == RenderStateBlendType_SrcAlpha);
    RenderState_BlendOp* state5 = static_cast<RenderState_BlendOp*>(
        passSpecular.render_states_.Find(RenderStateType_BlendOp));
    EXPECT_TRUE(state5->value_ == RenderStateBlendOpType_Add);
    RenderState_CullMode* state6 = static_cast<RenderState_CullMode*>(
      pass.render_states_.Find(RenderStateType_CullMode));
    EXPECT_TRUE(state6->value_ == CullModeType_CCW);
  }
  // Sampler state test
  {
    ShaderPass& pass = effect.techniques_.Get(0).passes_.Get(0);
    const int kNumSamplerStates = pass.sampler_states_.Count();
    EXPECT_TRUE(kNumSamplerStates == 12);
    SamplerState_AddressU* state0 = static_cast<SamplerState_AddressU*>(
        pass.sampler_states_.Find(SamplerStateType_AddressU));
    EXPECT_TRUE(state0->value_ == TextureAddressType_Clamp);
    SamplerState_MagFilter* state1 = static_cast<SamplerState_MagFilter*>(
        pass.sampler_states_.Find(SamplerStateType_MagFilter));
    EXPECT_TRUE(state1->value_ == TextureFilterType_Linear);
  }
  // Shader parameter test
  {
    ShaderParameter param;
    EXPECT_TRUE(param.register_count() == 1);
    EXPECT_TRUE(param.register_index() == -1);
  }
  // Rendering test
  TEST_DEPENDS_ON(PrepareRender);
  {
    Ptr<Mesh> mesh = new Mesh;
    MeshBuilder::BuildBox(*mesh, Vector3(1.f, 1.f, 1.f));
    g_renderer.BeginScene();
    effect.SubmitMesh(mesh.ptr(), NULL);
    g_renderer.EndScene();
  }
}

TEST(SimpleRender) {
  TEST_DEPENDS_ON(PrepareRender);
  g_app->Update();
  g_app->Render();
  EXPECT_TRUE(TestRenderTarget(TXT("simple")));
}

TEST(RenderState) {
  TEST_DEPENDS_ON(PrepareRender);
  RenderState_AlphaBlendEnable alpha_blend_state(true);
  RenderState_AlphaFunc alpha_func(RenderStateCompareType_LessEqual);
  RenderState_AlphaRef alpha_ref(128);
  RenderState_ZEnable z_enable(true);
  RenderState_ZFunc z_func(RenderStateCompareType_LessEqual);
  RenderState_CullMode cull_mode(CullModeType_CCW);
  EXPECT_TRUE(alpha_blend_state.Apply());
  EXPECT_TRUE(alpha_func.Apply());
  EXPECT_TRUE(alpha_ref.Apply());
  EXPECT_TRUE(z_enable.Apply());
  EXPECT_TRUE(z_func.Apply());
  EXPECT_TRUE(cull_mode.Apply());
}

void TestPresent(float wait_secs = 1.f) {
  g_app->PresentScreen();
  Os::SleepMilliSeconds(int(1000.f*(wait_secs))); // To see by eyes
}

TEST(FontRender) {
  TEST_DEPENDS_ON(PrepareRender);
  Ptr<Texture> font_texture = new Texture;
  font_texture->Init(GpuUsage_Dynamic, Size2(640, 128), GpuFormat_A8);
  // Render text with font
  {
    FontFace* font_face = FontFace::GetFontFace(TXT("font_a"));
    font_face->RenderText(
        font_texture.ptr(),
        TXT("ABcd !@#$ 1234 한글"),
        Point2(16, 32));
  }
  g_app->Update();
  // View block
  {
    ScopedNormalTechnique technique;
    g_app->view_->PreRender(&technique);
    // Draw font sprite
    {
      Rect rect(font_texture->size());
      ColorInt color = DG_MAKE_COLOR_INT(255, 255, 255, 255);
      g_renderer.font_sprite.DrawSprite(font_texture.ptr(), rect, color);
    }
    g_app->view_->PostRender(&technique);
  }
  TestPresent();
  EXPECT_TRUE(TestRenderTarget(TXT("font")));
}

TEST(ImageRender) {
  TEST_DEPENDS_ON(PrepareRender);
  Ptr<Texture> tex = LoadResource<Texture>(TXT("dev/res/test/zhologo.png"));
  tex->BindResource();
  g_app->Update();
  ScopedNormalTechnique technique;
  g_app->view_->PreRender(&technique);
  {
    const int kAlpha = 128;
    g_renderer.translucent_sprite.DrawSprite(
        tex.ptr(),
        Point2(0, 0),
        DG_MAKE_COLOR_INT(kAlpha,255,255,255));
    g_renderer.translucent_sprite.DrawSprite(
        tex.ptr(),
        Rect(50, 50, 200, 200),
        DG_MAKE_COLOR_INT(255,255,255,255));
    g_renderer.translucent_sprite.DrawSprite(
        tex.ptr(),
        Rect(100, 100, 400, 400),
        DG_MAKE_COLOR_INT(kAlpha,255,255,255));
  }
  g_app->view_->PostRender(&technique);
  TestPresent();
  EXPECT_TRUE(TestRenderTarget(TXT("image")));
}

TEST(LineRender) {
  TEST_DEPENDS_ON(PrepareRender);
  g_app->Update();
  {
    ScopedNormalTechnique technique;
    g_app->view_->PreRender(&technique);
    LineDrawer& drawer = g_line_drawer;
    drawer.PreRender();
    g_renderer.ClearScreen(DG_MAKE_COLOR_INT(255, 255, 255, 255), true /* is_depth_clear */);
    {
      drawer.DrawLine(Vector3(-100.f, -100.f, -100.f), Vector3(100.0f, 100.f, 100.f), DG_MAKE_COLOR_INT(255, 255, 255, 255));
      drawer.DrawLine(Vector3(0, 0, 0), Vector3(2, 0, 0), DG_MAKE_COLOR_INT(255, 255, 0, 0));
      drawer.DrawLine(Vector3(0, 0, 0), Vector3(0, 2, 0), DG_MAKE_COLOR_INT(255, 0, 255, 0));
      drawer.DrawLine(Vector3(0, 0, 0), Vector3(0, 0, 2), DG_MAKE_COLOR_INT(255, 0, 0, 255));
    }
    drawer.PostRender();
    g_app->view_->PostRender(&technique);
  }
  TestPresent();
  EXPECT_TRUE(TestRenderTarget(TXT("line")));
}

TEST(Line2Render) {
  TEST_DEPENDS_ON(PrepareRender);
  g_app->Update();
  {
    ScopedNormalTechnique technique;
    g_app->view_->PreRender(&technique);
    Line2Drawer& drawer = g_line2d_drawer;
    drawer.PreRender();
    {
      Point2 p[4];
      ColorInt color;
      for (int idx = 0; idx <= 255/2; ++idx) {
        int width = 255-idx*2;
        if ((idx % 10) == 0) {
          color = DG_MAKE_COLOR_INT(255, 255, 255, 255);
        } else {
          color = DG_MAKE_COLOR_INT(255, 255-idx, 0, 0);
        }
        p[0].Set(idx, idx);
        p[1].Set(idx + width, idx);
        p[2].Set(idx + width, idx + width);
        p[3].Set(idx, idx + width);
        drawer.DrawLine2(p[0], p[1], color);
        drawer.DrawLine2(p[1], p[2], color);
        drawer.DrawLine2(p[2], p[3], color);
        drawer.DrawLine2(p[3], p[0], color);
      }
    }
    drawer.PostRender();
    g_app->view_->PostRender(&technique);
  }
  TestPresent();
  EXPECT_TRUE(TestRenderTarget(TXT("line2")));
}

TEST(RectRender) {
  TEST_DEPENDS_ON(PrepareRender);
  const int kNumIterations = 100;
  // Rand()%256, rand()%256, rand()%256, rand()%256
  const ColorInt colors_argb[] = {
    0x9049f1f1, 0xdb3c870c, 0x1c06b747, 0x43bb8ba6, 0x38251f5d, 0x453b130d, 0x32209a50, 0x124932f6, 0x4f14f244, 0x30b7323b,
    0x9de18b1f, 0xb9729d49, 0xd5e980b2, 0x67d6bf14, 0x6683ef57, 0x61cdd11e, 0xe61df084, 0xe8392c53, 0x749e0cf4, 0x597e35cf,
    0xd3902d48, 0x1d2ae5c0, 0x440e5f50, 0x7b051507, 0x7092da64, 0x6915f846, 0xd9162f67, 0xd0576876, 0xae248879, 0xe53cf445,
    0xf5c56059, 0x762dd0c2, 0x6a792508, 0x6aa51128, 0x87978c2f, 0xc19be1c0, 0x86c2b554, 0xd1559038, 0x665e4ee1, 0x9fe2a5e2,
    0x382a4689, 0x78c263b1, 0x3e62e096, 0x3f895ef1, 0xa11e201d, 0x7840f14, 0xc9c5e72c, 0xf3a88dfe, 0xa0d03b75, 0x734dd0be,
    0x412b8cfa, 0x54863295, 0xe6fcf5ca, 0x411bfdb8, 0x1499cdd3, 0xa6665333, 0x4cec034c, 0xeaaadcf, 0xd89d144d, 0x7c9cc1f8,
    0x64b8a687, 0x4e2d99e7, 0x80ad0841, 0x9fe4189f, 0x4cd12104, 0x5340438a, 0xcfd3a195, 0x2af607ad, 0xc9c06932, 0x8f1756c6,
    0xb888138, 0xf954d0e7, 0x92291d86, 0x1cfa4f37, 0xf5df806c, 0x43651108, 0xeb15b8e0, 0x7bf45b62, 0xf7e5874a, 0x84c6c3b1,
    0x756f032f, 0x9bc88143, 0x9e47a181, 0x9ede81a3, 0x43cdcf57, 0x1c270328, 0x49344e60, 0x42b20d2b, 0x2f73e251, 0xd31f01be,
    0x21831915, 0x8bf90edc, 0x6d943192, 0x90d55e6a, 0x4b176b36, 0xea721e82, 0x5ed94eaf, 0x1d75e5dc, 0x5b81a07f, 0x49740255
  };
  Check(sizeof(colors_argb) == sizeof(ColorInt)*kNumIterations);
  // Rand()%g_app->view->view_size.x, rand()%g_app->view->view_size.y, rand()%400, rand()%400
  const int rect_points[] = {
    558, 78, 124, 369, 595, 491, 361, 27, 382, 292, 153, 302, 269, 138, 371, 126, 733, 122, 211, 303, 662, 44, 347, 68, 635, 316, 378, 329, 246, 448, 64, 142, 348, 393, 301, 206, 331, 176, 166, 240, 482, 518, 338, 337,
    377, 330, 304, 58, 229, 270, 272, 124, 755, 36, 361, 90, 124, 141, 350, 150, 553, 287, 257, 137, 146, 422, 188, 221, 610, 455, 362, 191, 350, 2, 41, 395, 468, 399, 148, 21, 588, 300, 338, 18,
    710, 21, 207, 83, 449, 200, 251, 135, 589, 209, 244, 208, 703, 514, 387, 123, 389, 281, 398, 196, 179, 38, 292, 138, 202, 111, 356, 88, 433, 275, 86, 362, 421, 51, 322, 398, 110, 200, 312, 275,
    202, 223, 55, 189, 532, 232, 157, 217, 625, 492, 368, 129, 553, 118, 60, 145, 666, 37, 149, 129, 282, 488, 105, 286, 313, 263, 186, 271, 556, 113, 145, 232, 729, 422, 396, 144, 39, 492, 59, 44,
    474, 513, 386, 249, 425, 2, 191, 358, 633, 359, 372, 274, 268, 563, 270, 173, 127, 499, 2, 27, 232, 403, 181, 261, 264, 86, 22, 142, 433, 235, 370, 270, 295, 494, 140, 150, 66, 571, 302, 258,
    400, 352, 319, 64, 627, 576, 315, 170, 777, 487, 365, 286, 562, 302, 120, 223, 602, 460, 64, 125, 720, 153, 347, 11, 500, 29, 262, 51, 39, 258, 0, 277, 601, 487, 313, 308, 704, 511, 140, 205,
    26, 416, 156, 85, 222, 296, 161, 269, 229, 129, 23, 41, 134, 584, 162, 53, 111, 83, 207, 163, 111, 354, 142, 223, 5, 134, 26, 270, 134, 437, 334, 193, 13, 300, 281, 148, 678, 24, 77, 11,
    18, 272, 75, 113, 88, 240, 231, 369, 251, 114, 153, 345, 181, 64, 5, 392, 761, 156, 349, 397, 477, 201, 374, 213, 270, 159, 192, 1, 647, 287, 170, 298, 289, 571, 210, 363, 432, 18, 304, 391,
    784, 447, 298, 222, 278, 146, 312, 345, 45, 540, 65, 344, 287, 152, 72, 332, 415, 569, 0, 327, 63, 209, 237, 143, 354, 558, 21, 60, 667, 148, 20, 230, 24, 429, 253, 238, 726, 518, 155, 144,
    364, 515, 184, 296, 206, 26, 272, 348, 565, 93, 175, 212, 438, 536, 252, 256, 637, 211, 225, 241, 652, 416, 353, 93, 148, 456, 303, 234, 58, 318, 280, 0, 502, 509, 43, 76,
  };
  Check(sizeof(rect_points) == sizeof(int)*4*kNumIterations);
  g_app->Update();
  dg::Rect rect;
  ColorInt color;
  RenderState_ScissorTestEnable scissor_state(true);
  for (int idx_max = kNumIterations-1; idx_max < kNumIterations; ++idx_max) {
    {
      ScopedNormalTechnique technique;
      g_app->view_->PreRender(&technique);
      scissor_state.Apply();
      RC_SetScissorRect(60, 30, 700, 400);
      for (int idx = 0; idx < idx_max; ++idx) {
        color = DG_MAKE_COLOR_ARGB(colors_argb[idx]);
        rect.Set(rect_points[idx*4], rect_points[idx*4+1], Size2(rect_points[idx*4+2], rect_points[idx*4+3]));
        rect.Translate(Point2(-10, -10)); // Offset to overlap-test on left and top area
        if ((idx%4 == 0)) g_renderer.opaque_sprite.DrawSprite(g_renderer.default_textures[Renderer::DefaultTextures_White], rect, color);
        else g_renderer.translucent_sprite.DrawSprite(g_renderer.default_textures[Renderer::DefaultTextures_White], rect, color);
      }
      g_app->view_->PostRender(&technique);
    }
    TestPresent();
  }
  scissor_state.value_ = false;
  scissor_state.Apply();
  EXPECT_TRUE(TestRenderTarget(TXT("rect")));
}

TEST(ImguiRender) {
  TEST_DEPENDS_ON(PrepareRender);
  // Imgui states
  bool checked1 = false;
  bool checked2 = false;
  bool checked3 = true;
  bool checked4 = false;
  float value1 = 50.f;
  float value2 = 30.f;
  int scrollarea1 = 0;
  int scrollarea2 = 0;
  {
    ScopedNormalTechnique technique;
    g_app->view_->PreRender(&technique);
    g_renderer.ClearScreen(DG_MAKE_COLOR_INT(255, 204, 204, 204), true /* is_depth_clear */);
    {
      LineDrawer& drawer = g_line_drawer;
      drawer.PreRender();
      {
        drawer.DrawLine(Vector3(-100.f, -100.f, -100.f), Vector3(100.0f, 100.f, 100.f), DG_MAKE_COLOR_INT(255, 255, 255, 255));
        drawer.DrawLine(Vector3(0, 0, 0), Vector3(2, 0, 0), DG_MAKE_COLOR_INT(255, 255, 0, 0));
        drawer.DrawLine(Vector3(0, 0, 0), Vector3(0, 2, 0), DG_MAKE_COLOR_INT(255, 0, 255, 0));
        drawer.DrawLine(Vector3(0, 0, 0), Vector3(0, 0, 2), DG_MAKE_COLOR_INT(255, 0, 0, 255));
      }
      drawer.PostRender();
    }
    imguiRenderBegin();
    {
      int width = g_renderer.GetScreenSize().x;
      int height = g_renderer.GetScreenSize().y;

      int mousex = 100;
      int mousey = 100;
      int mousebutton = 0;
      int mscroll = 0;

      imguiBeginFrame(mousex, mousey, mousebutton, mscroll);

      imguiBeginScrollArea("Scroll area", 10, 10, width / 5, height - 20, &scrollarea1);
      imguiSeparatorLine();
      imguiSeparator();

      imguiButton("Button");
      imguiButton("Disabled button", false);
      imguiItem("Item");
      imguiItem("Disabled item", false);
      bool toggle = imguiCheck("Checkbox", checked1);
      if (toggle)
        checked1 = !checked1;
      toggle = imguiCheck("Disabled checkbox", checked2, false);
      if (toggle)
        checked2 = !checked2;
      toggle = imguiCollapse("Collapse", "subtext", checked3);
      if (checked3)
      {
        imguiIndent();
        imguiLabel("Collapsible element");
        imguiUnindent();
      }
      if (toggle)
        checked3 = !checked3;
      toggle = imguiCollapse("Disabled collapse", "subtext", checked4, false);
      if (toggle)
        checked4 = !checked4;
      imguiLabel("Label");
      imguiValue("Value");
      imguiSlider("Slider", &value1, 0.f, 100.f, 1.f);
      imguiSlider("Disabled slider", &value2, 0.f, 100.f, 1.f, false);
      imguiIndent();
      imguiLabel("Indented");
      imguiUnindent();
      imguiLabel("Unindented");

      imguiEndScrollArea();

      imguiBeginScrollArea("Scroll area", 20 + width / 5, 500, width / 5, height - 510, &scrollarea2);
      imguiSeparatorLine();
      imguiSeparator();
      for (int i = 0; i < 100; ++i)
        imguiLabel("A wall of text");

      imguiEndScrollArea();
      imguiEndFrame();

      imguiDrawText(30 + width / 5 * 2, height - 20, IMGUI_ALIGN_LEFT, "Free text",  imguiRGBA(32,192, 32,192));
      imguiDrawText(30 + width / 5 * 2 + 100, height - 40, IMGUI_ALIGN_RIGHT, "Free text",  imguiRGBA(32, 32, 192, 192));
      imguiDrawText(30 + width / 5 * 2 + 50, height - 60, IMGUI_ALIGN_CENTER, "Free text",  imguiRGBA(192, 32, 32,192));

      imguiDrawLine(30 + width / 5 * 2, height - 80, 30 + width / 5 * 2 + 100, height - 60, 1, imguiRGBA(32,192, 32,192));
      imguiDrawLine(30 + width / 5 * 2, height - 100, 30 + width / 5 * 2 + 100, height - 80, 2, imguiRGBA(32, 32, 192, 192));
      imguiDrawLine(30 + width / 5 * 2, height - 120, 30 + width / 5 * 2 + 100, height - 100, 3, imguiRGBA(192, 32, 32,192));

      imguiDrawRoundedRect(30 + width / 5 * 2, height - 240, 100, 100, 5, imguiRGBA(32,192, 32,192));
      imguiDrawRoundedRect(30 + width / 5 * 2, height - 350, 100, 100, 10, imguiRGBA(32, 32, 192, 192));
      imguiDrawRoundedRect(30 + width / 5 * 2, height - 470, 100, 100, 20, imguiRGBA(192, 32, 32,192));

      imguiDrawRect(30 + width / 5 * 2, height - 590, 100, 100, imguiRGBA(32, 192, 32, 192));
      imguiDrawRect(30 + width / 5 * 2, height - 710, 100, 100, imguiRGBA(32, 32, 192, 192));
      imguiDrawRect(30 + width / 5 * 2, height - 830, 100, 100, imguiRGBA(192, 32, 32,192));

      imguiRenderGLDraw(width, height);
    }
    imguiRenderEnd();
    g_app->view_->PostRender(&technique);
    TestPresent();
  }
}

TEST(BoxRender) {
  TEST_DEPENDS_ON(SphereCameraWrite);
  TEST_DEPENDS_ON(TextureWrite);
  TEST_DEPENDS_ON(MaterialWrite);
  TEST_DEPENDS_ON(PrepareRender);
  // Build a mesh
  Ptr<Mesh> mesh = new Mesh;
  {
    MeshBuilder::BuildBox(*mesh, Vector3::kOne);
    {
      {
        Ptr<Chunk> chunk =
            CreateWriteChunk(TXT("dev/res/test/result/box.mesh"));
        mesh->WriteTo(chunk.ptr());
      }
      mesh = LoadResource<Mesh>(TXT("dev/res/test/result/box.mesh"));
      EXPECT_TRUE(mesh);
    }
  }
  // Build a material
  Ptr<Material> material = LoadResource<Material>(TXT("dev/res/test/result/material.mat"));
  EXPECT_TRUE(material);
  // Build a part
  Ptr<Part> part = new Part;
  {
    part->mesh_ = mesh;
    part->material_ = material;
    part->OnReadDone();
  }
  // Build a model
  Ptr<Model> model = new Model;
  {
    model->parts_.Resize(1);
    model->parts_.Get(0) = part;
    model->OnReadDone();
  }
  g_app->Update();
  ScopedNormalTechnique technique;
  g_app->view_->PreRender(&technique);
  {
    model->UpdateTransform();
    model->Submit();
  }
  g_app->view_->PostRender(&technique);
  TestPresent();
  EXPECT_TRUE(TestRenderTarget(TXT("box")));
  // Write box model
  {
    Ptr<Chunk> chunk = CreateWriteChunk(TXT("dev/res/test/result/box.mod"));
    model->WriteTo(chunk.ptr());
  }
}

TEST(SphereRender) {
  TEST_DEPENDS_ON(TextureWrite);
  TEST_DEPENDS_ON(MaterialWrite);
  TEST_DEPENDS_ON(PrepareRender);
  // Build a mesh
  Ptr<Mesh> mesh = new Mesh;
  MeshBuilder::BuildSphere(*mesh, 1.0f, 30);
  {
    Ptr<Chunk> chunk =
        CreateWriteChunk(TXT("dev/res/test/result/sphere.mesh"));
    mesh->WriteTo(chunk.ptr());
    chunk->Flush();
    mesh = LoadResource<Mesh>(TXT("dev/res/test/result/sphere.mesh"));
    EXPECT_TRUE(mesh);
  }
  // Build a material
  Ptr<Material> material = LoadResource<Material>(TXT("dev/res/test/result/material.mat"));
  EXPECT_TRUE(material);
  // Build a part
  Ptr<Part> part = new Part;
  {
    part->mesh_ = mesh;
    part->material_ = material;
    part->OnReadDone();
  }
  // Build a model
  Ptr<Model> model = new Model;
  {
    model->parts_.Resize(1);
    model->parts_.Get(0) = part;
    model->OnReadDone();
  }
  g_app->Update();
  ScopedNormalTechnique technique;
  g_app->view_->PreRender(&technique);
  {
    model->UpdateTransform();
    model->Submit();
  }
  g_app->view_->PostRender(&technique);
  TestPresent();
  EXPECT_TRUE(TestRenderTarget(TXT("sphere")));
  // Write sphere model
  {
    Ptr<Chunk> chunk = CreateWriteChunk(TXT("dev/res/test/result/sphere.mod"));
    model->WriteTo(chunk.ptr());
    chunk->Flush();
  }
}

TEST(SceneRender) {
  TEST_DEPENDS_ON(PrepareRender);
  // Load the model
  Ptr<Scene> scene = LoadResource<Scene>(TXT("res/generated/sphere_grid.pak#scene"));
  EXPECT_TRUE(scene);
  Model* model = scene->GetModel(0);
  g_app->Update();
  ScopedNormalTechnique technique;
  g_app->view_->PreRender(&technique);
  scene->UpdateTransform();
  scene->Submit();
  g_app->view_->PostRender(&technique);
  TestPresent();
  EXPECT_TRUE(TestRenderTarget(TXT("scenerender")));
}

TEST(PostEffectRender) {
  TEST_DEPENDS_ON(PrepareRender);
  Ptr<PostEffectBlur> post_effect = new PostEffectBlur;
  post_effect->Init(g_engine_config->windowed_screen_size_);
  g_app->view_->set_post_effect(post_effect.ptr());
  g_app->Update();
  g_app->Render();
  EXPECT_TRUE(TestRenderTarget(TXT("posteffect")));
  g_app->view_->set_camera(0);
}

TEST(SceneWithFontRender) {
  TEST_DEPENDS_ON(FontRender);
  TEST_DEPENDS_ON(SceneRender);
  // Load the model
  Ptr<Scene> scene = LoadResource<Scene>(TXT("res/generated/sphere_grid.pak#scene"));
  EXPECT_TRUE(scene);
  Model* model = scene->GetModel(0);
  model->local_transform_.scale_ = 0.02f;
  model->local_transform_.position_.x = 0;
  model->local_transform_.position_.y = 0;
  model->local_transform_.position_.z = 1.f;
  g_app->Update();
  ScopedNormalTechnique technique;
  g_app->view_->PreRender(&technique);
  {
    // Draw model
    {
      scene->UpdateTransform();
      scene->Submit();
    }
    // Draw text
    {
      const Cstr message[] = TXT("This test should confirm the nomal map rendering and shading with text rendering.");
      Point2 coord = Point2(16, 32);
      const ColorInt color = DG_MAKE_COLOR_INT(255, 255, 255, 255);
      g_engine.system_font.Draw(message, coord, color);
    }
  }
  g_app->view_->PostRender(&technique);
  TestPresent();
  EXPECT_TRUE(TestRenderTarget(TXT("astrowithfont")));
}

TEST(AnimationRender) {
  TEST_DEPENDS_ON(PrepareRender);
  Ptr<Model> model = LoadResource<Model>(TXT("res/fireman.pak#projecter.mod"));
  model->local_transform_.scale_ = .00008f;
  model->local_transform_.position_.z = 0.5f;
  model->UpdateTransform();
  Ptr<ModelAnimationController> anim_controller;
  Ptr<Animation> animation = LoadResource<Animation>(TXT("res/fireman.pak#animation.anim"));
  model->BindAnimation(animation.ptr());
  // Setup the animation
  {
    // Setup the animation controller
    if (animation) {
      anim_controller = new ModelAnimationController;
      anim_controller->animation_ = animation;
      anim_controller->model_ = model;
      anim_controller->sample_frames_per_second_ = g_engine_config->sample_frames_per_second_;
      anim_controller->Update(0); // Set up the start-up pose
    }
  }
  EXPECT_TRUE(model);
  g_app->Update();
  ScopedNormalTechnique technique;
  g_app->view_->PreRender(&technique);
  {
    // Draw model
    {
      model->UpdateTransform();
      model->Submit();
    }
  }
  g_app->view_->PostRender(&technique);
  TestPresent();
  EXPECT_TRUE(TestRenderTarget(TXT("animation")));
}

TEST(DeviceReset) {
  g_app->ResizeWindowSize(Size2(640, 480));
  Size2 client_rect_size = g_app->GetClientRectSize();
  g_app->ResizeScreenSize(client_rect_size);
}
