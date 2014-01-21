// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "../gui/gui_first.h"
#include "resource.h"
#include "viewer.h"

using namespace dg;

void ViewerView::DrawPostRender() {
  // Draw the blur texture
  if (g_engine_config->is_draw_depth_texture_ && post_effect_) {
    Size2 size(128, 128);
    Rect rect(200, 10, size);
    ColorInt color = DG_MAKE_COLOR_INT(255, 255, 255, 255);
    Ptr<PostEffectBlur> blur_effect = post_effect_;
    g_renderer.opaque_sprite.DrawSprite(
        blur_effect->last_frame_blur_.texture_.ptr(), rect, color);
  }
  // Draw the shadow texture
  if (g_engine_config->is_draw_shadow_texture_ && shadow_texture_) {
    Size2 size(128, 128);
    Rect rect(400, 10, size);
    ColorInt color = DG_MAKE_COLOR_INT(128, 255, 255, 0);
    g_renderer.translucent_sprite.DrawSprite(shadow_texture_.ptr(), rect, color);
  }
  // Draw the depth texture
  if (g_engine_config->is_draw_depth_texture_) {
    Size2 size(128, 128);
    Rect rect(400, 10+128, size);
    ColorInt color = DG_MAKE_COLOR_INT(128, 255, 0, 0);
    g_renderer.translucent_sprite.DrawSprite(
        depth_target_.texture_.ptr(), rect, color);
  }
  // Draw axis
  if (g_engine_config->is_draw_axis_) {
    AxisDrawer::Draw(Vector3::kZero);
  }
  // To show light direction
  if (0) {
    g_line_drawer.PreRender();
    Vector3 origin(app_->light_camera_->view_desc.eye_pos);
    g_line_drawer.DrawLine(
        Vector3::kZero,
        origin + g_engine.light_manager->global_light_->direction_,
        DG_MAKE_COLOR_INT(255, 255, 0, 0));
    g_line_drawer.PostRender();
  }
  DrawIMGUITest();
}

void ViewerView::Render(ScopedTechnique* technique) {
  View::Render(technique);
  // Add something to draw more here
}

ViewerApp::ViewerApp() :
  Application() {
}

bool ViewerApp::Init(const Cstr* config_file) {
  Application::Init(GetApplicationName(), config_file);
  camera_ = LoadResource<SphereCamera>(g_engine_config->default_camera_.Get());
  camera_->proj_desc.aspect_ratio = float(screen_size_.x) / screen_size_.y;
  camera_->proj_desc.near_plane = 0.1f;
  camera_->proj_desc.far_plane = 40.0f;
  camera_->height_ = 0.5f;
  camera_->UpdateTransform();
  {
    Check(!light_camera_);
    light_camera_ = new Camera;
    light_camera_->is_light_camera = true;
    light_camera_->is_orthogonal = true;
    light_camera_->view_desc.eye_pos.Set(0, 0, 10.0f);
    light_camera_->view_desc.target_pos.Set(0, 0, 0);
    light_camera_->view_desc.up_dir.Set(1.0f, 0, 0);
    light_camera_->ortho_desc.Init(.1f, 40.0f);
    light_camera_->UpdateTransform();
  }
  // Setup scene
  Ptr<Scene> scene = new Scene;
  scene->OnReadDone();
  // Setup view
  Ptr<ViewerView> viewer_view = new ViewerView;
  viewer_view->set_app(this);
  SetupDefaultView(scene.ptr(), viewer_view.ptr(), camera_.ptr(), light_camera_.ptr());
  // Override the background color
  viewer_view->clear_color.Set(.5f, .5f, .5f, 1.0f);
  // Setup light view
  Ptr<View> light_view = new View;
  SetupDefaultLightView(scene.ptr(), light_view.ptr(), light_camera_.ptr());
  // Set shadow texture
  if (light_view && light_view->post_effect()) {
    RenderTarget* target = light_view->post_effect()->render_target();
    Check(target->texture_);
    viewer_view->set_shadow_texture(target->texture_.ptr());
  }
  // Update light direction
  g_engine.light_manager->global_light_->direction_.NormalizeFrom(
      light_camera_->GetEyeReference());
  // Add ground plane(as box)
  ground_ = model_util::CreateBoxModel(
      &Vector3(0, 0, 0), &Vector3(5.f, 5.f, 0.00001f),
      &Vector4(1.f, 1.f, 1.f, 1.f),
      &Vector4(.2f, .2f, .2f, 1.f));
  actor_ = NULL;
  actor_ = LoadActor1();
  if (ground_) viewer_view->scene()->AddModel(ground_.ptr());
  if (actor_ && actor_->model) viewer_view->scene()->AddModel(actor_->model.ptr());
  return true;
}

