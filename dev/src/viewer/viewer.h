// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

namespace dg {

class ViewerView : public View {
public:
  void set_shadow_texture(Texture* tex) {
    shadow_texture_ = tex;
  }
  void set_app(class ViewerApp* app) {
    app_ = app;
  }

private:
  class ViewerApp* app_;
  Ptr<Texture> shadow_texture_;

public:
  ViewerView() {}
  ~ViewerView() {}

  virtual void Render(ScopedTechnique* technique);

  void DrawIMGUITest();

  virtual void DrawPostRender();
};

struct ViewerActor : public RefCount {
public:
  Ptr<Model> model;
  Vector3 yaw_pitch_roll;
};

class ViewerApp : public Application {
public:
  ViewerApp();
  virtual const Cstr* GetApplicationName() const {
    return TXT("Viewer");
  }
  virtual bool Init(const Cstr* config_file);
  virtual void Clear();
  virtual void PreRender();
  virtual void Render();
  virtual void OnMouseUpdate(int x, int y, MouseButtonState button_state);
  virtual void OnMouseWheel(int deltaZ);
  virtual void OnKeyDown(KeyCode code);
  virtual void OnSize(int width, int height);
  ViewerActor* LoadActor1();
  ViewerActor* LoadActor2();
  ViewerActor* LoadActor3();

  Ptr<ViewerActor> actor_;
  Ptr<Model> ground_;
  Ptr<ModelAnimationController> animation_controller_;
  Ptr<Camera> light_camera_;

private:
  Ptr<Animation> animation_;
  Ptr<SphereCamera> camera_;
  Ptr<CameraModelFollowController> shadow_camera_controller_;
};

} // namespace dg
