// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

// Light Properties
float4 GlobalLightDirection : register(vs, c24);
float4 GlobalLightAmbient : register(ps, c1);
float4 GlobalLightDiffuse : register(ps, c2);
float4 GlobalLightBrightness : register(ps, c4);

// Camera
float4 GlobalCameraPosition : register(vs, c25);

// ShadowMap Offsets
float4 GlobalShadowMapOffsets[6] : register(ps, c6);

// Cascaded Shadow Map
static const int GlobalNumCascades = 4; //#csm #numcascades
float4 GlobalCascadeDepths[(GlobalNumCascades-1+3)/4] : register(ps, c13);
float4 GlobalCascadeScales[GlobalNumCascades] : register(ps, c14);
float4 GlobalCascadeOffsets[GlobalNumCascades] : register(ps, c18);

// Material Properties
float4 GlobalMaterialSpecular; // Rgb: intensity, a : power
float4 GlobalMaterialAmbient;
float4 GlobalMaterialDiffuse;

void CalcTangentBasis(in VertexShaderInput input, out float3x3 outTangentBasis) {
  float3 tangent = normalize(mul(input.tangent.xyz, GlobalWorldMatrix));
  float3 normal = normalize(mul(input.normal.xyz, GlobalWorldMatrix));
  // Flip the binormal with the sign value when the UVs are mirrored
  float3 binormal = normalize(cross(normal, tangent)) * input.tangent.w;
  outTangentBasis = transpose(float3x3(tangent, binormal, normal));
}

float CalcShadow(in PixelShaderInput input) {
  const float kShadowEpsilon = 0.0f;
  float2 tex_coord_shadow = 0.5 * input.shadow_position.xy / input.shadow_position.w + 0.5;
  tex_coord_shadow.y = 1 - tex_coord_shadow.y;
  float depth = (input.shadow_position.z / input.shadow_position.w) - kShadowEpsilon;
  float4 depth4;
  float shadow_term = 0;
  const float kNumSamples = 13.0f;
  for (int idx = 0; idx < 3; idx++) {
    depth4.x =
        tex2D(ShadowTextureSampler, tex_coord_shadow + GlobalShadowMapOffsets[idx*2+0].xy).x;
    depth4.y =
        tex2D(ShadowTextureSampler, tex_coord_shadow + GlobalShadowMapOffsets[idx*2+0].zw).x;
    depth4.z =
        tex2D(ShadowTextureSampler, tex_coord_shadow + GlobalShadowMapOffsets[idx*2+1].xy).x;
    depth4.w =
        tex2D(ShadowTextureSampler, tex_coord_shadow + GlobalShadowMapOffsets[idx*2+1].zw).x;
    shadow_term += dot(depth4 > depth, (1.0 / kNumSamples));
  }
  shadow_term +=
      (tex2D(ShadowTextureSampler, tex_coord_shadow).x > depth) ? (1.0 / kNumSamples) : 0;
  return shadow_term;
}

float CalcShadowTerm(in float2 shadowTexCoord, in float shadowDepth) {
  float4 depth4;
  float shadow_term = 0;
  const float InvNumSamples = 1.0f / 13.0f;
  for (int idx = 0; idx < 3; idx++) {
    depth4.x =
        tex2D(ShadowTextureSampler, shadowTexCoord + GlobalShadowMapOffsets[idx*2+0].xy).x;
    depth4.y =
        tex2D(ShadowTextureSampler, shadowTexCoord + GlobalShadowMapOffsets[idx*2+0].zw).x;
    depth4.z =
        tex2D(ShadowTextureSampler, shadowTexCoord + GlobalShadowMapOffsets[idx*2+1].xy).x;
    depth4.w =
        tex2D(ShadowTextureSampler, shadowTexCoord + GlobalShadowMapOffsets[idx*2+1].zw).x;
    shadow_term += dot(depth4 > shadowDepth, InvNumSamples);
  }
  shadow_term +=
      (tex2D(ShadowTextureSampler, shadowTexCoord).x > shadowDepth) ? InvNumSamples : 0;
  return shadow_term;
}

float CalcShadowFast(in PixelShaderInput input) {
  const float kShadowEpsilon = 0.0f;
  float2 tex_coord_shadow = 0.5 * input.shadow_position.xy / input.shadow_position.w + 0.5;
  tex_coord_shadow.y = 1 - tex_coord_shadow.y;
  float depth = (input.shadow_position.z / input.shadow_position.w) - kShadowEpsilon;
  float shadow_term =
      (tex2D(ShadowTextureSampler, tex_coord_shadow).x > depth) ? 1.0f : 0;
  return shadow_term;
}

