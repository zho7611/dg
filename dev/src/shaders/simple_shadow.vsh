// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "Common.hlsl"
#include "Lighting.hlsl"

PixelShaderInputForShadow Main(
    float4 position : POSITION,
    float4 normal : NORMAL,
    float4 tex_coord : TEXCOORD0) {
  PixelShaderInputForShadow output;
  float4 world_pos = GetWorldPos(position);
  output.position = mul(world_pos, GlobalViewProjectionMatrix);
  output.tex_coord.xyzw = float4(tex_coord.x, tex_coord.y, 0, 0);
  output.depth.xyzw = float4(output.position.z, output.position.w, 0, 0);
  return output;
}
