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

#ifdef ENABLE_LIGHTING
float4	fLightDirection		: register( c244 );
float4	fLightColor		: register( c245 );
float4	fLightAmbient		: register( c246 );
#endif
float4 mUVInversed		: register(c247);
#endif


struct VS_Input
{
	float3 Pos		: POSITION0;
	float3 Normal		: NORMAL0;
	float3 Binormal		: NORMAL1;
	float3 Tangent		: NORMAL2;
	float2 UV		: TEXCOORD0;
	float4 Color		: NORMAL3;
	uint4 Index		: BLENDINDICES0;

};

struct VS_Output
{
	float4 Pos		: SV_POSITION;
	float2 UV		: TEXCOORD0;
    
#ifdef __EFFEKSEER_BUILD_VERSION16__
    float3 Normal		: TEXCOORD1;
	float3 Binormal		: TEXCOORD2;
	float3 Tangent		: TEXCOORD3;  
    
    float2 AlphaUV              : TEXCOORD4;
    float2 UVDistortionUV       : TEXCOORD5;
    float2 BlendUV              : TEXCOORD6;
    float2 BlendAlphaUV         : TEXCOORD7;
    float2 BlendUVDistortionUV  : TEXCOORD8;
    
    float FlipbookRate          : TEXCOORD9;
    float2 FlipbookNextIndexUV  : TEXCOORD10;
    
    float AlphaThreshold        : TEXCOORD11;
#else

#if ENABLE_NORMAL_TEXTURE
	half3 Normal		: TEXCOORD1;
	half3 Binormal		: TEXCOORD2;
	half3 Tangent		: TEXCOORD3;   
#endif
    
#endif

	float4 Color		: COLOR;
};

#ifdef __EFFEKSEER_BUILD_VERSION16__
#include "FlipbookInterpolationUtils.fx"
#endif

VS_Output VS( const VS_Input Input )
{
	float4x4 matModel = mModel[Input.Index.x];
	float4 uv = fUV[Input.Index.x];
#ifdef __EFFEKSEER_BUILD_VERSION16__
    float4 alphaUV = fAlphaUV[Input.Index.x];
    float4 uvDistortionUV = fUVDistortionUV[Input.Index.x];
    float4 blendUV = fBlendUV[Input.Index.x];
    float4 blendAlphaUV = fBlendAlphaUV[Input.Index.x];
    float4 blendUVDistortionUV = fBlendUVDistortionUV[Input.Index.x];
#endif
	float4 modelColor = fModelColor[Input.Index.x] * Input.Color;

	VS_Output Output = (VS_Output)0;
	float4 localPosition = { Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0 }; 
	localPosition = mul( matModel, localPosition );
	Output.Pos = mul( mCameraProj, localPosition );

	Output.UV.x = Input.UV.x * uv.z + uv.x;
	Output.UV.y = Input.UV.y * uv.w + uv.y;
    
#ifdef __EFFEKSEER_BUILD_VERSION16__
    // alpha texture
    Output.AlphaUV.x = Input.UV.x * alphaUV.z + alphaUV.x;
	Output.AlphaUV.y = Input.UV.y * alphaUV.w + alphaUV.y;
    
    // uv distortion texture
    Output.UVDistortionUV.x = Input.UV.x * uvDistortionUV.z + uvDistortionUV.x;
	Output.UVDistortionUV.y = Input.UV.y * uvDistortionUV.w + uvDistortionUV.y;
    
    // blend texture
    Output.BlendUV.x = Input.UV.x * blendUV.z + blendUV.x;
    Output.BlendUV.y = Input.UV.y * blendUV.w + blendUV.y;
    
    // blend alpha texture
    Output.BlendAlphaUV.x = Input.UV.x * blendAlphaUV.z + blendAlphaUV.x;
    Output.BlendAlphaUV.y = Input.UV.y * blendAlphaUV.w + blendAlphaUV.y;
    
    // blend uv distortion texture
    Output.BlendUVDistortionUV.x = Input.UV.x * blendUVDistortionUV.z + blendUVDistortionUV.x;
    Output.BlendUVDistortionUV.y = Input.UV.y * blendUVDistortionUV.w + blendUVDistortionUV.y;
    
    // flipbook interpolation
	ApplyFlipbookVS(
		Output.FlipbookRate, Output.FlipbookNextIndexUV, fFlipbookParameter, fFlipbookIndexAndNextRate[Input.Index.x].x, Output.UV);
    
    // alpha threshold
    Output.AlphaThreshold = fModelAlphaThreshold[Input.Index.x].x;
#endif

#if ENABLE_LIGHTING
	float3x3 lightMat = (float3x3)matModel;
	//lightMat = transpose( lightMat );

	float4 localNormal = { 0.0, 0.0, 0.0, 1.0 };
	localNormal.xyz = normalize( mul( lightMat, Input.Normal ) );

	float4 localBinormal = { 0.0, 0.0, 0.0, 1.0 };
	localBinormal.xyz = normalize( mul( lightMat, Input.Binormal ) );

	float4 localTangent = { 0.0, 0.0, 0.0, 1.0 };
	localTangent.xyz = normalize( mul( lightMat, Input.Tangent ) );
	Output.Normal = localNormal.xyz;
	Output.Binormal = localBinormal.xyz;
	Output.Tangent = localTangent.xyz;
#else
    
#ifdef __EFFEKSEER_BUILD_VERSION16__
    Output.Normal   = normalize( mul((float3x3)matModel, Input.Normal) );
	Output.Binormal = normalize( mul((float3x3)matModel, Input.Binormal) );
	Output.Tangent  = normalize( mul((float3x3)matModel, Input.Tangent) );
#endif
    
#endif
	Output.Color = modelColor;

	Output.UV.y = mUVInversed.x + mUVInversed.y * Output.UV.y;
#ifdef __EFFEKSEER_BUILD_VERSION16__
    Output.AlphaUV.y = mUVInversed.x + mUVInversed.y * Output.AlphaUV.y;
    Output.UVDistortionUV.y = mUVInversed.x + mUVInversed.y * Output.UVDistortionUV.y;
    Output.BlendUV.y = mUVInversed.x + mUVInversed.y * Output.BlendUV.y;
    Output.BlendAlphaUV.y = mUVInversed.x + mUVInversed.y * Output.BlendAlphaUV.y;
    Output.BlendUVDistortionUV.y = mUVInversed.x + mUVInversed.y * Output.BlendUVDistortionUV.y;
#endif

	return Output;
}