float3 GetCascadeIndex(float3 pos_in_light_proj_space, float depth_in_camera_proj_space, out float out_depth) {
  /* #fixme: this is not working for now
  int kCascadeIdx = 0;
  for(int i = 0; i < (GlobalNumCascades-1+3) / 4; ++i){
    float4 v = GlobalCascadeDepths[i] < float4(depth_in_camera_proj_space, depth_in_camera_proj_space, depth_in_camera_proj_space, depth_in_camera_proj_space);
    kCascadeIdx += dot(float4(1,1,1,1), v);
  }
  */
  float4 v = GlobalCascadeDepths[0] < float4(depth_in_camera_proj_space, depth_in_camera_proj_space, depth_in_camera_proj_space, depth_in_camera_proj_space);
  int kCascadeIdx = dot(float4(1, 1, 1, 1), v);
  pos_in_light_proj_space = pos_in_light_proj_space * GlobalCascadeScales[kCascadeIdx].xyz + GlobalCascadeOffsets[kCascadeIdx].xyz;
  float kShadowEpsilon = -0.01f;
  out_depth = pos_in_light_proj_space.z + kShadowEpsilon;
  float3 tex_shadow = float3(0.5 * pos_in_light_proj_space.xy + 0.5, kCascadeIdx);
  tex_shadow.y = 1.0f - tex_shadow.y;
  tex_shadow.x /=GlobalNumCascades;
  tex_shadow.x += ((float)kCascadeIdx)/GlobalNumCascades;
  return tex_shadow;
}

float3 GetCascadeIndexColor(float3 pos_in_light_proj_space, float depth_in_camera_proj_space, out float depth) {
  float4 v = GlobalCascadeDepths[0] < float4(depth_in_camera_proj_space, depth_in_camera_proj_space, depth_in_camera_proj_space, depth_in_camera_proj_space);
  depth = 0;
  if (GlobalCascadeDepths[0].z < depth_in_camera_proj_space) return float3(1, 1, 0);
  if (GlobalCascadeDepths[0].y < depth_in_camera_proj_space) return float3(0, 0, 1);
  if (GlobalCascadeDepths[0].x < depth_in_camera_proj_space) return float3(0, 1, 0);
  return float3(1, 0, 0);
}

float CalcShadowCsm(in PixelShaderInput input) {
  float depth_in_camera_proj_space = input.interpolated_position.z / input.interpolated_position.w;
  float depth;
  float3 tex_coord_shadow = GetCascadeIndex(
      input.shadow_position.xyz / input.shadow_position.w,
      depth_in_camera_proj_space,
      depth);
  // For simple and fast shadow
  //#define WITH_SIMPLE_FAST_SHADOW 1
#if WITH_SIMPLE_FAST_SHADOW
  float shadow_term = (tex2D(ShadowTextureSampler, tex_coord_shadow.xy).x > depth) ? 1.0f : 0;
#else
  float shadow_term = CalcShadowTerm(tex_coord_shadow.xy, depth);
#endif
  return shadow_term;
}

float3 CalcShadowColorCsm(in PixelShaderInput input) {
  float depth_in_camera_proj_space = input.interpolated_position.z / input.interpolated_position.w;
  float depth;
  float3 indexColor = GetCascadeIndexColor(input.shadow_position.xyz / input.shadow_position.w, depth_in_camera_proj_space, depth);
  return indexColor;
}

float3 GetNormal(in PixelShaderInput input) {
  // [0, 1] -> [-1,1]
  return normalize(2 * (tex2D(NormalTextureSampler, input.tex_coord).xyz - .5));
}

float GetAmbientOcclusion(in PixelShaderInput input) {
  float ambient_occlusion = tex2D(AmbientOcclusionTextureSampler, input.tex_coord).x;
  //#todo Enable when it is ready
  //return ambient_occlusion;
  return 1;
}

float3 GetAmbient(in PixelShaderInput input) {
  return GlobalLightAmbient * GlobalMaterialAmbient;
}

float3 GetDiffuse(in PixelShaderInput input, in float3 normal) {
  return GlobalLightDiffuse * GlobalMaterialDiffuse * saturate(dot(normal, input.light_dir));
}

float3 GetSpecular(in PixelShaderInput input, in float3 normal) {
  float3 specular_factor = tex2D(SpecularTextureSampler, input.tex_coord);
  return specular_factor * GlobalMaterialSpecular.rgb * pow(saturate(dot(normal, input.half_dir)), GlobalMaterialSpecular.a);
}

float3 GetAlbedo(in PixelShaderInput input) {
  return tex2D(DiffuseTextureSampler, input.tex_coord);
}

#if WITH_DEPTHOFFIELD
float GetDepth(in PixelShaderInput input) {
  return tex2D(DepthTextureSampler, input.tex_coord).r;
}
#endif
