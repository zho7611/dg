// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

//#define WITH_DEPTHOFFIELD 1

// Transform Matrices
float4x4 GlobalWorldMatrix : register(vs, c4); // World matrix
float4x4 GlobalViewProjectionMatrix : register(vs, c12); // View * Projection matrix
float4x4 GlobalLightViewProjectionMatrix : register(vs, c20); // LightView * LightProjection matrix for shadow

float4 GlobalGammaCorrection : register(ps, c0);

sampler DiffuseTextureSampler : register(s0);
sampler SpecularTextureSampler : register(s1);
sampler ShadowTextureSampler : register(s2);
sampler NormalTextureSampler : register(s3);
sampler AmbientOcclusionTextureSampler : register(s4);
#if WITH_DEPTHOFFIELD
sampler DepthTextureSampler : register(s5);
#endif

struct VertexShaderInput {
  float4 position : POSITION;
  float4 normal : NORMAL;
  float4 tex_coord : TEXCOORD0;
  float4 tangent : TANGENT;
};

struct PixelShaderInputForPosTex {
  float4 position : POSITION; // Vertex position
  float4 tex_coord : TEXCOORD0; // Texture coordinates
};

struct PixelShaderInputForVertexColor {
  float4 position : POSITION;
  float4 color : COLOR;
};

struct PixelShaderInput {
  float4 position : POSITION; // Vertex position
  float4 tex_coord : TEXCOORD0; // Texture coordinates
  float4 eye_dir : TEXCOORD1; // Tangent-space Eye Vector3
  float4 light_dir : TEXCOORD2; // Tangent-space Light Vector3
  float4 shadow_position : TEXCOORD3; // Position3 in light coordinates for shadow term
  float4 half_dir : TEXCOORD4; // Tanent-space Half Vector3
  float4 interpolated_position : TEXCOORD5; // Interpolated Vector3
};

struct PixelShaderInputForNull {
  float4 position : POSITION; // Vertex position
};

struct PixelShaderInputForShadow {
  float4 position : POSITION; // Vertex position
  float4 tex_coord : TEXCOORD0;
  float4 depth : TEXCOORD1; // Depth and w
};

struct PixelShaderInputForDepth {
  float4 position : POSITION; // Vertex Vector3
  float4 depth : TEXCOORD0; // Xy(Depth and w)
};

struct PixelShaderOutput {
  float4 color : COLOR0; // Pixel Color
};

struct VertexShaderInputForLine {
  float4 position : POSITION;
  float4 color : COLOR;
};

struct PixelShaderInputForLine {
  float4 position : POSITION; // Vertex position
  float4 color : COLOR;
};

float4 GetWorldPos(in float4 position) {
  return mul(position, GlobalWorldMatrix);
}

float3 GetWorldNormal(in float3 normal) {
  return normalize(mul(float4(normal, 1), GlobalWorldMatrix).xyz);
}
