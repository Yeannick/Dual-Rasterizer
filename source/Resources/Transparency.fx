
//----------------------------------------------------
//  Globals
//----------------------------------------------------

float gPI : PI;
float gLightIntensity : LightIntensity;
float gShininess : Shininess;

float3 gLightDirection 		: LightDirection;

float4x4 gWorldViewProj 	: WorldViewProjection;
float4x4 gWorldMatrix 		: WorldMatrix;
float4x4 gViewInverse 		: ViewInverseMatrix;

Texture2D gDiffuseMap 		: DiffuseMap;
Texture2D gNormalMap 		: NormalMap;
Texture2D gSpecularMap 		: SpecularMap;
Texture2D gGlossinessMap	: GlossinessMap;

RasterizerState gRasterizer : Rasterizer;

SamplerState samPoint
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Border; // or Mirror or Clamp or Border
	AddressV = Clamp;
	AddressW = Clamp;// or Mirror or Clamp or Border
	BorderColor = float4(0.0f, 0.0f, 1.0f, 1.0f);
};
SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Border; // or Mirror or Clamp or Border
	AddressV = Clamp;
	AddressW = Clamp;// or Mirror or Clamp or Border
	BorderColor = float4(0.0f, 0.0f, 1.0f, 1.0f);
};
SamplerState samAnisotropic
{
	Filter = ANISOTROPIC;
	AddressU = Border; // or Mirror or Clamp or Border
	AddressV = Clamp; // or Mirror or Clamp or Border
	AddressW = Clamp;
	BorderColor = float4(0.0f, 0.0f, 1.0f, 1.0f);
};
//----------------------------------------------------
//  Input/Output Structs
//----------------------------------------------------
struct VS_INPUT
{
	float3 Position : POSITION;
	float2 UV : TEXCOORD;
	float3 Normal : NORMAL;
	float3 Tangent 	: TANGENT;
	float3 Color :COLOR;
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float2 UV : TEXCOORD;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
	float4 WorldPosition :COLOR;

};
//----------------------------------------------------
//  Vertex Shader
//----------------------------------------------------

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.Position = mul(float4(input.Position, 1.f), gWorldViewProj);
	output.UV = input.UV;
	output.Normal = mul(normalize(input.Normal), (float3x3)gWorldMatrix);
	output.Tangent = mul(normalize(input.Tangent), (float3x3)gWorldMatrix);
	output.WorldPosition = mul(float4(input.Position, 1.f), gWorldMatrix);
	return output;
}
//----------------------------------------------------
//  Pixel Shader functions
//----------------------------------------------------
float4 SampleTexture(SamplerState mySampler, float2 UV)
{
	float4 textureSample = gDiffuseMap.Sample(mySampler, UV);
	return textureSample;
}
float3 SampleNormal(SamplerState mySampler, float2 UV)
{
	float4 normalSample = gNormalMap.Sample(mySampler, UV);
	return normalSample.xyz;
}
float SampleSpecular(SamplerState mySampler, float2 UV)
{
	float4 specularSample = gSpecularMap.Sample(mySampler, UV);
	return specularSample.r;
}

float SampleGlossiness(SamplerState mySampler, float2 UV)
{
	float4 glossinessSample = gGlossinessMap.Sample(mySampler, UV);
	return glossinessSample.r;
}
float3 CalcLambert(float diffuseRelfectance, float3 diffuseColor)
{
	return diffuseColor * diffuseRelfectance / gPI;
}
float3 CalcPhong(float reflectance, int exponent, float3 lightdirection, float3 viewDirection, float3 normal)
{
	float3 reflection = reflect(lightdirection, normal);
	float cosa = dot(viewDirection, reflection);

	float value = reflectance * pow(abs(cosa), float(exponent));
	float3 phongColor = float3(value, value, value);
	return phongColor;

}
float3 Shading(VS_OUTPUT input, SamplerState mySampler)
{
	float3 viewDirection = normalize(input.WorldPosition.xyz - gViewInverse[3].xyz);
	float3 binormal = normalize(cross(input.Normal, input.Tangent));
	float3x3 tangentSpaceAxis = float3x3(input.Tangent, binormal, input.Normal);

	float3 sampledNormal = SampleNormal(mySampler, input.UV);

	sampledNormal.xyz = 2.f * sampledNormal.xyz - 1.f;


	float3 newNormal = normalize(mul(sampledNormal.xyz, tangentSpaceAxis));

	float observedArea = dot(newNormal, -gLightDirection);


	float3 specularColor = SampleSpecular(mySampler, input.UV);
	float3 glossinessColor = SampleGlossiness(mySampler, input.UV);
	glossinessColor *= gShininess;
	float3 phongColor = CalcPhong(specularColor.r, int(glossinessColor.r), gLightDirection, viewDirection, newNormal);

	float3 diffuseColor;
	if (observedArea >= 0)
	{
		diffuseColor = SampleTexture(mySampler, input.UV);
	}
	else
	{
		diffuseColor = float3(1.f, 1.f, 1.f);
	}
	float diffuseReflectance = 1.f;
	float3 diffuseLambert = CalcLambert(diffuseReflectance, diffuseColor);
	float3 lightColor = float3(1.f, 1.f, 1.f);
	float3 FinalColor = lightColor * gLightIntensity * (diffuseLambert + phongColor) * observedArea;
	return saturate(FinalColor);
}
//----------------------------------------------------
//  Pixel Shader
//----------------------------------------------------

float4 PS(VS_OUTPUT input) : SV_TARGET
{

	return SampleTexture(samPoint,input.UV);
}
float4 PSLinear(VS_OUTPUT input) : SV_TARGET
{

	return SampleTexture(samLinear,input.UV);
}
float4 PSAnisotropic(VS_OUTPUT input) : SV_TARGET
{

	return SampleTexture(samAnisotropic,input.UV);
}
//----------------------------------------------------
//  Raster stage
//----------------------------------------------------

RasterizerState gRasterizerState
{
	CullMode = none;
	FrontCounterClockwise = true;
};
BlendState gBlendState
{
	BlendEnable[0] = true;
	SrcBlend = src_alpha;
	DestBlend = inv_src_alpha;
	BlendOp = add;
	SrcBlendAlpha = zero;
	DestBlendAlpha = zero;
	BlendOpAlpha = add;
	RenderTargetWriteMask[0] = 0x0F;


};

DepthStencilState gDepthStencilState
{
	DepthEnable = true;
	DepthWriteMask = zero;
	DepthFunc = less;
	StencilEnable = false;

	StencilReadMask = 0x0F;
	StencilWriteMask = 0x0F;

	FrontFaceStencilFunc = always;
	BackFaceStencilFunc = always;

	FrontFaceStencilDepthFail = keep;
	BackFaceStencilDepthFail = keep;

	FrontFaceStencilPass = keep;
	BackFaceStencilPass = keep;

	FrontFaceStencilFail = keep;
	BackFaceStencilFail = keep;
};
//----------------------------------------------------
//  Technique
//----------------------------------------------------

technique11 DefaultTechnique
{
	pass P0
	{
		SetRasterizerState(gRasterizerState);
		SetDepthStencilState(gDepthStencilState, 0);
		SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
	pass P1
	{
		SetRasterizerState(gRasterizerState);
		SetDepthStencilState(gDepthStencilState, 0);
		SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PSLinear()));
	}
	pass P2
	{
		SetRasterizerState(gRasterizerState);
		SetDepthStencilState(gDepthStencilState, 0);
		SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PSAnisotropic()));
	}
}