void ViewerApp::Clear() {
  shadow_camera_controller_ = NULL;
  animation_ = NULL;
  animation_controller_ = NULL;
  DG_SAFE_DELETE(actor_);
  ground_ = NULL;
  light_camera_ = NULL;
  camera_ = NULL;
  Application::Clear();
}

ViewerActor* ViewerApp::LoadActor1() {
  DG_SAFE_DELETE(actor_);
  actor_ = new ViewerActor;
  actor_->model = LoadResource<Model>(TXT("res/zhom.pak#body.mod"));
  Check(actor_->model);
  actor_->yaw_pitch_roll = Vector3::kZero;
  // Load animations
  if (0) {
    Ptr<Animation> animation = LoadResource<Animation>(TXT("res/zhom.pak#animation.anim"));
    actor_->model->BindAnimation(animation.ptr());
    animation_controller_ = new ModelAnimationController;
    animation_controller_->animation_ = animation;
    animation_controller_->model_ = actor_->model;
    animation_controller_->Play();
    animation_controller_->Update(0);
  }
  actor_->model->UpdateTransform();
  return actor_;
}

ViewerActor* ViewerApp::LoadActor2() {
  DG_SAFE_DELETE(actor_);
  actor_ = new ViewerActor;
  actor_->model = LoadResource<Model>(TXT("dev/res/test/result/box.mod"));
  Check(actor_->model);
  actor_->yaw_pitch_roll = Vector3::kZero;
  actor_->model->local_transform_.position_.z = 0.5f;
  actor_->model->local_transform_.scale_ = 0.5f;
  actor_->model->UpdateTransform();
  animation_controller_ = NULL;
  return actor_;
}

ViewerActor* ViewerApp::LoadActor3() {
  DG_SAFE_DELETE(actor_);
  actor_ = new ViewerActor;
  actor_->model = LoadResource<Model>(TXT("dev/res/test/result/sphere.mod"));
  Check(actor_->model);
  actor_->yaw_pitch_roll = Vector3::kZero;
  actor_->model->local_transform_.position_.z = 0.5f;
  actor_->model->local_transform_.scale_ = 0.25f;
  actor_->model->UpdateTransform();
  animation_controller_ = NULL;
  return actor_;
}

void ViewerApp::OnMouseUpdate(int x, int y, MouseButtonState button_state) {
  if (button_state & MouseButtonState_RightButton) {
    camera_->AddYaw(g_engine.mouse.delta_x*.01f);
    camera_->AddPitch(g_engine.mouse.delta_y*.01f);
  }
}

void ViewerApp::OnMouseWheel(int deltaZ) {
  float sensitivity = 0.002f;
  if (IsKeyPressed(KeyCode_LeftShift)) {
    sensitivity = 0.0001f;
  }
  camera_->AddRadius(-(float)deltaZ*sensitivity);
}

void ViewerApp::OnKeyDown(KeyCode code) {
  switch (code) {
  case 'W':
    g_renderer.is_wire_mode = !g_renderer.is_wire_mode;
    if (view_->post_effect()) {
      view_->post_effect()->is_enabled_ = !g_renderer.is_wire_mode;
    }
    break;
  case KeyCode_Space:
    if (animation_controller_) {
      animation_controller_->FlipPlayPause();
    }
    break;
  case KeyCode_Up:
    camera_->height_ += .1f;
    break;
  case KeyCode_Down:
    camera_->height_ -= .1f;
    break;
  }
}

// This is called after the application window is resized
void ViewerApp::OnSize(int width, int height) {
  if (width == 0 && height == 0) return;
  Application::OnSize(width, height);
  if (camera_) {
    camera_->proj_desc.aspect_ratio =
        float(screen_size_.x) / float(screen_size_.y);
  }
}

void ViewerApp::PreRender() {
  // Update animation
  if (animation_controller_) {
    const float kDeltaTime = g_renderer.timer.GetDeltaInMilliSeconds();
    // Animation controller update
    animation_controller_->Update(kDeltaTime);
  }
  // Update model
  if (actor_ && actor_->model) {
    actor_->model->local_transform_.rotation_.StoreRotationYawPitchRoll(
        actor_->yaw_pitch_roll.x*kDegreeToRadian,
        actor_->yaw_pitch_roll.y*kDegreeToRadian,
        actor_->yaw_pitch_roll.z*kDegreeToRadian);
    actor_->model->UpdateTransform();
  }
  Application::PreRender();
}

void ViewerApp::Render() {
  Application::Render();
}

