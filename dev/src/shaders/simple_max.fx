// Transform Matrices
float4x4 GlobalViewMatrix : VIEW; // View matrix
float4x4 GlobalWorldMatrix : WORLD; // World matrix
float4x4 GlobalProjectionMatrix : PROJECTION; // Projection matrix
float4x4 GlobalWorldViewProjectionMatrix : WORLDVIEWPROJ;    // World * View * Projection matrix

// Light Properties
float4 GlobalLightAmbient  < string UIName = "Ambient"; > =
	float4(0.47f, 0.47f, 0.47f, 1.0f);    // Ambient
float4 GlobalLightDiffuse  < string UIName = "Diffuse"; > =
	float4(0.47f, 0.47f, 0.47f, 1.0f);    // Diffuse
	
float4 GlobalLightDirection : Direction
<  
	string UIName = "Light Direction"; 
	string Object = "TargetLight";
	int RefID = 0;
> = {-0.577, -0.577, 0.577,1.0};

texture GlobalAmbientOcclusionTexture : AmbientOcclusionMap
<
	string Name = "";
	string UIName = "Ambient Occlusion Texture";
	int TexCoord_ = 0;
	int MapChannel = 1;
>;

texture GlobalDiffuseTexture : DiffuseMap
<
	string Name = "";
	string UIName = "Diffuse Texture";
	int TexCoord_ = 0;
	int MapChannel = 1;
>;

texture GlobalSpecularTexture : SpecularMap
<
	string Name = "";
	string UIName = "Specular Texture";
	int TexCoord_ = 0;
	int MapChannel = 1;
>;

texture GlobalNormalTexture : NormalMap
<
	string Name = "";
	string UIName = "Normal Texture";
	int TexCoord_ =0;
	int MapChannel = 1;
>;

sampler AmbientOcclusionMapSampler = sampler_state
{
	Texture = <GlobalAmbientOcclusionTexture>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

sampler DiffuseMapSampler = sampler_state
{
	Texture = <GlobalDiffuseTexture>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

sampler SpecularMapSampler = sampler_state
{
	Texture = <GlobalSpecularTexture>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

sampler NormalMapSampler = sampler_state
{
	Texture = <GlobalNormalTexture>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

struct PixelShaderInput
{
	float4 Position_ : POSITION;   // vertex Position_ 
	float4 Diffuse_ : COLOR0;     // vertex Diffuse Color_ (note that COLOR0 is clamped from 0..1)
	float2 TexCoord_ : TEXCOORD0;	// Texture coordinates
	float3 EyeDir_ : TEXCOORD1; // Tangent-space Eye Vector
	float3 LightDir_ : TEXCOORD2; // Tangent-space Light Vector
};

struct PixelShaderInputForShadow
{
	float4 Position_ : POSITION;   // vertex Position_
	float2 TexCoord_ : TEXCOORD0;
	float2 Depth_ : TEXCOORD1;  // Depth and w
};

struct VertexShaderInput
{
	float4 Position_ : POSITION;
	float3 Normal_ : NORMAL;
	float2 TexCoord_ : TEXCOORD0;
	float3 Tangent_ : TANGENT;
	float3 Binormal_ : BINORMAL;
};

PixelShaderInput VertexShaderForSimpleMax(VertexShaderInput InInput)
{
	PixelShaderInput output;
	float3 nomalInWorld = mul(float4(InInput.Normal_, 0), GlobalWorldMatrix).xyz;
	output.Position_ = mul(InInput.Position_, GlobalWorldViewProjectionMatrix);
	output.Diffuse_.rgb =
		GlobalLightAmbient.rgb + max(dot(GlobalLightDirection.xyz, nomalInWorld), 0.0f);
	output.Diffuse_.a = 1.0f;
	output.TexCoord_.xy = InInput.TexCoord_.xy;
	// Transform LightDir_ and EyeDir_ into Tangent-space
	float3 tangent = mul(InInput.Tangent_, GlobalWorldMatrix);
	float3 normal = mul(InInput.Normal_, GlobalWorldMatrix);
	float3 binormal = mul(InInput.Binormal_, GlobalWorldMatrix);
	float3x3 tangentSpace = float3x3(binormal, tangent, normal);
	output.LightDir_ = mul(tangentSpace, GlobalLightDirection);
	output.EyeDir_ = 0; // What is the camera direction?
	return output;
}

struct PixelShaderOutput
{
	float4 Color_ : COLOR0;  // Pixel color
};

PixelShaderOutput PixelShaderForSimpleMax(PixelShaderInput InInput)
{
	PixelShaderOutput output;
	float diffuse = max(0, dot(2*(tex2D(NormalMapSampler, InInput.TexCoord_) - 0.5), InInput.LightDir_));
	output.Color_.rgb = diffuse * tex2D(DiffuseMapSampler, InInput.TexCoord_);
	output.Color_.a = 1.0f;
	return output;
}

technique TechForSimpleMax
{
	pass DefaultPass
	{
		VertexShader = compile vs_1_1 VertexShaderForSimpleMax();
		PixelShader = compile ps_2_0 PixelShaderForSimpleMax();
	}
}
