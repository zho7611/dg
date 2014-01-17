// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "batcher_first.h"

namespace dg {
namespace batcher {

void CompileShaderJob::Usage() {
  PopupMessage(TXT("Info"), TXT("Batcher.exe Usage"));
}

// Build the shader into the package along with its shader source file
// Also add the shader into the system shader map
template<class T>
static bool BuildShader(Package& package, ShaderProfile profile, Cstr* source_file_name, Cstr* shader_name) {
  // Create the read file chunk from the source shader file name
  Ptr<Chunk> chunk = LoadResource<Chunk>(source_file_name, LoadResourceOption_Read);
  NoDeleteBlob source_code;
  // Fill the source code blob from the read chunk
  source_code.SetData(chunk->data(), chunk->size());
  // Set shader chunk's resource path
  ResourcePath resource_path(package.GetResourcePath().Get(), shader_name);
  // Load/create the shader chunk
  Ptr<T> shader = LoadResource<T>(resource_path.Get(), LoadResourceOption_Write);
  // Compile the shader with the source code
  FilePath file_path(source_file_name);
  DG_LOG_LINE(TXT("compileshaderjob.buildshader: sourcefilename:%s shadername:%s"), source_file_name, shader_name);
  const Cstr* kIncludeDir = file_path.GetDir();
  if (!ShaderCompiler::CompileShader(profile, kIncludeDir, &source_code, shader.ptr())) {
    return false;
  }
  shader->shader_name_.Set(shader_name);
  // Create the shader file package for writing
  Chunk* write_chunk = package.OpenChunk(shader_name, Package::kWrite);
  // Write the shader into the package chunk
  shader->WriteTo(write_chunk);
  // Add the newly created shader into system shader map
  g_engine.AddShader(shader_name, shader.ptr());
  return true;
}

template<class T>
static Ptr<T> LoadShaderEffect(Package& package, Cstr* source_file_name, Cstr* shader_effect_name) {
  ResourcePath resource_path(package.GetResourcePath().Get(), shader_effect_name);
  Ptr<T> effect = LoadResource<T>(resource_path.Get(), LoadResourceOption_Write);
  Ptr<Chunk> chunk = LoadResource<Chunk>(source_file_name, LoadResourceOption_Read);
  bool kRet = ShaderEffectParser::ParseChunk(*chunk, effect.ptr());
  if (!kRet) {
    return NULL;
  }
  return effect;
}

static void SaveShaderEffect(Ptr<ShaderEffect> effect, Package& package) {
  const Cstr* kChunkName = effect->GetResourcePath().GetChunkName();
  Chunk* write_chunk = package.OpenChunk(kChunkName, Package::kWrite);
  effect->WriteTo(write_chunk);
}

#if defined(DG_RENDERER_GL2)
#define DG_SHADER_DIR TXT("dev/src/shaders/opengl/")
#define DG_SHADER_EFFECT TXT("dev/src/shaders/")
#else
#define DG_SHADER_DIR TXT("dev/src/shaders/")
#define DG_SHADER_EFFECT TXT("dev/src/shaders/")
#endif

bool CompileShaderJob::Run(CommandLine* command_line) {
  DG_UNREF(command_line);
  // First assure that there is no fonts.pak
  const Cstr kShaderPakFile[] = TXT("res/generated/shaders.pak");
  DG_LOG_LINE(TXT("compileshaderjob: creating:%s"), kShaderPakFile);
  // Clear the system shader map before generating all shader files
  // For now, all shaders are generated here off-line
  // That means there is no support run-time shader compilation support
  Ptr<Package> package;
  package = LoadResource<Package>(kShaderPakFile, LoadResourceOption_Write);
  g_engine.ClearShaderMap();
  package->OnReadDone();
  // Shaders for Null Technique
  {
    // Vertex shaders
    BuildShader<VertexShader>(*package, ShaderProfile_VS_30, DG_SHADER_DIR TXT("null.vsh"), TXT("NullVertexShader"));
    // Pixel shaders
    BuildShader<PixelShader>(*package, ShaderProfile_PS_30, DG_SHADER_DIR TXT("null.psh"), TXT("NullPixelShader"));
  }
  // Shaders for Simple Technique
  {
    // Vertex shaders
    BuildShader<VertexShader>(*package, ShaderProfile_VS_30, DG_SHADER_DIR TXT("simple.vsh"), TXT("SimpleVertexShader"));
    BuildShader<VertexShader>(*package, ShaderProfile_VS_30, DG_SHADER_DIR TXT("simple_depth.vsh"), TXT("SimpleDepthVertexShader"));
    BuildShader<VertexShader>(*package, ShaderProfile_VS_30, DG_SHADER_DIR TXT("simple_shadow.vsh"), TXT("SimpleShadowVertexShader"));
    // Pixel shaders
    BuildShader<PixelShader>(*package, ShaderProfile_PS_30, DG_SHADER_DIR TXT("simple_depth.psh"), TXT("SimpleDepthPixelShader"));
    BuildShader<PixelShader>(*package, ShaderProfile_PS_30, DG_SHADER_DIR TXT("simple_diffuse.psh"), TXT("SimpleDiffusePixelShader"));
    BuildShader<PixelShader>(*package, ShaderProfile_PS_30, DG_SHADER_DIR TXT("simple_specular.psh"), TXT("SimpleSpecularPixelShader"));
    BuildShader<PixelShader>(*package, ShaderProfile_PS_30, DG_SHADER_DIR TXT("simple_shadow.psh"), TXT("SimpleShadowPixelShader"));
  }
  // Shaders for Line Technique
  {
    // Vertex shaders
    BuildShader<VertexShader>(*package, ShaderProfile_VS_30, DG_SHADER_DIR TXT("line.vsh"), TXT("LineVertexShader"));
    BuildShader<VertexShader>(*package, ShaderProfile_VS_30, DG_SHADER_DIR TXT("line2.vsh"), TXT("Line2VertexShader"));
    // Pixel shaders
    BuildShader<PixelShader>(*package, ShaderProfile_PS_30, DG_SHADER_DIR TXT("line.psh"), TXT("LinePixelShader"));
  }
  // Shaders for Sprite Technique
  {
    // Vertex shaders
    BuildShader<VertexShader>(*package, ShaderProfile_VS_30, DG_SHADER_DIR TXT("sprite.vsh"), TXT("SpriteVertexShader"));
    // Pixel shaders
    BuildShader<PixelShader>(*package, ShaderProfile_PS_30, DG_SHADER_DIR TXT("sprite.psh"), TXT("SpritePixelShader"));
    BuildShader<PixelShader>(*package, ShaderProfile_PS_30, DG_SHADER_DIR TXT("sprite_font.psh"), TXT("SpriteFontPixelShader"));
  }
  // Shaders for Blur Technique
  {
    // Vertex shaders
    BuildShader<VertexShader>(*package, ShaderProfile_VS_30, DG_SHADER_DIR TXT("blur.vsh"), TXT("BlurVertexShader"));
    // Pixel shaders
    BuildShader<PixelShader>(*package, ShaderProfile_PS_30, DG_SHADER_DIR TXT("blur_down_sample.psh"), TXT("BlurDownSamplePixelShader"));
    BuildShader<PixelShader>(*package, ShaderProfile_PS_30, DG_SHADER_DIR TXT("blur_horizontal.psh"), TXT("BlurHorizontalPixelShader"));
    BuildShader<PixelShader>(*package, ShaderProfile_PS_30, DG_SHADER_DIR TXT("blur_vertical.psh"), TXT("BlurVerticalPixelShader"));
    BuildShader<PixelShader>(*package, ShaderProfile_PS_30, DG_SHADER_DIR TXT("blur_last_frame_blend.psh"), TXT("BlurLastFrameBlendPixelShader"));
    BuildShader<PixelShader>(*package, ShaderProfile_PS_30, DG_SHADER_DIR TXT("blur_original_blend.psh"), TXT("BlurOriginalBlendPixelShader"));
    BuildShader<PixelShader>(*package, ShaderProfile_PS_30, DG_SHADER_DIR TXT("blur_copy_current_frame.psh"), TXT("BlurCopyCurrentFramePixelShader"));
    BuildShader<PixelShader>(*package, ShaderProfile_PS_30, DG_SHADER_DIR TXT("blur_gamma_corrected.psh"), TXT("BlurGammaCorrectedPixelShader"));
  }
  // Shaders for Imgui Technique
  {
    // Vertex shaders
    BuildShader<VertexShader>(*package, ShaderProfile_VS_30, DG_SHADER_DIR TXT("imgui.vsh"), TXT("ImguiVertexShader"));
    // Pixel shaders
    BuildShader<PixelShader>(*package, ShaderProfile_PS_30, DG_SHADER_DIR TXT("imgui.psh"), TXT("ImguiPixelShader"));
  }
  // Shader effect
  {
    Ptr<ShaderEffect> effect;

    effect = LoadShaderEffect<SimpleShaderEffect>(*package, DG_SHADER_EFFECT TXT("simple_effect.json"), TXT("SimpleEffect"));
    SaveShaderEffect(effect, *package);

    effect = LoadShaderEffect<SpriteShaderEffect>(*package, DG_SHADER_EFFECT TXT("sprite_effect.json"), TXT("SpriteEffect"));
    SaveShaderEffect(effect, *package);

    effect = LoadShaderEffect<LineShaderEffect>(*package, DG_SHADER_EFFECT TXT("line_effect.json"), TXT("LineEffect"));
    SaveShaderEffect(effect, *package);

    effect = LoadShaderEffect<Line2ShaderEffect>(*package, DG_SHADER_EFFECT TXT("line2_effect.json"), TXT("Line2Effect"));
    SaveShaderEffect(effect, *package);

    effect = LoadShaderEffect<BlurShaderEffect>(*package, DG_SHADER_EFFECT TXT("blur_effect.json"), TXT("BlurEffect"));
    SaveShaderEffect(effect, *package);

    effect = LoadShaderEffect<ImguiShaderEffect>(*package, DG_SHADER_EFFECT TXT("imgui_effect.json"), TXT("ImguiEffect"));
    SaveShaderEffect(effect, *package);
  }
  // Write the Package resource into disk
  SaveResource(package.ptr());
  // Make the system dirty for the system to be serialized when it is closed
  g_engine.MakeDirty();
  return true;
}

} // namespace batcher
} // namespace dg
