// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "engine_first.h"

namespace dg {

#define DG_RETURN_VALUE_WHEN_NAME_MATCHES(NAME, VALUE) if (typeStr.IsEqualNoCase(NAME)) return VALUE;

static TextureAddressType GetSamplerStateTextureAddressType(const String& typeStr) {
  DG_RETURN_VALUE_WHEN_NAME_MATCHES(TXT("Wrap"), TextureAddressType_Wrap);
  DG_RETURN_VALUE_WHEN_NAME_MATCHES(TXT("Mirror"), TextureAddressType_Mirror);
  DG_RETURN_VALUE_WHEN_NAME_MATCHES(TXT("Clamp"), TextureAddressType_Clamp);
  DG_RETURN_VALUE_WHEN_NAME_MATCHES(TXT("Border"), TextureAddressType_Border);
  DG_RETURN_VALUE_WHEN_NAME_MATCHES(TXT("MirrorOnce"), TextureAddressType_MirrorOnce);
  return TextureAddressType_None;
}

static TextureFilterType GetSamplerStateTextureFilterType(const String& typeStr) {
  DG_RETURN_VALUE_WHEN_NAME_MATCHES(TXT("Point"), TextureFilterType_Point);
  DG_RETURN_VALUE_WHEN_NAME_MATCHES(TXT("Linear"), TextureFilterType_Linear);
  DG_RETURN_VALUE_WHEN_NAME_MATCHES(TXT("Anisotropic"), TextureFilterType_Anisotropic);
  DG_RETURN_VALUE_WHEN_NAME_MATCHES(TXT("PyramidalQuad"), TextureFilterType_PyramidalQuad);
  DG_RETURN_VALUE_WHEN_NAME_MATCHES(TXT("GaussianQuad"), TextureFilterType_GaussianQuad);
  DG_RETURN_VALUE_WHEN_NAME_MATCHES(TXT("ConvolutionMono"), TextureFilterType_ConvolutionMono);
  return TextureFilterType_None;
}

static RenderStateBlendType GetRenderStateBlendType(const String& typeStr) {
  DG_RETURN_VALUE_WHEN_NAME_MATCHES(TXT("Zero"), RenderStateBlendType_Zero);
  DG_RETURN_VALUE_WHEN_NAME_MATCHES(TXT("One"), RenderStateBlendType_One);
  DG_RETURN_VALUE_WHEN_NAME_MATCHES(TXT("SrcColor"), RenderStateBlendType_SrcColor);
  DG_RETURN_VALUE_WHEN_NAME_MATCHES(TXT("InvSrcColor"), RenderStateBlendType_InvSrcColor);
  DG_RETURN_VALUE_WHEN_NAME_MATCHES(TXT("SrcAlpha"), RenderStateBlendType_SrcAlpha);
  DG_RETURN_VALUE_WHEN_NAME_MATCHES(TXT("InvSrcAlpha"), RenderStateBlendType_InvSrcAlpha);
  DG_RETURN_VALUE_WHEN_NAME_MATCHES(TXT("DestAlpha"), RenderStateBlendType_DestAlpha);
  DG_RETURN_VALUE_WHEN_NAME_MATCHES(TXT("InvDestAlpha"), RenderStateBlendType_InvDestAlpha);
  DG_RETURN_VALUE_WHEN_NAME_MATCHES(TXT("DestColor"), RenderStateBlendType_DestColor);
  DG_RETURN_VALUE_WHEN_NAME_MATCHES(TXT("InvDestColor"), RenderStateBlendType_InvDestColor);
  DG_RETURN_VALUE_WHEN_NAME_MATCHES(TXT("SrcAlphaSat"), RenderStateBlendType_SrcAlphaSat);
  DG_RETURN_VALUE_WHEN_NAME_MATCHES(TXT("BothSrcAlpha"), RenderStateBlendType_BothSrcAlpha);
  DG_RETURN_VALUE_WHEN_NAME_MATCHES(TXT("BothInvSrcAlpha"), RenderStateBlendType_BothInvSrcAlpha);
  DG_RETURN_VALUE_WHEN_NAME_MATCHES(TXT("BlendFactor"), RenderStateBlendType_BlendFactor);
  DG_RETURN_VALUE_WHEN_NAME_MATCHES(TXT("InvBlendFactor"), RenderStateBlendType_InvBlendFactor);
  DG_RETURN_VALUE_WHEN_NAME_MATCHES(TXT("SrcColor2"), RenderStateBlendType_SrcColor2);
  DG_RETURN_VALUE_WHEN_NAME_MATCHES(TXT("InvSrcColor2"), RenderStateBlendType_InvSrcColor2);
  return RenderStateBlendType_None;
}

static RenderStateBlendOpType GetRenderStateBlendOpType(const String& typeStr) {
  DG_RETURN_VALUE_WHEN_NAME_MATCHES(TXT("Add"), RenderStateBlendOpType_Add);
  DG_RETURN_VALUE_WHEN_NAME_MATCHES(TXT("Subtract"), RenderStateBlendOpType_Subtract);
  DG_RETURN_VALUE_WHEN_NAME_MATCHES(TXT("RevSubtract"), RenderStateBlendOpType_RevSubtract);
  DG_RETURN_VALUE_WHEN_NAME_MATCHES(TXT("Min"), RenderStateBlendOpType_Min);
  DG_RETURN_VALUE_WHEN_NAME_MATCHES(TXT("Max"), RenderStateBlendOpType_Max);
  return RenderStateBlendOpType_None;
}

static RenderStateCompareType GetRenderStateCompareType(const String& typeStr) {
  DG_RETURN_VALUE_WHEN_NAME_MATCHES(TXT("Never"), RenderStateCompareType_Never);
  DG_RETURN_VALUE_WHEN_NAME_MATCHES(TXT("Less"), RenderStateCompareType_Less);
  DG_RETURN_VALUE_WHEN_NAME_MATCHES(TXT("Equal"), RenderStateCompareType_Equal);
  DG_RETURN_VALUE_WHEN_NAME_MATCHES(TXT("LessEqual"), RenderStateCompareType_LessEqual);
  DG_RETURN_VALUE_WHEN_NAME_MATCHES(TXT("Greater"), RenderStateCompareType_Greater);
  DG_RETURN_VALUE_WHEN_NAME_MATCHES(TXT("NotEqual"), RenderStateCompareType_NotEqual);
  DG_RETURN_VALUE_WHEN_NAME_MATCHES(TXT("GreaterEqual"), RenderStateCompareType_GreaterEqual);
  DG_RETURN_VALUE_WHEN_NAME_MATCHES(TXT("Always"), RenderStateCompareType_Always);
  return RenderStateCompareType_None;
}

static CullModeType GetRenderStateCullModeType(const String& typeStr) {
  DG_RETURN_VALUE_WHEN_NAME_MATCHES(TXT("None"), CullModeType_None);
  DG_RETURN_VALUE_WHEN_NAME_MATCHES(TXT("CW"), CullModeType_CW);
  DG_RETURN_VALUE_WHEN_NAME_MATCHES(TXT("CCW"), CullModeType_CCW);
  return CullModeType_None;
}

bool ShaderEffectParser::ParseChunk(Chunk& chunk, ShaderEffect* out_effect) {
  return ParseBuffer((const Cstr*)chunk.data_const(), out_effect);
}

bool ShaderEffectParser::ParseRenderState(JsonObject& pass, ShaderPass& shader_pass) {
  JsonObject::OrderedKeysIterator it = pass.Begin();
  JsonObject::OrderedKeysIterator ie = pass.End();
  shader_pass.render_states_.Clear();
  for (; it != ie; ++it) {
    // OrderedKeysIterator can iterate keys in FIFO order
    String& key = (*it);
    JsonValue& value = pass.Get(key);
    if (key.IsEqualNoCase(TXT("AlphaBlendEnable"))) {
      shader_pass.render_states_.PushBack(new RenderState_AlphaBlendEnable(value.GetBoolean()));
    } else if (key.IsEqualNoCase(TXT("AlphaTestEnable"))) {
      shader_pass.render_states_.PushBack(new RenderState_AlphaTestEnable(value.GetBoolean()));
    } else if (key.IsEqualNoCase(TXT("ZEnable"))) {
      shader_pass.render_states_.PushBack(new RenderState_ZEnable(value.GetBoolean()));
    } else if (key.IsEqualNoCase(TXT("ZWriteEnable"))) {
      shader_pass.render_states_.PushBack(new RenderState_ZWriteEnable(value.GetBoolean()));
    } else if (key.IsEqualNoCase(TXT("ScissorTestEnable"))) {
      shader_pass.render_states_.PushBack(new RenderState_ScissorTestEnable(value.GetBoolean()));
    } else if (key.IsEqualNoCase(TXT("ZFunc"))) {
      RenderStateCompareType compare_type = GetRenderStateCompareType(value.GetString());
      shader_pass.render_states_.PushBack(new RenderState_ZFunc(compare_type));
    } else if (key.IsEqualNoCase(TXT("CullMode"))) {
      CullModeType cull_type = GetRenderStateCullModeType(value.GetString());
      shader_pass.render_states_.PushBack(new RenderState_CullMode(cull_type));
    } else if (key.IsEqualNoCase(TXT("AlphaFunc"))) {
      RenderStateCompareType compare_type = GetRenderStateCompareType(value.GetString());
      shader_pass.render_states_.PushBack(new RenderState_AlphaFunc(compare_type));
    } else if (key.IsEqualNoCase(TXT("AlphaRef"))) {
      shader_pass.render_states_.PushBack(new RenderState_AlphaRef(value.GetInteger()));
    } else if (key.IsEqualNoCase(TXT("SrcBlend"))) {
      RenderStateBlendType blend_mode = GetRenderStateBlendType(value.GetString());
      shader_pass.render_states_.PushBack(new RenderState_SrcBlend(blend_mode));
    } else if (key.IsEqualNoCase(TXT("DestBlend"))) {
      RenderStateBlendType blend_mode = GetRenderStateBlendType(value.GetString());
      shader_pass.render_states_.PushBack(new RenderState_DestBlend(blend_mode));
    } else if (key.IsEqualNoCase(TXT("BlendOp"))) {
      RenderStateBlendOpType blend_op = GetRenderStateBlendOpType(value.GetString());
      shader_pass.render_states_.PushBack(new RenderState_BlendOp(blend_op));
    }
  }
  return true;
}

bool ShaderEffectParser::ParseSamplerStates(JsonArray& json_sampler_states, SamplerStateSet& sampler_states) {
  sampler_states.Clear();
  for (int idx = 0; idx < json_sampler_states.Count(); ++idx) {
    // Parse the render state
    JsonObject& json_state = json_sampler_states.Get(idx).GetJsonObject();
    JsonObject::OrderedKeysIterator it = json_state.Begin();
    JsonObject::OrderedKeysIterator ie = json_state.End();
    int stage_index = -1;
    for (; it != ie; ++it) {
      String& key = (*it);
      JsonValue& value = json_state.Get(key);
      if (key.IsEqualNoCase(TXT("Index"))) {
        stage_index = value.GetInteger();
      } else if (key.IsEqualNoCase(TXT("AddressU"))) {
        TextureAddressType state_value = GetSamplerStateTextureAddressType(value.GetString());
        sampler_states.PushBack(new SamplerState_AddressU(stage_index, state_value));
      } else if (key.IsEqualNoCase(TXT("AddressV"))) {
        TextureAddressType state_value = GetSamplerStateTextureAddressType(value.GetString());
        sampler_states.PushBack(new SamplerState_AddressV(stage_index, state_value));
      } else if (key.IsEqualNoCase(TXT("AddressW"))) {
        TextureAddressType state_value = GetSamplerStateTextureAddressType(value.GetString());
        sampler_states.PushBack(new SamplerState_AddressW(stage_index, state_value));
      } else if (key.IsEqualNoCase(TXT("MagFilter"))) {
        TextureFilterType state_value = GetSamplerStateTextureFilterType(value.GetString());
        sampler_states.PushBack(new SamplerState_MagFilter(stage_index, state_value));
      } else if (key.IsEqualNoCase(TXT("MinFilter"))) {
        TextureFilterType state_value = GetSamplerStateTextureFilterType(value.GetString());
        sampler_states.PushBack(new SamplerState_MinFilter(stage_index, state_value));
      } else if (key.IsEqualNoCase(TXT("MipFilter"))) {
        TextureFilterType state_value = GetSamplerStateTextureFilterType(value.GetString());
        sampler_states.PushBack(new SamplerState_MipFilter(stage_index, state_value));
      } else if (key.IsEqualNoCase(TXT("SrgbTexture"))) {
        sampler_states.PushBack(new SamplerState_SrgbTexture(stage_index, value.GetBoolean()));
      } else {
        DG_UNIMPLEMENTED();
      }
    }
  }
  return true;
}

bool ShaderEffectParser::ParseBuffer(const Cstr* buffer, ShaderEffect* out_effect) {
  Json json;
  JsonValue value;
  // Parse the json
  if (!json.Parse(buffer, value)) {
    return false;
  }
  // Parse the name
  out_effect->effect_name_ = value.Get(TXT("Name")).GetString();
  // Parse the techniques
  out_effect->techniques_.Clear();
  JsonArray& techs = value.Get(TXT("Techniques")).GetJsonArray();
  for (int idx = 0; idx < techs.Count(); ++idx) {
    // Get the current object
    JsonObject& tech = techs.Get(idx).GetJsonObject();
    // Create a shader tech
    ShaderTechnique shader_tech;
    shader_tech.name_ = tech.Get(TXT("Name")).GetString();
    // Passes
    JsonArray& passes = tech.Get(TXT("Passes")).GetJsonArray();
    for (int pass_idx = 0; pass_idx < passes.Count(); ++pass_idx) {
      // Get the pass
      JsonObject& pass = passes.Get(pass_idx).GetJsonObject();
      ShaderPass shader_pass;
      shader_pass.name_ = pass.Get(TXT("Name")).GetString();
      // Get the render states
      ParseRenderState(pass, shader_pass);
      // Get the sampler states
      if (pass.IsFound(TXT("SamplerStates"))) {
        ParseSamplerStates(pass.Get(TXT("SamplerStates")).GetJsonArray(), shader_pass.sampler_states_);
      }
      // Get the vertex shader
      String vertex_shader_name = pass.Get(TXT("VertexShader")).GetString();
      VertexShader* vs = static_cast<VertexShader*>(FindShaderObject(vertex_shader_name.Get()));
      // Get the pixel shader
      String pixel_shader_name = pass.Get(TXT("PixelShader")).GetString();
      PixelShader* ps = static_cast<PixelShader*>(FindShaderObject(pixel_shader_name.Get()));
      shader_pass.shader_program_.SetShaders(vs, ps);
      // Add the shader pass into the shader tech pass list
      shader_tech.passes_.PushBack(shader_pass);
    }
    // Add the shader tech into the techniques
    out_effect->techniques_.PushBack(shader_tech);
  }
  return true;
}

Shader* ShaderEffectParser::FindShaderObject(const Cstr* shader_binary_name) {
  return g_engine.FindShader(shader_binary_name);
}

} // namespace dg
