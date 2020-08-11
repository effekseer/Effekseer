
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
	float3 Pos		: POSITION0;
	float3 Normal		: NORMAL0;
	float3 Binormal		: NORMAL1;
	float3 Tangent		: NORMAL2;
	float2 UV		: TEXCOORD0;
	float4 Color		: NORMAL3;
#ifndef DISABLE_INSTANCE
	uint4 Index		: BLENDINDICES0;
#endif
};

struct VS_Output
{
	float4 Pos : SV_POSITION;
	float2 UV : TEXCOORD0;

	float3 Normal : TEXCOORD1;
	float3 Binormal : TEXCOORD2;
	float3 Tangent : TEXCOORD3;
	float4 Color : COLOR;

	float4 Alpha_Dist_UV : TEXCOORD4;
	float4 Blend_Alpha_Dist_UV : TEXCOORD5;

	// BlendUV, FlipbookNextIndexUV
	float4 Blend_FBNextIndex_UV : TEXCOORD6;

	// x - FlipbookRate, y - AlphaThreshold
	float2 Others : TEXCOORD7;
};

#include "model_renderer_common_VS.fx"

VS_Output main( const VS_Input Input )
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
	float4x4 matModel = mModel[Input.Index.x];
	float4 uv = fUV[Input.Index.x];
    float4 alphaUV = fAlphaUV[Input.Index.x];
    float4 uvDistortionUV = fUVDistortionUV[Input.Index.x];
    float4 blendUV = fBlendUV[Input.Index.x];
    float4 blendAlphaUV = fBlendAlphaUV[Input.Index.x];
    float4 blendUVDistortionUV = fBlendUVDistortionUV[Input.Index.x];
	float4 modelColor = fModelColor[Input.Index.x] * Input.Color;
    float flipbookIndexAndNextRate = fFlipbookIndexAndNextRate[Input.Index.x].x;
    float modelAlphaThreshold = fModelAlphaThreshold[Input.Index.x].x;
#endif

	VS_Output Output = (VS_Output)0;
	float4 localPosition = { Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0 }; 

#ifdef DISABLE_INSTANCE
	float4 cameraPosition = mul(mModel, localPosition);
#else
	float4 cameraPosition = mul(matModel, localPosition);
#endif

	Output.Pos = mul( mCameraProj, cameraPosition );

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

	Output.Normal = localNormal.xyz;
	Output.Binormal = localBinormal.xyz;
	Output.Tangent = localTangent.xyz;
#else
    
#ifdef DISABLE_INSTANCE
	float4 localNormal = {Input.Normal.x, Input.Normal.y, Input.Normal.z, 0.0};
	localNormal = normalize(mul(mModel, localNormal));
#else
	float4 localNormal = {Input.Normal.x, Input.Normal.y, Input.Normal.z, 0.0};
	localNormal = normalize(mul(matModel, localNormal));
#endif
    Output.Normal = localNormal.xyz;
#endif
	Output.Color = modelColor;

	Output.UV.y = mUVInversed.x + mUVInversed.y * Output.UV.y;

	CalculateAndStoreAdvancedParameter(Output.UV, alphaUV, uvDistortionUV, blendUV, blendAlphaUV, blendUVDistortionUV, flipbookIndexAndNextRate, modelAlphaThreshold, Output);

	return Output;
}

