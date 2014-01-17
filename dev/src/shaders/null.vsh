// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "Common.hlsl"

PixelShaderInputForNull Main(VertexShaderInput input) {
  PixelShaderInputForNull output;
  output.position = input.position;
  return output;
}