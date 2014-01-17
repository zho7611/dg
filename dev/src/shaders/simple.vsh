// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "common.hlsl"
#include "lighting.hlsl"

PixelShaderInput Main(VertexShaderInput input) {
  PixelShaderInput output;
  float4 world_pos = GetWorldPos(input.position);
  output.interpolated_position = output.position = mul(world_pos, GlobalViewProjectionMatrix);
  output.tex_coord = float4(input.tex_coord.x, input.tex_coord.y, 0, 0);
  float3x3 tangent_basis;
  CalcTangentBasis(input, tangent_basis);
  output.light_dir = float4(normalize(mul(normalize(GlobalLightDirection.xyz), tangent_basis)), 1.0);
  float3 eye_dir_in_world = normalize(GlobalCameraPosition.xyz - world_pos.xyz/world_pos.w);
  output.eye_dir = float4(normalize(mul(eye_dir_in_world, tangent_basis)), 1.0);
  output.half_dir = float4(normalize(output.light_dir.xyz + output.eye_dir.xyz), 1.0);
  output.shadow_position = mul(world_pos, GlobalLightViewProjectionMatrix);
  return output;
}
