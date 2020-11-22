
struct VS_Input
{
	float3 Pos : POSITION0;
	float4 Color : NORMAL0;
	float4 Normal : NORMAL1;
	float4 Tangent : NORMAL2;
	float2 UV1 : TEXCOORD0;
	float2 UV2 : TEXCOORD1;

	float4 Alpha_Dist_UV : TEXCOORD2;
	float2 BlendUV : TEXCOORD3;
	float4 Blend_Alpha_Dist_UV : TEXCOORD4;
	float FlipbookIndex : TEXCOORD5;
	float AlphaThreshold : TEXCOORD6;
};

struct VS_Output
{
	float4 PosVS : SV_POSITION;
	linear centroid float4 VColor : COLOR;
	linear centroid float2 UV : TEXCOORD0;
	float3 WorldN : TEXCOORD1;
	float3 WorldT : TEXCOORD2;
	float3 WorldB : TEXCOORD3;

	float4 Alpha_Dist_UV : TEXCOORD4;
	float4 Blend_Alpha_Dist_UV : TEXCOORD5;

	// BlendUV, FlipbookNextIndexUV
	float4 Blend_FBNextIndex_UV : TEXCOORD6;

	// x - FlipbookRate, y - AlphaThreshold
	float2 Others : TEXCOORD7;

#ifndef DISABLED_SOFT_PARTICLE
	float4 PosP : TEXCOORD8;
#endif
};

cbuffer VS_ConstantBuffer : register(b0)
{
	float4x4 mCamera;
	float4x4 mProj;
	float4 mUVInversed;

	float4 mflipbookParameter; // x:enable, y:loopType, z:divideX, w:divideY
};

#include "ad_sprite_common_vs.fx"

VS_Output main(const VS_Input Input)
{
	VS_Output Output = (VS_Output)0;
	float3 worldPos = Input.Pos;
	float3 worldNormal = (Input.Normal.xyz - float3(0.5, 0.5, 0.5)) * 2.0;
	float3 worldTangent = (Input.Tangent.xyz - float3(0.5, 0.5, 0.5)) * 2.0;
	float3 worldBinormal = cross(worldNormal, worldTangent);

	// UV
	float2 uv1 = Input.UV1;
	float2 uv2 = Input.UV1;
	uv1.y = mUVInversed.x + mUVInversed.y * uv1.y;
	uv2.y = mUVInversed.x + mUVInversed.y * uv2.y;

	// NBT
	Output.WorldN = worldNormal;
	Output.WorldB = worldBinormal;
	Output.WorldT = worldTangent;

	float3 pixelNormalDir = float3(0.5, 0.5, 1.0);

	float4 cameraPos = mul(mCamera, float4(worldPos, 1.0));
	cameraPos = cameraPos / cameraPos.w;
	Output.PosVS = mul(mProj, cameraPos);

	Output.VColor = Input.Color;
	Output.UV = uv1;

	CalculateAndStoreAdvancedParameter(Input, Output);

#ifndef DISABLED_SOFT_PARTICLE
	Output.PosP = Output.PosVS;

#endif

	return Output;
}
