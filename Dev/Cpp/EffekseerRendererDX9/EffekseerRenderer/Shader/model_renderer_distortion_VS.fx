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
	float4 fUVDistortionUV[20];

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
float4 fUVDistortionUV[20]: register(c124);
float4 fFlipbookParameter : register(c144);
float4 fFlipbookIndexAndNextRate[20] : register(c145);
float4 fModelAlphaThreshold[20] : register(c165);
float4 fModelColor[20] : register(c185);

float4 fLightDirection : register(c205);
float4 fLightColor : register(c206);
float4 fLightAmbient : register(c207);

float4 mUVInversed : register(c208);

#else
float4x4 mCameraProj		: register( c0 );
float4x4 mModel[20]		: register( c4 );
float4	fUV[20]			: register( c84 );
float4	fModelColor[20]		: register( c104 );
float4 mUVInversed		: register(c127);
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
	float4 Position		: POSITION0;
	float2 UV		: TEXCOORD0;
//	float4 Normal		: TEXCOORD1;
	float4 Binormal		: TEXCOORD2;
	float4 Tangent		: TEXCOORD3;
	float4 Pos			: TEXCOORD4;
	float4 Color		: COLOR;
#ifdef __EFFEKSEER_BUILD_VERSION16__
    float2 AlphaUV : TEXCOORD5;
	float2 UVDistortionUV : TEXCOORD6;
    float FlipbookRate  : TEXCOORD7;
    float2 FlipbookNextIndexUV : TEXCOORD8;
    float AlphaThreshold : TEXCOORD9;
#endif
};

VS_Output VS( const VS_Input Input )
{
	float4x4 matModel = mModel[Input.Index.x];
	float4 uv = fUV[Input.Index.x];
	float4 modelColor = fModelColor[Input.Index.x];
#ifdef __EFFEKSEER_BUILD_VERSION16__
    float4 alphaUV = fAlphaUV[Input.Index.x];
	float4 uvDistortionUV = fUVDistortionUV[Input.Index.x];
#endif

	VS_Output Output = (VS_Output)0;
	float4 localPosition = { Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0 }; 
	float4 localNormal = { Input.Pos.x + Input.Normal.x, Input.Pos.y + Input.Normal.y, Input.Pos.z + Input.Normal.z, 1.0 };
	float4 localBinormal = { Input.Pos.x + Input.Binormal.x, Input.Pos.y + Input.Binormal.y, Input.Pos.z + Input.Binormal.z, 1.0 };
	float4 localTangent = { Input.Pos.x + Input.Tangent.x, Input.Pos.y + Input.Tangent.y, Input.Pos.z + Input.Tangent.z, 1.0 };


	localPosition = mul( matModel, localPosition );
	localNormal = mul(matModel, localNormal);
	localBinormal = mul(matModel, localBinormal);
	localTangent = mul(matModel, localTangent);

	localNormal = localPosition + normalize(localNormal - localPosition);
	localBinormal = localPosition + normalize(localBinormal - localPosition);
	localTangent = localPosition + normalize(localTangent - localPosition);

	Output.Position = mul( mCameraProj, localPosition );

	Output.UV.x = Input.UV.x * uv.z + uv.x;
	Output.UV.y = Input.UV.y * uv.w + uv.y;
#ifdef __EFFEKSEER_BUILD_VERSION16__
    Output.AlphaUV.x = Input.UV.x * alphaUV.z + alphaUV.x;
    Output.AlphaUV.y = Input.UV.y * alphaUV.w + alphaUV.y;
	Output.UVDistortionUV.x = Input.UV.x * uvDistortionUV.z + uvDistortionUV.x;
	Output.UVDistortionUV.y = Input.UV.y * uvDistortionUV.w + uvDistortionUV.y;
#endif

//	Output.Normal = mul(mCameraProj, localNormal);
	Output.Binormal = mul(mCameraProj, localBinormal);
	Output.Tangent = mul(mCameraProj, localTangent);
	Output.Pos = Output.Position;

	Output.Color = modelColor;

	Output.UV.y = mUVInversed.x + mUVInversed.y * Output.UV.y;

#ifdef __EFFEKSEER_BUILD_VERSION16__
    Output.AlphaUV.y =  mUVInversed.x + mUVInversed.y * Output.AlphaUV.y;
	Output.UVDistortionUV.y = mUVInversed.x + mUVInversed.y * Output.UVDistortionUV.y;

    // flipbook interpolation
	ApplyFlipbookVS(
		Output.FlipbookRate, Output.FlipbookNextIndexUV, fFlipbookParameter, fFlipbookIndexAndNextRate[Input.Index.x].x, Output.UV);

    // alpha threshold
    Output.AlphaThreshold = fModelAlphaThreshold[Input.Index.x].x;
#endif

	return Output;
}

