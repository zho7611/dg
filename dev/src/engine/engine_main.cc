// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

Engine* Engine::s_instance_ = NULL;
Engine dg::g_engine;

void SystemFontText::Init() {
  Check(font_face_ == 0);
  font_face_ = FontFace::GetFontFace(g_engine_config->system_font_face_.Get());
}

void SystemFontText::Clear() {
  font_face_ = 0;
  texture_ = 0;
}

void SystemFontText::Draw(const Cstr* text, Point2& position, ColorInt color) {
  if (!text) {
    return;
  }
  if (!texture_) {
    texture_ = new Texture;
    texture_->Init(GpuUsage_Dynamic, Size2(512, 32), GpuFormat_A8);
  }
  Check(font_face_);
  {
    text_.Set(text);
    font_face_->RenderText(
        texture_.ptr(),
        text_.Get(),
        position);
  }
  Rect rect(texture_->size());
  g_renderer.font_sprite.DrawSprite(texture_.ptr(), rect, color);
}

Engine::Engine() :
    usage_(Usage_None),
    is_system_serialized_(false),
    is_system_deserialized_(false),
    resource_manager(NULL),
    light_manager(NULL) {
  Check(!s_instance_);
  memory_pool::InitMemoryPool();
}

Engine::~Engine() {
  memory_pool::ClearMemoryPool();
  Check(light_manager == NULL); // Should be deleted in Clear()
  Check(resource_manager == NULL);
  Check(!s_instance_);
}

void Engine::RegisterClasses() {
  dg::RegisterClasses();
}

void Engine::InitWorkingDir(bool is_use_module_dir) {
  if (is_use_module_dir) {
    String theDir;
    Os::GetModuleDirectory(theDir);
    Os::SetWorkingDirectory(theDir.Get());
  }
}

void Engine::ChangeWorkingDir() {
  Os::SetWorkingDirectory(g_engine_config->working_dir_.Get());
}

bool Engine::Init(Usage usage, const Cstr* config_path) {
  Check(!s_instance_);
  s_instance_ = this;
  CoreInit();
  usage_ = usage;
  InitWorkingDir(true);
  const Cstr default_config_path[] = TXT(".\\config\\Common.ini");
  config_path = config_path ? config_path : default_config_path;
  Check(!g_config);
  g_config = g_engine_config = new EngineConfig;
  if (!g_engine_config->Open(config_path)) {
    DG_LOG_LINE(TXT("engine.open: config-path:%s"), config_path);
    return false;
  }
  ChangeWorkingDir();
  // Now it's safe to log after the following line
  if (!g_logger->Open(g_engine_config->log_path_.Get())) {
    // Logging failed
    // There might be another application running just ignore for now, but logging is disabled
  }
  // Print out the first system log with command line arguments
  {
    CommandLine command_line;
    // ===...
    for (int idx = 0; idx < 80; ++idx) {
      DG_LOG(TXT("="));
    }
    DG_LOG(string_util::kLineEndStr);
    DG_LOG_LINE(TXT("engine.init.begin-log"));
    String cmdline_string;
    for(int idx = 0; idx < command_line.NumOptions(); idx++) {
      String param_string;
      param_string.Format(TXT("%s "), command_line.GetOption(idx));
      cmdline_string.AddRight(param_string);
    }
    DG_LOG_LINE(TXT("engine.init.commandline:%s"), cmdline_string.Get());
  }
  // Print out the working dir
  String working_dir;
  Os::GetWorkingDirectory(working_dir);
  DG_LOG_LINE(TXT("engine.init.working-dir:%s"), working_dir.Get());
  DG_LOG_LINE(TXT("engine.init.begin: config-path:%s"), config_path);
  Check(statistics == NULL);
  statistics = new Statistics;
  Check(resource_manager == NULL);
  resource_manager = new class ResourceManager;
  resource_manager->Init();
  RegisterClasses();
  if (usage_ & Usage_Renderer) {
    g_renderer.Init();
  }
  DeserializeSystem();
  if (usage_ & Usage_FontManager) {
    Check(is_system_deserialized_);
    g_font_manager.Init();
    system_font.Init();
  }
  {
    Check(light_manager == NULL);
    light_manager = new LightManager;
    light_manager->Init();
  }
  g_shadow_manager.Init();
  DG_LOG_LINE(TXT("engine.init.end:"));
  return true;
}

void Engine::Clear() {
  DG_LOG_LINE(TXT("engine.clear.begin:"));
  g_shadow_manager.Clear();
  {
    light_manager->Clear();
    DG_SAFE_DELETE(light_manager);
  }
  SerializeSystem();
  // Clear the shader maps explicitly
  vertex_shader_map_.Clear();
  pixel_shader_map_.Clear();
  if (usage_ & Usage_FontManager) {
    Check(is_system_serialized_);
    system_font.Clear();
    g_font_manager.Clear();
  }
  if (usage_ & Usage_Renderer) {
    g_renderer.Clear();
  }
  resource_manager->Clear();
  DG_SAFE_DELETE(resource_manager);
  DG_SAFE_DELETE(statistics);
  if (0) memory_pool::ReportMemoryPool();
  DG_LOG_LINE(TXT("engine.clear.end:"));
  g_logger->Close();
  g_engine_config->Close();
  DG_SAFE_DELETE(g_config);
  s_instance_ = NULL;
  CoreClear();
}

void Engine::DeserializeSystem() {
  Check(!is_system_deserialized_);
  if (File::IsFound(g_engine_config->system_file_path_.Get())) {
    Ptr<Chunk> chunk = CreateReadChunk(g_engine_config->system_file_path_.Get());
    ReadFrom(chunk.ptr());
  } else {
    DG_LOG_LINE(TXT("error: deserialize-system.failed: engine.system-file-path.not-exist"));
  }
  is_system_deserialized_ = true;
}

void Engine::SerializeSystem() {
  Check(!is_system_serialized_);
  Ptr<Chunk> chunk = CreateWriteChunk(g_engine_config->system_file_path_.Get());
  if (chunk) {
    WriteTo(chunk.ptr());
  }
  is_system_serialized_ = true;
}

void Engine::ReadFrom(Stream* stream) {
  g_name_pool->ReadFrom(stream);
  g_font_manager.ReadFrom(stream);
  StreamReadClass(stream, vertex_shader_map_);
  StreamReadClass(stream, pixel_shader_map_);
}

void Engine::WriteTo(Stream* out_stream) {
  g_name_pool->WriteTo(out_stream);
  g_font_manager.WriteTo(out_stream);
  StreamWriteClass(out_stream, vertex_shader_map_);
  StreamWriteClass(out_stream, pixel_shader_map_);
}

Shader* Engine::FindShader(const Cstr* shader_name) {
  VertexShader* foundVertexShader = vertex_shader_map_.FindShader(shader_name);
  if (foundVertexShader) {
    return foundVertexShader;
  }
  PixelShader* foundPixelShader = pixel_shader_map_.FindShader(shader_name);
  return foundPixelShader;
}

} // namespace dg
