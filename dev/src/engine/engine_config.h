// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class EngineConfig : public Config {
public:
  EngineConfig();
  ~EngineConfig();

  String global_effect_path_;
  String default_effect_path_;
  String sprite_effect_path_;
  String line_effect_path_;
  String line2_effect_path_;
  String blur_effect_path_;
  String imgui_effect_path_;
  String shader_package_path_;
  String system_font_face_;
  String default_model_;
  String default_animation_;
  String default_camera_;
  String default_scene_;
  ColorInt background_color_;
  float model_scale_;
  float camera_height_;
  float global_light_brightness_;
  float global_gamma_correction_;
  float global_bloom_weight_;
  Size2 shadow_map_size_;
  Size2 windowed_screen_size_;
  Size2 full_screen_screen_size_;
  bool is_full_screen_;
  bool is_shadow_enabled_;
  float sample_frames_per_second_;
  bool is_draw_frame_rate_;
  bool is_draw_depth_texture_;
  bool is_draw_shadow_texture_;
  bool is_draw_skeleton_;
  bool is_draw_axis_;
  bool is_post_process_blur_;
  bool is_rotate_camera_;

protected:
  virtual bool LoadFrom(IniFile* ini_file);
};

extern EngineConfig* g_engine_config;

} // namespace dg
