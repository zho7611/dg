// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "Common.hlsl"
#include "Blur.hlsl"

PixelShaderInputForBlur Main(VertexShaderInputForBlur input) {
  PixelShaderInputForBlur output;
  output.position = input.position + BlurVertexTexcoordOffset;
  output.tex_coord.xyzw = float4(input.tex_coord.x, input.tex_coord.y, 0, 0);
  return output;
}
