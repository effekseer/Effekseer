
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
	float2 UV : TEXCOORD0;

	float3 Binormal : NORMAL1;
	float3 Tangent : NORMAL2;

	float4 Alpha_Dist_UV : TEXCOORD1;
	float2 BlendUV : TEXCOORD2;
	float4 Blend_Alpha_Dist_UV : TEXCOORD3;
	float FlipbookIndex : TEXCOORD4;
	float AlphaThreshold : TEXCOORD5;
};

struct VS_Output
{
	float4 PosVS : SV_POSITION;
	linear centroid float4 Color : COLOR;
	linear centroid float2 UV : TEXCOORD0;

	float4 PosP : TEXCOORD1;
	float4 PosU : TEXCOORD2;
	float4 PosR : TEXCOORD3;

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

	float4 localBinormal = {Input.Pos.x + Input.Binormal.x, Input.Pos.y + Input.Binormal.y, Input.Pos.z + Input.Binormal.z, 1.0};
	float4 localTangent = {Input.Pos.x + Input.Tangent.x, Input.Pos.y + Input.Tangent.y, Input.Pos.z + Input.Tangent.z, 1.0};
	localBinormal = mul(mCamera, localBinormal);
	localTangent = mul(mCamera, localTangent);

	float4 cameraPos = mul(mCamera, pos4);
	cameraPos = cameraPos / cameraPos.w;

	localBinormal = localBinormal / localBinormal.w;
	localTangent = localTangent / localTangent.w;

	localBinormal = cameraPos + normalize(localBinormal - cameraPos);
	localTangent = cameraPos + normalize(localTangent - cameraPos);

	Output.PosVS = mul(mProj, cameraPos);

	Output.PosP = Output.PosVS;

	Output.PosU = mul(mProj, localBinormal);
	Output.PosR = mul(mProj, localTangent);

	Output.PosU /= Output.PosU.w;
	Output.PosR /= Output.PosR.w;
	Output.PosP /= Output.PosP.w;

	Output.Color = Input.Color;
	Output.UV = Input.UV;

	Output.UV.y = mUVInversed.x + mUVInversed.y * Input.UV.y;

	CalculateAndStoreAdvancedParameter(Input, Output);

	return Output;
}
