// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class Camera;
class PostEffect;
class Scene;

class View : public Resource {
public:
  ColorRGBA clear_color;
  bool is_enabled;
  Size2 view_size;

  View();
  virtual ~View();
  virtual void Init();
  void Clear();
  virtual void ReadFrom(Stream* stream);
  virtual void WriteTo(Stream* out_stream);
  void Update();
  void CreateView();
  void DestroyView();
  virtual void PreRender(ScopedTechnique* technique);
  virtual void Render(ScopedTechnique* technique);
  virtual void PostRender(ScopedTechnique* technique);
  RenderTarget* render_target();
  void ResetSize(const Size2& view_size);
  virtual void DrawPostRender();
  void SetCascadeIndex(int cascade_index);
  Camera* camera() { return camera_.ptr(); }
  void set_camera(Camera* camera) { camera_ = camera; }
  PostEffect* post_effect() { return post_effect_.ptr(); }
  void set_post_effect(PostEffect* post_effect) { post_effect_ = post_effect; }
  Camera* light_camera() { return light_camera_.ptr(); }
  void set_light_camera(Camera* light_camera) { light_camera_ = light_camera; }
  Scene* scene() { return scene_.ptr(); }
  void set_scene(Scene* scene) { scene_ = scene; }
  RenderTarget* depth_target() { return &depth_target_; }

protected:
  Ptr<Camera> camera_;
  Ptr<PostEffect> post_effect_;
  Ptr<Camera> light_camera_;
  Ptr<Scene> scene_;
  RenderTarget depth_target_;

  DG_DECLARE_RESOURCE(View, Resource);
};

} // namespace dg
