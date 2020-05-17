// 1.6
#ifdef __EFFEKSEER_BUILD_VERSION16__
#include "FlipbookInterpolationUtils.fx"
#endif

#ifdef __EFFEKSEER_BUILD_VERSION16__
/* hlsl3.0 must specify register(sort automatically)
cbuffer VS_ConstantBuffer : register(b0)
{
    float4x4 mCameraProj;
    float4x4 mModel[20];
    float4 fUV[20];
    float4 fAlphaUV[20];

    float4 fFlipbookParameter; // x:enable, y:loopType, z:divideX, w:divideY
    float4 fFlipbookIndexAndNextRate[20];

    float4 fModelAlphaThreshold[20];

    float4 fModelColor[20];
    
    float4 fLightDirection;
    float4 fLightColor;
    float4 fLightAmbient;
    
    float4 mUVInversed;
};
*/

float4x4 mCameraProj : register(c0);
float4x4 mModel[20] : register(c4);
float4 fUV[20] : register(c84);

float4 fAlphaUV[20] : register(c104);
float4 fFlipbookParameter : register(c124);
float4 fFlipbookIndexAndNextRate[20] : register(c125);
float4 fModelAlphaThreshold[20] : register(c145);
float4 fModelColor[20] : register(c165);

float4 fLightDirection : register(c185);
float4 fLightColor : register(c186);
float4 fLightAmbient : register(c187);

float4 mUVInversed : register(c188);

#else

float4x4 mCameraProj		: register( c0 );
float4x4 mModel[20]		: register( c4 );
float4	fUV[20]			: register( c84 );
float4	fModelColor[20]		: register( c104 );

#ifdef ENABLE_LIGHTING
float4	fLightDirection		: register( c124 );
float4	fLightColor		: register( c125 );
float4	fLightAmbient		: register( c126 );
#endif
float4 mUVInversed		: register(c127);

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
	float4 Pos		: POSITION0;
	float2 UV		: TEXCOORD0;
#if ENABLE_NORMAL_TEXTURE
	 half3 Normal		: TEXCOORD1;
	half3 Binormal		: TEXCOORD2;
	half3 Tangent		: TEXCOORD3;

#ifdef __EFFEKSEER_BUILD_VERSION16__
    float2 AlphaUV      : TEXCOORD4;
    
    float FlipbookRate  : TEXCOORD5;
    float2 FlipbookNextIndexUV : TEXCOORD6;
    
    float AlphaThreshold : TEXCOORD7;
#endif

#else

#ifdef __EFFEKSEER_BUILD_VERSION16__
    float2 AlphaUV      : TEXCOORD1;
    
    float FlipbookRate  : TEXCOORD2;
    float2 FlipbookNextIndexUV : TEXCOORD3;
    
    float AlphaThreshold : TEXCOORD4;
#endif


#endif
	float4 Color		: COLOR;
};

VS_Output VS( const VS_Input Input )
{
	float4x4 matModel = mModel[Input.Index.x];
	float4 uv = fUV[Input.Index.x];
#ifdef __EFFEKSEER_BUILD_VERSION16__
    float4 alphaUV = fAlphaUV[Input.Index.x];
#endif
	float4 modelColor = fModelColor[Input.Index.x] * Input.Color;

	VS_Output Output = (VS_Output)0;
	float4 localPosition = { Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0 }; 
	localPosition = mul( matModel, localPosition );
	Output.Pos = mul( mCameraProj, localPosition );

	Output.Color = modelColor;

	Output.UV.x = Input.UV.x * uv.z + uv.x;
	Output.UV.y = Input.UV.y * uv.w + uv.y;

#ifdef __EFFEKSEER_BUILD_VERSION16__
    // alpha texture
    Output.AlphaUV.x = Input.UV.x * alphaUV.z + alphaUV.x;
	Output.AlphaUV.y = Input.UV.y * alphaUV.w + alphaUV.y;
    
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
#endif

#ifdef __EFFEKSEER_BUILD_VERSION16__
    Output.AlphaUV.y = mUVInversed.x + mUVInversed.y * Output.AlphaUV.y;
#endif

	return Output;
}

