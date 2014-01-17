// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

template<class T>
class ShaderMap {
protected:
  typedef Pair<String, String> InternalShaderType;
  typedef List<InternalShaderType > InternalShadersType;
  typedef StringMap<Ptr<T> > ShadersType;

public:
  void Clear() {
    shaders_.Clear();
  }

  bool AddShader(const Cstr* shader_name, T* shader) {
    if (shaders_.IsFound(shader_name)) {
      return false;
    }
    Ptr<T> shaderPtr(shader);
    shaders_.Add(shader_name, shaderPtr);
    return true;
  }

  T* FindShader(const Cstr* shader_name) {
    Ptr<T> found;
    if (shaders_.Find(shader_name, found)) {
      return found.ptr();
    }
    return NULL;
  }

  void ReadFrom(Stream* stream) {
    // Read the internal shaders
    InternalShadersType shaders;
    StreamReadClass(stream, shaders);
    // Build the shaders_ from the internal shaders
    InternalShadersType::Iterator it = shaders.Begin();
    InternalShadersType::Iterator ie = shaders.End();
    for (; it != ie; ++it) {
      // Get the current internal shader item
      InternalShaderType& shader = *it;
      // Load the actual shader
      const Cstr* shaderPath = shader.second_.GetConst();
      Ptr<T> loadedShader = LoadResource<T>(shaderPath);
      Check(loadedShader);
      // Add the shader into the shader map
      shaders_.Add(shader.first_, loadedShader);
      // Print what we are doing
      if (0) {
        DG_LOG_LINE(TXT("shadermap.read: shadername:%s resourcepath:%s"),
            shader.first_.GetConst(),
            shader.second_.GetConst());
      }
    }
  }

  void WriteTo(Stream* out_stream) {
    InternalShadersType shaders;
    // Build the shaders
    typename ShadersType::Iterator it = shaders_.Begin();
    typename ShadersType::Iterator ie = shaders_.End();
    for (; it != ie; ++it) {
      // Get the current item
      String shader_name = it.Key();
      Ptr<T> shaderPtr = it.Value();
      // Build the internal shader item
      InternalShaderType shader;
      shader.first_ = shader_name;
      shader.second_ = shaderPtr->GetResourcePath().Get();
      Check(!string_util::IsEmpty(shader.second_.GetConst()));
      // Add the internal shader item
      shaders.PushBack(shader);
      // Print what we are doing
      if (0) {
        DG_LOG_LINE(TXT("shadermap.write: shadername:%s resourcepath:%s"),
            shader_name.GetConst(),
            shaderPtr->GetResourcePath().Get());
      }
    }
    // Write the internal shaders
    StreamWriteClass(out_stream, shaders);
  }

protected:
  ShadersType shaders_;
};

} // namespace dg
