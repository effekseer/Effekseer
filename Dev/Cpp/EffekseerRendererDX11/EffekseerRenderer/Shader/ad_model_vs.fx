
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
	float4x4 mModel[__INST__];
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

struct VS_Output
{
	float4 PosVS : SV_POSITION;
	linear centroid float4 Color : COLOR;
	linear centroid float2 UV : TEXCOORD0;
	float3 WorldN : TEXCOORD1;
#if ENABLE_LIGHTING
	float3 WorldB : TEXCOORD2;
	float3 WorldT : TEXCOORD3;
#endif

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

	float4x4 matModel = mModel[index];
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

#ifdef DISABLE_INSTANCE
	float4 cameraPosition = mul(mModel, localPosition);
#else
	float4 cameraPosition = mul(matModel, localPosition);
#endif

	Output.PosVS = mul(mCameraProj, cameraPosition);

	Output.UV.x = Input.UV.x * uv.z + uv.x;
	Output.UV.y = Input.UV.y * uv.w + uv.y;

#if ENABLE_LIGHTING

#ifdef DISABLE_INSTANCE
	float4 localNormal = {Input.Normal.x, Input.Normal.y, Input.Normal.z, 0.0};
	localNormal = normalize(mul(mModel, localNormal));

	float4 localBinormal = {Input.Binormal.x, Input.Binormal.y, Input.Binormal.z, 0.0};
	localBinormal = normalize(mul(mModel, localBinormal));

	float4 localTangent = {Input.Tangent.x, Input.Tangent.y, Input.Tangent.z, 0.0};
	localTangent = normalize(mul(mModel, localTangent));
#else
	float4 localNormal = {Input.Normal.x, Input.Normal.y, Input.Normal.z, 0.0};
	localNormal = normalize(mul(matModel, localNormal));

	float4 localBinormal = {Input.Binormal.x, Input.Binormal.y, Input.Binormal.z, 0.0};
	localBinormal = normalize(mul(matModel, localBinormal));

	float4 localTangent = {Input.Tangent.x, Input.Tangent.y, Input.Tangent.z, 0.0};
	localTangent = normalize(mul(matModel, localTangent));
#endif

	Output.WorldN = localNormal.xyz;
	Output.WorldB = localBinormal.xyz;
	Output.WorldT = localTangent.xyz;
#else

#ifdef DISABLE_INSTANCE
	float4 localNormal = {Input.Normal.x, Input.Normal.y, Input.Normal.z, 0.0};
	localNormal = normalize(mul(mModel, localNormal));
#else
	float4 localNormal = {Input.Normal.x, Input.Normal.y, Input.Normal.z, 0.0};
	localNormal = normalize(mul(matModel, localNormal));
#endif
	Output.WorldN = localNormal.xyz;
#endif
	Output.Color = modelColor;

	Output.UV.y = mUVInversed.x + mUVInversed.y * Output.UV.y;

	CalculateAndStoreAdvancedParameter(Input.UV, Output.UV.xy, alphaUV, uvDistortionUV, blendUV, blendAlphaUV, blendUVDistortionUV, flipbookIndexAndNextRate, modelAlphaThreshold, Output);

#ifndef DISABLED_SOFT_PARTICLE
	Output.PosP = Output.PosVS;
#endif

	return Output;
}
