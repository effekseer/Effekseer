
cbuffer VS_ConstantBuffer : register(b0)
{
	float4x4 mCameraProj;

#ifdef DISABLE_INSTANCE
	float4x4 mModel;
	float4 fUV;
	float4 fAlphaUV;
	float4 fUVDistortionUV;
	float4 fBlendUV;
	float4 fBlendAlphaUV;
	float4 fBlendUVDistortionUV;
#else
	float4x4 mModel_Inst[__INST__];
	float4 fUV[__INST__];
	float4 fAlphaUV[__INST__];
	float4 fUVDistortionUV[__INST__];
	float4 fBlendUV[__INST__];
	float4 fBlendAlphaUV[__INST__];
	float4 fBlendUVDistortionUV[__INST__];
#endif

	float4 fFlipbookParameter; // x:enable, y:loopType, z:divideX, w:divideY

#ifdef DISABLE_INSTANCE
	float4 fFlipbookIndexAndNextRate;

	float4 fModelAlphaThreshold;

	float4 fModelColor;
#else
	float4 fFlipbookIndexAndNextRate[__INST__];

	float4 fModelAlphaThreshold[__INST__];

	float4 fModelColor[__INST__];
#endif

	float4 fLightDirection;
	float4 fLightColor;
	float4 fLightAmbient;

	float4 mUVInversed;
};

struct VS_Input
{
	float3 Pos : POSITION0;
	float3 Normal : NORMAL0;
	float3 Binormal : NORMAL1;
	float3 Tangent : NORMAL2;
	float2 UV : TEXCOORD0;
	float4 Color : NORMAL3;

#if defined(ENABLE_DIVISOR)
	float Index : BLENDINDICES0;
#elif !defined(DISABLE_INSTANCE)
	uint Index : SV_InstanceID;
#endif
};

#if defined(ENABLE_DISTORTION)

struct VS_Output
{
	float4 PosVS : SV_POSITION;
	// xy uv z - FlipbookRate, w - AlphaThreshold
	linear centroid float4 UV_Others : TEXCOORD0;
	float4 ProjBinormal : TEXCOORD1;
	float4 ProjTangent : TEXCOORD2;
	float4 PosP : TEXCOORD3;
	linear centroid float4 Color : COLOR0;

	float4 Alpha_Dist_UV : TEXCOORD4;
	float4 Blend_Alpha_Dist_UV : TEXCOORD5;

	// BlendUV, FlipbookNextIndexUV
	float4 Blend_FBNextIndex_UV : TEXCOORD6;
};

#else

struct VS_Output
{
	float4 PosVS : SV_POSITION;
	linear centroid float4 Color : COLOR;
	// xy uv z - FlipbookRate, w - AlphaThreshold
	linear centroid float4 UV_Others : TEXCOORD0;
	float3 WorldN : TEXCOORD1;
#if ENABLE_LIGHTING
	float3 WorldB : TEXCOORD2;
	float3 WorldT : TEXCOORD3;
#endif

	float4 Alpha_Dist_UV : TEXCOORD4;
	float4 Blend_Alpha_Dist_UV : TEXCOORD5;

	// BlendUV, FlipbookNextIndexUV
	float4 Blend_FBNextIndex_UV : TEXCOORD6;

#ifndef DISABLED_SOFT_PARTICLE
	float4 PosP : TEXCOORD7;
#endif
};

#endif

#include "ad_model_common_vs.fx"

VS_Output main(const VS_Input Input)
{
#ifdef DISABLE_INSTANCE
	float4 uv = fUV;
	float4 alphaUV = fAlphaUV;
	float4 uvDistortionUV = fUVDistortionUV;
	float4 blendUV = fBlendUV;
	float4 blendAlphaUV = fBlendAlphaUV;
	float4 blendUVDistortionUV = fBlendUVDistortionUV;
	float4 modelColor = fModelColor * Input.Color;
	float flipbookIndexAndNextRate = fFlipbookIndexAndNextRate.x;
	float modelAlphaThreshold = fModelAlphaThreshold.x;
#else

#if defined(ENABLE_DIVISOR)
	int index = (int)Input.Index;
#else
	uint index = Input.Index;
#endif

	float4x4 mModel = mModel_Inst[index];
	float4 uv = fUV[index];
	float4 alphaUV = fAlphaUV[index];
	float4 uvDistortionUV = fUVDistortionUV[index];
	float4 blendUV = fBlendUV[index];
	float4 blendAlphaUV = fBlendAlphaUV[index];
	float4 blendUVDistortionUV = fBlendUVDistortionUV[index];
	float4 modelColor = fModelColor[index] * Input.Color;
	float flipbookIndexAndNextRate = fFlipbookIndexAndNextRate[index].x;
	float modelAlphaThreshold = fModelAlphaThreshold[index].x;
#endif

	VS_Output Output = (VS_Output)0;
	float4 localPosition = {Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0};

	float4 worldPos = mul(mModel, localPosition);

	Output.PosVS = mul(mCameraProj, worldPos);

	float2 outputUV = Input.UV;
	outputUV.x = outputUV.x * uv.z + uv.x;
	outputUV.y = outputUV.y * uv.w + uv.y;
	outputUV.y = mUVInversed.x + mUVInversed.y * outputUV.y;
	Output.UV_Others.xy = outputUV;

#if defined(ENABLE_LIGHTING) || defined(ENABLE_DISTORTION)
	float4 localNormal = {Input.Normal.x, Input.Normal.y, Input.Normal.z, 0.0};
	float4 localBinormal = {Input.Binormal.x, Input.Binormal.y, Input.Binormal.z, 0.0};
	float4 localTangent = {Input.Tangent.x, Input.Tangent.y, Input.Tangent.z, 0.0};

	float4 worldNormal = mul(mModel, localNormal);
	float4 worldBinormal = mul(mModel, localBinormal);
	float4 worldTangent = mul(mModel, localTangent);

	worldNormal = normalize(worldNormal);
	worldBinormal = normalize(worldBinormal);
	worldTangent = normalize(worldTangent);

#if defined(ENABLE_LIGHTING)

	Output.WorldN = worldNormal.xyz;
	Output.WorldB = worldBinormal.xyz;
	Output.WorldT = worldTangent.xyz;

#elif defined(ENABLE_DISTORTION)
	Output.ProjTangent = mul(mCameraProj, worldPos + worldTangent);
	Output.ProjBinormal = mul(mCameraProj, worldPos + worldBinormal);
#endif

#else
	// Unlit
	float4 localNormal = {Input.Normal.x, Input.Normal.y, Input.Normal.z, 0.0};
	localNormal = normalize(mul(mModel, localNormal));
	Output.WorldN = localNormal.xyz;
	
#endif
	Output.Color = modelColor;

	CalculateAndStoreAdvancedParameter(Input.UV, Output.UV_Others.xy, alphaUV, uvDistortionUV, blendUV, blendAlphaUV, blendUVDistortionUV, flipbookIndexAndNextRate, modelAlphaThreshold, Output);

#ifndef DISABLED_SOFT_PARTICLE
	Output.PosP = Output.PosVS;
#endif

	return Output;
}
