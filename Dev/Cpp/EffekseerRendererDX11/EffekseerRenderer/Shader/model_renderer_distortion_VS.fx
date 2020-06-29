#ifdef __EFFEKSEER_BUILD_VERSION16__
cbuffer VS_ConstantBuffer : register(b0)
{
    float4x4 mCameraProj;
    float4x4 mModel[40];
    float4 fUV[40];
    float4 fAlphaUV[40];
    float4 fUVDistortionUV[40];
    float4 fBlendUV[40];
    float4 fBlendAlphaUV[40];
    float4 fBlendUVDistortionUV[40];

    float4 fFlipbookParameter; // x:enable, y:loopType, z:divideX, w:divideY
    float4 fFlipbookIndexAndNextRate[40];

    float4 fModelAlphaThreshold[40];

    float4 fModelColor[40];

    float4 fLightDirection;
    float4 fLightColor;
    float4 fLightAmbient;
    
    float4 mUVInversed;
};
#else
float4x4 mCameraProj		: register( c0 );
float4x4 mModel[40]		: register( c4 );
float4	fUV[40]			: register( c164 );
float4	fModelColor[40]		: register( c204 );
float4 mUVInversed		: register(c247);
#endif

struct VS_Input
{
	float3 Pos		: POSITION0;
	float3 Normal		: NORMAL0;
	float3 Binormal		: NORMAL1;
	float3 Tangent		: NORMAL2;
	float2 UV		: TEXCOORD0;
	uint4 Index		: BLENDINDICES0;

};

struct VS_Output
{
	float4 Position		: SV_POSITION;
	float2 UV		: TEXCOORD0;
	float4 Normal		: TEXCOORD1;
	float4 Binormal		: TEXCOORD2;
	float4 Tangent		: TEXCOORD3;
	float4 Pos		: TEXCOORD4;
	float4 Color		: COLOR0;
#ifdef __EFFEKSEER_BUILD_VERSION16__
    float2 AlphaUV              : TEXCOORD5;
    float2 UVDistortionUV       : TEXCOORD6;
    float2 BlendUV              : TEXCOORD7;
    float2 BlendAlphaUV         : TEXCOORD8;
    float2 BlendUVDistortionUV  : TEXCOORD9;
    
    float FlipbookRate          : TEXCOORD10;
    float2 FlipbookNextIndexUV  : TEXCOORD11;
    float AlphaThreshold        : TEXCOORD12;
#endif
};

#ifdef __EFFEKSEER_BUILD_VERSION16__
#include "FlipbookInterpolationUtils.fx"
#endif

VS_Output VS( const VS_Input Input )
{
	float4x4 matModel = mModel[Input.Index.x];
		float4 uv = fUV[Input.Index.x];
		float4 modelColor = fModelColor[Input.Index.x];
#ifdef __EFFEKSEER_BUILD_VERSION16__
    float4 alphaUV = fAlphaUV[Input.Index.x];
    float4 uvDistortionUV = fUVDistortionUV[Input.Index.x];
    float4 blendUV = fBlendUV[Input.Index.x];
    float4 blendAlphaUV = fBlendAlphaUV[Input.Index.x];
    float4 blendUVDistortionUV = fBlendUVDistortionUV[Input.Index.x];
#endif

		VS_Output Output = (VS_Output) 0;
	float4 localPosition = { Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0 };
		float4 localNormal = { Input.Pos.x + Input.Normal.x, Input.Pos.y + Input.Normal.y, Input.Pos.z + Input.Normal.z, 1.0 };
		float4 localBinormal = { Input.Pos.x + Input.Binormal.x, Input.Pos.y + Input.Binormal.y, Input.Pos.z + Input.Binormal.z, 1.0 };
		float4 localTangent = { Input.Pos.x + Input.Tangent.x, Input.Pos.y + Input.Tangent.y, Input.Pos.z + Input.Tangent.z, 1.0 };


	localPosition = mul(matModel, localPosition);
	localNormal = mul(matModel, localNormal);
	localBinormal = mul(matModel, localBinormal);
	localTangent = mul(matModel, localTangent);

	localNormal = localPosition + normalize(localNormal - localPosition);
	localBinormal = localPosition + normalize(localBinormal - localPosition);
	localTangent = localPosition + normalize(localTangent - localPosition);

	Output.Position = mul(mCameraProj, localPosition);

	Output.UV.x = Input.UV.x * uv.z + uv.x;
	Output.UV.y = Input.UV.y * uv.w + uv.y;
#ifdef __EFFEKSEER_BUILD_VERSION16__
    Output.AlphaUV.x = Input.UV.x * alphaUV.z + alphaUV.x;
    Output.AlphaUV.y = Input.UV.y * alphaUV.w + alphaUV.y;
    Output.UVDistortionUV.x = Input.UV.x * uvDistortionUV.z + uvDistortionUV.x;
    Output.UVDistortionUV.y = Input.UV.y * uvDistortionUV.w + uvDistortionUV.y;
    Output.BlendUV.x = Input.UV.x * blendUV.z + blendUV.x;
    Output.BlendUV.y = Input.UV.y * blendUV.w + blendUV.y;
    Output.BlendAlphaUV.x = Input.UV.x * blendAlphaUV.z + blendAlphaUV.x;
    Output.BlendAlphaUV.y = Input.UV.y * blendAlphaUV.w + blendAlphaUV.y;
    Output.BlendUVDistortionUV.x = Input.UV.x * blendUVDistortionUV.z + blendUVDistortionUV.x;
    Output.BlendUVDistortionUV.y = Input.UV.y * blendUVDistortionUV.w + blendUVDistortionUV.y;
#endif

	Output.Normal = mul(mCameraProj, localNormal);
	Output.Binormal = mul(mCameraProj, localBinormal);
	Output.Tangent = mul(mCameraProj, localTangent);
	Output.Pos = Output.Position;

	Output.Color = modelColor;

	Output.UV.y = mUVInversed.x + mUVInversed.y * Output.UV.y;
#ifdef __EFFEKSEER_BUILD_VERSION16__
    Output.AlphaUV.y =  mUVInversed.x + mUVInversed.y * Output.AlphaUV.y;
    Output.UVDistortionUV.y =  mUVInversed.x + mUVInversed.y * Output.UVDistortionUV.y;
    Output.BlendUV.y = mUVInversed.x + mUVInversed.y * Output.BlendUV.y;
    Output.BlendAlphaUV.y = mUVInversed.x + mUVInversed.y * Output.BlendAlphaUV.y;
    Output.BlendUVDistortionUV.y = mUVInversed.x + mUVInversed.y * Output.BlendUVDistortionUV.y;
    
    // flipbook interpolation
	ApplyFlipbookVS(
		Output.FlipbookRate, Output.FlipbookNextIndexUV, fFlipbookParameter, fFlipbookIndexAndNextRate[Input.Index.x].x, Output.UV);
    
    // alpha threshold
    Output.AlphaThreshold = fModelAlphaThreshold[Input.Index.x].x;
#endif

	return Output;
}

