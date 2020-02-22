#ifdef __EFFEKSEER_BUILD_VERSION16__
cbuffer VS_ConstantBuffer : register(b0)
{
    float4x4 mCameraProj;
    float4x4 mModel[40];
    float4 fUV[40];
    float4 fAlphaUV[40];

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
    float2 AlphaUV : TEXCOORD5;
    float FlipbookRate  : TEXCOORD6;
    float2 FlipbookNextIndexUV : TEXCOORD7;
    float AlphaThreshold : TEXCOORD8;
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
#endif

	Output.Normal = mul(mCameraProj, localNormal);
	Output.Binormal = mul(mCameraProj, localBinormal);
	Output.Tangent = mul(mCameraProj, localTangent);
	Output.Pos = Output.Position;

	Output.Color = modelColor;

	Output.UV.y = mUVInversed.x + mUVInversed.y * Output.UV.y;
#ifdef __EFFEKSEER_BUILD_VERSION16__
    Output.AlphaUV.y =  mUVInversed.x + mUVInversed.y * Output.AlphaUV.y;
    
    // flipbook interpolation
    if(fFlipbookParameter.x > 0)
    {
        float FlipbookIndexAndNextRate = fFlipbookIndexAndNextRate[Input.Index.x].x;
    
        Output.FlipbookRate = frac(FlipbookIndexAndNextRate);
    
        float NextIndex = floor(FlipbookIndexAndNextRate) + 1;
        
        // loop none 
        if(fFlipbookParameter.y == 0)
        {
            if (NextIndex >= fFlipbookParameter.z * fFlipbookParameter.w)
			{
				NextIndex = (fFlipbookParameter.z * fFlipbookParameter.w) - 1;
			}
        }
        // loop
        else if(fFlipbookParameter.y == 1)
        {
            NextIndex %= (fFlipbookParameter.z * fFlipbookParameter.w);
        }
        // loop reverse
        else if(fFlipbookParameter.y == 2)
        {
            bool Reverse = (floor(NextIndex) / (fFlipbookParameter.z * fFlipbookParameter.w)) % 2 == 1;
            NextIndex = int(NextIndex) % (fFlipbookParameter.z * fFlipbookParameter.w);
            if(Reverse)
            {
                NextIndex = fFlipbookParameter.z * fFlipbookParameter.w - 1 - NextIndex;
            }
        }
    
        Output.FlipbookNextIndexUV = GetFlipbookUVForIndex(Input.UV, NextIndex, fFlipbookParameter.z, fFlipbookParameter.w);
    }
    
    // alpha threshold
    Output.AlphaThreshold = fModelAlphaThreshold[Input.Index.x].x;
#endif

	return Output;
}

