// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "Common.hlsl"

// 1 / SurfaceSize
float4 GlobalInvSurfaceSize;

PixelShaderInputForLine Main(VertexShaderInputForLine input) {
  PixelShaderInputForLine output;
  output.position = input.position;
  output.position.xy = 2 * (input.position - .5f) * GlobalInvSurfaceSize.xy - 1;
  output.position.y *= -1;
  output.color = input.color;
  return output;
}
