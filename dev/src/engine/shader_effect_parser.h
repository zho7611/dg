// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class ShaderEffectParser {
public:
  static bool ParseChunk(Chunk& chunk, ShaderEffect* out_effect);
  static bool ParseBuffer(const Cstr* buffer, ShaderEffect* out_effect);

protected:
  static Shader* FindShaderObject(const Cstr* shader_binary_name);
  static bool ParseRenderState(JsonObject& pass, ShaderPass& shader_pass);
  static bool ParseSamplerStates(JsonArray& json_sampler_states, SamplerStateSet& sampler_states);
};

} // namespace dg
