// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "Common.hlsl"
#include "Sprite.hlsl"

PixelShaderInputForSprite Main(VertexShaderInputForSprite input) {
  PixelShaderInputForSprite output;
  output.position = input.position;
  output.position.xy = 2 * (input.position - .5f) * GlobalInvSurfaceSize.xy - 1;
  output.position.y *= -1;
  output.color = input.color;
  output.tex_coord = input.tex_coord;
  return output;
}
