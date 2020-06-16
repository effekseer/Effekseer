#ifdef __EFFEKSEER_BUILD_VERSION16__
cbuffer VS_ConstantBuffer : register(b0)
{
    float4x4 mCameraProj;
    float4x4 mModel[40];
    float4 fUV[40];
    float4 fAlphaUV[40];
    float4 fUVDistortionUV[40];
    float4 fBlendUV[40];

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
#if ENABLE_NORMAL_TEXTURE
	 half3 Normal		: TEXCOORD1;
	half3 Binormal		: TEXCOORD2;
	half3 Tangent		: TEXCOORD3;
    
#ifdef __EFFEKSEER_BUILD_VERSION16__
    float2 AlphaUV              : TEXCOORD4;
    float2 UVDistortionUV       : TEXCOORD5;
    float2 BlendUV              : TEXCOORD6;
    
    float FlipbookRate          : TEXCOORD7;
    float2 FlipbookNextIndexUV  : TEXCOORD8;
    
    float AlphaThreshold        : TEXCOORD9;
#endif
    
#else // else ENABLE_NORMAL_TEXTURE
    
#ifdef __EFFEKSEER_BUILD_VERSION16__
    float2 AlphaUV              : TEXCOORD1;
    float2 UVDistortionUV       : TEXCOORD2;
    float2 BlendUV              : TEXCOORD3;
    
    float FlipbookRate          : TEXCOORD4;
    float2 FlipbookNextIndexUV  : TEXCOORD5;
    
    float AlphaThreshold        : TEXCOORD6;
#endif
    
#endif // end ENABLE_NORMAL_TEXTURE
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
    
    // flipbook interpolation
	ApplyFlipbookVS(
		Output.FlipbookRate, Output.FlipbookNextIndexUV, fFlipbookParameter, fFlipbookIndexAndNextRate[Input.Index.x].x, Output.UV);
	/*
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
    */
    
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
	Output.Color = modelColor;

	Output.UV.y = mUVInversed.x + mUVInversed.y * Output.UV.y;
#ifdef __EFFEKSEER_BUILD_VERSION16__
    Output.AlphaUV.y = mUVInversed.x + mUVInversed.y * Output.AlphaUV.y;
    Output.UVDistortionUV.y = mUVInversed.x + mUVInversed.y * Output.UVDistortionUV.y;
    Output.BlendUV.y = mUVInversed.x + mUVInversed.y * Output.BlendUV.y;
#endif

	return Output;
}

