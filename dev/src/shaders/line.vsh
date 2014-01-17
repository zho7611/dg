// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "Common.hlsl"

PixelShaderInputForLine Main(VertexShaderInputForLine input) {
  PixelShaderInputForLine output;
  float4 world_pos = input.position;
  output.position = mul(world_pos, GlobalViewProjectionMatrix);
  output.color = input.color;
  return output;
}
