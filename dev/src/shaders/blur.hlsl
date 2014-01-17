// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

// Common Shaders for Blur
static const int kNumDownSamples = 16;
static const int kNumBlurSamples = 15;
float4 BlurMapOffsets[kNumBlurSamples];
float4 BlurMapWeights[kNumBlurSamples];
float4 DownSampleOffsets[kNumDownSamples];
float4 BlurVertexTexcoordOffset;
float4 BlurTimeDeltaInSec;
float4 GlobalBloomWeight;

sampler SourceTextureSampler : register(s0);
sampler TargetTextureSampler : register(s1);

struct VertexShaderInputForBlur {
  float4 position : POSITION;
  float4 tex_coord : TEXCOORD0;
};

struct PixelShaderInputForBlur {
  float4 position : POSITION;
  float4 tex_coord : TEXCOORD0;
};
