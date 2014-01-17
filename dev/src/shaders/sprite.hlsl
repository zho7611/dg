// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

// Common HLSL for Sprite Shaders

// 1 / SurfaceSize
float4 GlobalInvSurfaceSize;

// Sprite Texture
texture GlobalSpriteTexture;

sampler SpriteTextureSampler = sampler_state {
  Texture = <GlobalSpriteTexture>;
  MinFilter = Linear;
  MagFilter = Linear;
  MipFilter = Linear;
};

struct VertexShaderInputForSprite {
float4 position :
  POSITION;
float4 tex_coord :
  TEXCOORD0;
float4 color :
  COLOR;
};

struct PixelShaderInputForSprite {
float4 position :
  POSITION;
float4 tex_coord :
  TEXCOORD0;
float4 color :
  COLOR;
};
