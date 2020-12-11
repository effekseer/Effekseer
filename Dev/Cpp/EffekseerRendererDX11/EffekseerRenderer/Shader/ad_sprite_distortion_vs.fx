
cbuffer VS_ConstantBuffer : register(b0)
{
	float4x4 mCamera;
	float4x4 mProj;
	float4 mUVInversed;

	float4 mflipbookParameter; // x:enable, y:loopType, z:divideX, w:divideY
};

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
	linear centroid float2 UV : TEXCOORD0;
	float4 ProjBinormal : TEXCOORD1;
	float4 ProjTangent : TEXCOORD2;
	float4 PosP : TEXCOORD3;
	linear centroid float4 Color : COLOR0;

	float4 Alpha_Dist_UV : TEXCOORD4;
	float4 Blend_Alpha_Dist_UV : TEXCOORD5;

	// BlendUV, FlipbookNextIndexUV
	float4 Blend_FBNextIndex_UV : TEXCOORD6;

	// x - FlipbookRate, y - AlphaThreshold
	float2 Others : TEXCOORD7;
};

#include "ad_sprite_common_vs.fx"

VS_Output main(const VS_Input Input)
{
	VS_Output Output = (VS_Output)0;
	float4 pos4 = {Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0};

	float3 worldNormal = (Input.Normal.xyz - float3(0.5, 0.5, 0.5)) * 2.0;
	float3 worldTangent = (Input.Tangent.xyz - float3(0.5, 0.5, 0.5)) * 2.0;
	float3 worldBinormal = cross(worldNormal, worldTangent);

	float4 cameraPos = mul(mCamera, pos4);
	cameraPos = cameraPos / cameraPos.w;

	Output.PosVS = mul(mProj, cameraPos);

	Output.PosP = Output.PosVS;

	float4 localTangent = pos4;
	float4 localBinormal = pos4;
	localTangent.xyz += worldTangent;
	localBinormal.xyz += worldBinormal;
	
	Output.ProjTangent = mul(mProj, mul(mCamera, localTangent));
	Output.ProjBinormal = mul(mProj, mul(mCamera, localBinormal));

	Output.Color = Input.Color;
	Output.UV = Input.UV1;

	Output.UV.y = mUVInversed.x + mUVInversed.y * Input.UV1.y;

	CalculateAndStoreAdvancedParameter(Input, Output);

	return Output;
}