void ViewerView::DrawIMGUITest() {
  // Draw imgui ui
  imguiRenderBegin();
  {
    int width = g_renderer.GetScreenSize().x;
    int height = g_renderer.GetScreenSize().y;

    unsigned char mousebutton = 0;
    int mscroll = 0;
    int mousex; int mousey;
    mousex = (int)g_engine.mouse.x;
    mousey = (int)(height - g_engine.mouse.y); // Height - mousey;
    int leftButton = (g_engine.mouse.button_state == MouseButtonState_LeftButton);
    int rightButton = (g_engine.mouse.button_state == MouseButtonState_RightButton);
    int middleButton = (g_engine.mouse.button_state == MouseButtonState_MiddleButton);
    int toggle = 0;

    if (leftButton)
      mousebutton |= IMGUI_MBUT_LEFT;

    // Imgui states
    static bool checked1 = false;
    static bool checked2 = false;
    static bool checked3 = true;
    static bool checked4 = false;
    static float value1 = 50.f;
    static float value2 = 30.f;
    static int scrollarea1 = 0;
    static int scrollarea2 = 0;
    static int selected_model = 0;

    imguiBeginFrame(mousex, mousey, mousebutton, mscroll);

    imguiBeginScrollArea("Property", 10, 10, width / 5, height - 20, &scrollarea1);

    imguiSeparatorLine();
    imguiLabel("Select Model");
    toggle = imguiCheck("Actor1", selected_model == 0);
    ViewerActor* actor = app_->actor_;
    if (toggle) {
      if (selected_model != 0) {
        selected_model = 0;
        actor = app_->LoadActor1();
      }
    }
    toggle = imguiCheck("Actor2", selected_model == 1);
    if (toggle) {
      if (selected_model != 1) {
        selected_model = 1;
        actor = app_->LoadActor2();
      }
    }
    toggle = imguiCheck("Actor3", selected_model == 2);
    if (toggle) {      
      if (selected_model != 2) {
        selected_model = 2;
        actor = app_->LoadActor3();
      }
    }
    if (actor) {
      scene_->RemoveAll();
      if (app_->ground_) scene_->AddModel(app_->ground_.ptr());
      if (actor->model) scene_->AddModel(actor->model.ptr());
    }
    
    imguiSeparatorLine();
    imguiLabel("Animation");
    const char* msg;
    if (app_->animation_controller_ && app_->animation_controller_->is_playing())
      msg = "Stop";
    else msg = "Play";
    if (imguiButton(msg, (app_->animation_controller_) ? true : false)) {
      if (app_->animation_controller_)
        app_->animation_controller_->FlipPlayPause();
    }

    if (0) {
      imguiSeparatorLine();
      toggle = imguiCheck("Post Effect", post_effect_->is_enabled_);
      if (toggle) {
        post_effect_->is_enabled_ = !post_effect_->is_enabled_;
      }
    }

    // Transform model
    static bool is_transform_expanded = true;
    bool is_toggle_transform = imguiCollapse("Transform", NULL, is_transform_expanded);
    if (is_transform_expanded && actor && actor->model) {
      imguiIndent();
      Transform& tm = actor->model->local_transform_;
      imguiSlider("Position X", &tm.position_.x, -3.0f, 3.0f, 0.001f);
      imguiSlider("Position Y", &tm.position_.y, -3.0f, 3.0f, 0.001f);
      imguiSlider("Position Z", &tm.position_.z, -3.0f, 3.0f, 0.001f);
      imguiSlider("Rotate Yaw", &actor->yaw_pitch_roll.x, -180.0f, 180.0f, kDegreeToRadian);
      imguiSlider("Rotate Pitch", &actor->yaw_pitch_roll.y, -180.0f, 180.0f, kDegreeToRadian);
      imguiSlider("Rotate Roll", &actor->yaw_pitch_roll.z, -180.0f, 180.0f, kDegreeToRadian);
      imguiUnindent();
    }
    if (is_toggle_transform)
      is_transform_expanded = !is_transform_expanded;
    // Global light dir
    static bool is_light_expanded = true;
    bool is_toggle_light = imguiCollapse("Light", NULL, is_light_expanded);
    if (is_light_expanded && g_engine.light_manager->global_light_) {
      imguiIndent();
      imguiSlider("Light Brightness", &g_engine_config->global_light_brightness_, 0.0, 10.0f, 0.001f);
      imguiSlider("Bloom Weight", &g_engine_config->global_bloom_weight_, 0.0, 2.0f, 0.001f);
      imguiSlider("Gamma Correction", &g_engine_config->global_gamma_correction_, 0.0, 10.0f, 0.001f);
      imguiSeparatorLine();
      {
        Vector3 direction(.0f, -1.0f, 1.0f);
        direction.Normalize();
        Quaternion rotation;
        static Vector3 s_yaw_pitch_roll;
        Vector3 yaw_pitch_roll = s_yaw_pitch_roll*kDegreeToRadian;
        rotation.StoreRotationYawPitchRoll(yaw_pitch_roll.x, yaw_pitch_roll.y, yaw_pitch_roll.z);
        Matrix transform;
        direction.StoreTransformNormal(direction, transform.StoreRotationQuaternion(rotation));
        imguiSlider("Light Direction", &s_yaw_pitch_roll.z, -360.0f, 360.0f, 0.001f);
        g_engine.light_manager->global_light_->direction_ = direction;
      }
      imguiSeparatorLine();
      imguiSlider("Light Ambient R", &g_engine.light_manager->global_light_->ambient_color_.r, 0.0, 2.0f, 0.001f);
      imguiSlider("Light Ambient G", &g_engine.light_manager->global_light_->ambient_color_.g, 0.0, 2.0f, 0.001f);
      imguiSlider("Light Ambient B", &g_engine.light_manager->global_light_->ambient_color_.b, 0.0, 2.0f, 0.001f);
      imguiSeparatorLine();
      imguiSlider("Light Diffuse R", &g_engine.light_manager->global_light_->diffuse_color_.r, 0.0, 2.0f, 0.001f);
      imguiSlider("Light Diffuse G", &g_engine.light_manager->global_light_->diffuse_color_.g, 0.0, 2.0f, 0.001f);
      imguiSlider("Light Diffuse B", &g_engine.light_manager->global_light_->diffuse_color_.b, 0.0, 2.0f, 0.001f);
      imguiUnindent();
    }
    if (is_toggle_light)
      is_light_expanded = !is_light_expanded;
    static bool is_material_expanded = true;
    char num_material_label[256];
    if (actor && actor->model) {
      sprintf(num_material_label, "# : %d", actor->model->parts_.Count());
    } else {
      sprintf(num_material_label, "");
    }
    bool is_toggle_material = imguiCollapse("Material", num_material_label, is_material_expanded);
    if (is_material_expanded && actor && actor->model) {
      imguiIndent();
      Model* model = actor->model.ptr();
      for (int idx = 0; idx < model->parts_.Count(); ++idx) {
        Part* part = model->parts_.Get(idx).ptr();
        imguiSeparatorLine();
        char label[256];
        sprintf(label, "Part(%d)", idx);
        imguiLabel(label);
        if (part->material_) {
          ShaderParameter* p = part->material_->GetParameterByName(TXT("GlobalMaterialAmbient"));
          if (p) {
            Vector4 ambient = p->GetVector4();
            imguiSlider("Ambient R", &ambient.x, 0.0, 2.0f, 0.01f);
            imguiSlider("Ambient G", &ambient.y, 0.0, 2.0f, 0.01f);
            imguiSlider("Ambient B", &ambient.z, 0.0, 2.0f, 0.01f);
            p->SetVector4(&ambient);
          }
          p = part->material_->GetParameterByName(TXT("GlobalMaterialDiffuse"));
          if (p) {
            Vector4 diffuse = p->GetVector4();
            imguiSlider("Diffuse R", &diffuse.x, 0.0, 2.0f, 0.01f);
            imguiSlider("Diffuse G", &diffuse.y, 0.0, 2.0f, 0.01f);
            imguiSlider("Diffuse B", &diffuse.z, 0.0, 2.0f, 0.01f);
            p->SetVector4(&diffuse);
          }
          p = part->material_->GetParameterByName(TXT("GlobalMaterialSpecular"));
          if (p) {
            Vector4 specular = p->GetVector4();
            imguiSlider("Specular Intensity R", &specular.x, 0.0, 2.0f, 0.01f);
            imguiSlider("Specular Intensity G", &specular.y, 0.0, 2.0f, 0.01f);
            imguiSlider("Specular Intensity B", &specular.z, 0.0, 2.0f, 0.01f);
            imguiSlider("Specular Power", &specular.w, 0.0, 100.0f, 0.01f);
            p->SetVector4(&specular);
          }
        }
      }
      imguiUnindent();
    }
    if (is_toggle_material) {
      is_material_expanded = !is_material_expanded;
    }
    
    imguiEndScrollArea();

    imguiEndFrame();

    imguiRenderGLDraw(width, height);
  }
  imguiRenderEnd();
}

// Usage: Viewer.exe [config-file]
// Ex. Viewer.exe Config/Fireman.ini
int dg::ApplicationMain() {
  ViewerApp app;
  const Cstr* config_file = NULL;
  CommandLine command_line;
  if (command_line.NumOptions() > 1) {
    config_file = command_line.GetOption(1);
  }
  if (app.Init(config_file)) {
    app.Run();
    app.Clear();
  }
  return 0;
}
