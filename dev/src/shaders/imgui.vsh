// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

struct VertexShaderInput {
  float4 VertexPosition : POSITION;
  float2 VertexTexCoord : TEXCOORD0;
  float4 VertexColor : TEXCOORD1;
};

struct PixelShaderInput {
  float4 position : POSITION; // Vertex position
  float2 texCoord : TEXCOORD0;
  float4 vertexColor : TEXCOORD1;
};

float4 Viewport;

PixelShaderInput Main(VertexShaderInput input) {
  PixelShaderInput output;
  output.vertexColor = input.VertexColor;
  output.texCoord = input.VertexTexCoord;
  output.position = float4((input.VertexPosition.xy + 0.5) * 2.0 / Viewport.xy - 1.0, 0.0, 1.0);
  return output;
}
