// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class SystemFontText {
public:
  SystemFontText() : font_face_(0) {}
  void Init();
  void Clear();
  void Draw(const Cstr* text, Point2& position, ColorInt color);

private:
  String text_;
  FontFace* font_face_;
  Ptr<Texture> texture_;
};

class Engine {
public:
  enum Usage {
      Usage_None = 0,
      Usage_Renderer = 1,
      Usage_FontManager = 1 << 4,
      Usage_All =
          Usage_Renderer |
          Usage_FontManager,
  };

  Engine();
  ~Engine();
  bool Init(Usage usage = Usage_All, const Cstr* configPath = 0);
  void Clear();
  void ReadFrom(Stream* stream);
  void WriteTo(Stream* out_stream);
  void MakeDirty() {
    is_system_serialized_ = false;
  }
  Shader* FindShader(const Cstr* shader_name);
  void AddShader(const Cstr* shader_name, VertexShader* shader) {
    vertex_shader_map_.AddShader(shader_name, shader);
  }
  void AddShader(const Cstr* shader_name, PixelShader* shader) {
    pixel_shader_map_.AddShader(shader_name, shader);
  }
  void ClearShaderMap() {
    vertex_shader_map_.Clear();
    pixel_shader_map_.Clear();
  }
  virtual void RegisterClasses();

  SystemFontText system_font;
  class LightManager* light_manager;
  class ResourceManager* resource_manager;
  class Statistics* statistics;
  Mouse mouse;

private:
  ShaderMap<VertexShader> vertex_shader_map_;
  ShaderMap<PixelShader> pixel_shader_map_;
  Usage usage_;
  bool is_system_serialized_;
  bool is_system_deserialized_;
  static Engine* s_instance_;

  void InitWorkingDir(bool is_use_module_dir = false);
  void ChangeWorkingDir();
  void SerializeSystem();
  void DeserializeSystem();
};

extern Engine g_engine;

} // namespace dg
