
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

	float4 cameraPos = mul(mCamera, pos4);
	cameraPos = cameraPos / cameraPos.w;
	Output.PosP = mul(mProj, cameraPos);
	Output.PosVS = Output.PosP;

	Output.Color = Input.Color;
	Output.UV = Input.UV;

	Output.UV.y = mUVInversed.x + mUVInversed.y * Input.UV.y;

	CalculateAndStoreAdvancedParameter(Input, Output);

	return Output;
}
