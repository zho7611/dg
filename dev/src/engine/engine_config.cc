// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

EngineConfig* dg::g_engine_config = NULL;

EngineConfig::EngineConfig() {
  model_scale_ = 1.f;
  global_light_brightness_ = 1.f;
#if defined(DG_RENDERER_GL2)
  // In gl2 renderer, gamma correction is not supported for now
  global_gamma_correction_ = 1.0f;
#else
  global_gamma_correction_ = 2.2f;
#endif
  global_bloom_weight_ = 0.75f;
  is_rotate_camera_ = false;
  is_post_process_blur_ = true;
}

EngineConfig::~EngineConfig() {}

bool EngineConfig::LoadFrom(IniFile* ini_file) {
  Check(ini_file);
  if (!Config::LoadFrom(ini_file)) {
    return false;
  }
  // Engine
  {
    ini_file->GetValue(TXT("Engine"), TXT("SystemFontFace"), system_font_face_);
  }
  // Path
  {
    ini_file->GetValue(TXT("Path"), TXT("GlobalEffectPath"), global_effect_path_);
    ini_file->GetValue(TXT("Path"), TXT("SpriteEffectPath"), sprite_effect_path_);
    ini_file->GetValue(TXT("Path"), TXT("LineEffectPath"), line_effect_path_);
    ini_file->GetValue(TXT("Path"), TXT("Line2EffectPath"), line2_effect_path_);
    ini_file->GetValue(TXT("Path"), TXT("BlurEffectPath"), blur_effect_path_);
    ini_file->GetValue(TXT("Path"), TXT("DefaultEffectPath"), default_effect_path_);
    ini_file->GetValue(TXT("Path"), TXT("ImguiEffectPath"), imgui_effect_path_);
  }
  // Shader
  {
    ini_file->GetValue(TXT("Shader"), TXT("GlobalLightBrightness"), global_light_brightness_);
    ini_file->GetValue(TXT("Shader"), TXT("GlobalGammaCorrection"), global_gamma_correction_);
    ini_file->GetValue(TXT("Shader"), TXT("GlobalBloomWeight"), global_bloom_weight_);
  }
  // Scene
  {
    ini_file->GetValue(TXT("Scene"), TXT("DefaultScene"), default_scene_);
    ini_file->GetValue(TXT("Scene"), TXT("DefaultCamera"), default_camera_);
    ini_file->GetValue(TXT("Scene"), TXT("DefaultModel"), default_model_);
    ini_file->GetValue(TXT("Scene"), TXT("DefaultAnimation"), default_animation_);
    ini_file->GetValue(TXT("Scene"), TXT("ModelScale"), model_scale_);
    ini_file->GetValue(TXT("Scene"), TXT("CameraHeight"), camera_height_);
    {
      String color_str;
      if (ini_file->GetValue(TXT("Scene"), TXT("BackgroundColor"), color_str)) {
        background_color_ = color_str.ToUintHexadecimal();
        // Config file is written as ARGB format
#if defined(DG_USE_COLOR_INT_AS_ABGR)
        // ARGB to ABGR (swap R and B)
        int temp = (background_color_ >> 16) & 0xff;
        background_color_ = (background_color_&0xff00ff00) | (temp) | ((background_color_&0xff)<<16);
#endif
      }
    }
  }
  // Shadow map
  {
    int shadow_map_width(0), shadow_map_height(0);
    bool has_width = ini_file->GetValue(TXT("Renderer"), TXT("ShadowMapWidth"), shadow_map_width);
    bool has_height = ini_file->GetValue(TXT("Renderer"), TXT("ShadowMapHeight"), shadow_map_height);
    if (has_width && has_height) {
      shadow_map_size_.Set(shadow_map_width, shadow_map_height);
    }
  }
  // Windowed screen size
  {
    int width(0), height(0);
    bool has_width = ini_file->GetValue(TXT("Renderer"), TXT("WindowedScreenWidth"), width);
    bool has_height = ini_file->GetValue(TXT("Renderer"), TXT("WindowedScreenHeight"), height);
    if (has_width && has_height) {
      windowed_screen_size_.Set(width, height);
    }
  }
  // Full-screen screen size
  {
    int width(0), height(0);
    bool has_width = ini_file->GetValue(TXT("Renderer"), TXT("FullscreenScreenWidth"), width);
    bool has_height = ini_file->GetValue(TXT("Renderer"), TXT("FullscreenScreenHeight"), height);
    if (has_width && has_height) {
      full_screen_screen_size_.Set(width, height);
    }
  }
  // Renderer
  {
    ini_file->GetValue(TXT("Renderer"), TXT("IsFullscreen"), is_full_screen_);
    ini_file->GetValue(TXT("Renderer"), TXT("IsShadowEnabled"), is_shadow_enabled_);
    ini_file->GetValue(TXT("Renderer"), TXT("IsDrawFrameRate"), is_draw_frame_rate_);
    ini_file->GetValue(TXT("Renderer"), TXT("IsDrawDepthTexture"), is_draw_depth_texture_);
    ini_file->GetValue(TXT("Renderer"), TXT("IsDrawShadowTexture"), is_draw_shadow_texture_);
    ini_file->GetValue(TXT("Renderer"), TXT("IsDrawSkeleton"), is_draw_skeleton_);
    ini_file->GetValue(TXT("Renderer"), TXT("IsDrawAxis"), is_draw_axis_);
    ini_file->GetValue(TXT("Renderer"), TXT("IsPostProcessBlur"), is_post_process_blur_);
    ini_file->GetValue(TXT("Renderer"), TXT("IsRotateCamera"), is_rotate_camera_);
  }
  // Animation
  ini_file->GetValue(TXT("Animation"), TXT("SampleFramesPerSeconds"), sample_frames_per_second_);
  return true;
}

} // namespace dg
