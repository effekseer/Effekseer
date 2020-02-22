
#ifdef __EFFEKSEER_BUILD_VERSION16__
cbuffer VS_ConstantBuffer : register(b0)
{
    float4x4 mCamera;
    float4x4 mProj;
    float4 mUVInversed;

    float4 mflipbookParameter; // x:enable, y:loopType, z:divideX, w:divideY
};
#else
float4x4 mCamera		: register(c0);
float4x4 mProj			: register(c4);
float4 mUVInversed		: register(c8);
#endif

struct VS_Input
{
	float3 Pos		: POSITION0;
	float4 Color		: NORMAL0;
	float2 UV		: TEXCOORD0;

	float3 Binormal		: NORMAL1;
	float3 Tangent		: NORMAL2;
    
#ifdef __EFFEKSEER_BUILD_VERSION16__
    float2 AlphaUV  : TEXCOORD1;
    float FlipbookIndex : TEXCOORD2;
    float AlphaThreshold : TEXCOORD3;
#endif
};

struct VS_Output
{
	float4 Position		: SV_POSITION;
	float4 Color		: COLOR;
	float2 UV		: TEXCOORD0;

	float4 Pos		: TEXCOORD1;
	float4 PosU		: TEXCOORD2;
	float4 PosR		: TEXCOORD3;
    
#ifdef __EFFEKSEER_BUILD_VERSION16__
    float2 AlphaUV  : TEXCOORD4;
    float FlipbookRate : TEXCOORD5;
    float2 FlipbookNextIndexUV : TEXCOORD6;
    float AlphaThreshold : TEXCOORD7;
#endif
};

#ifdef __EFFEKSEER_BUILD_VERSION16__
#include "FlipbookInterpolationUtils.fx"
#endif

VS_Output VS( const VS_Input Input )
{
	VS_Output Output = (VS_Output)0;
	float4 pos4 = { Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0 };

	float4 localBinormal = { Input.Pos.x + Input.Binormal.x, Input.Pos.y + Input.Binormal.y, Input.Pos.z + Input.Binormal.z, 1.0 };
	float4 localTangent = { Input.Pos.x + Input.Tangent.x, Input.Pos.y + Input.Tangent.y, Input.Pos.z + Input.Tangent.z, 1.0 };
	localBinormal = mul(mCamera, localBinormal);
	localTangent = mul(mCamera, localTangent);

	float4 cameraPos = mul(mCamera, pos4);
		cameraPos = cameraPos / cameraPos.w;

	localBinormal = localBinormal / localBinormal.w;
	localTangent = localTangent / localTangent.w;

	localBinormal = cameraPos + normalize(localBinormal - cameraPos);
	localTangent = cameraPos + normalize(localTangent - cameraPos);


	Output.Position = mul(mProj, cameraPos);

	Output.Pos = Output.Position;

	Output.PosU = mul(mProj, localBinormal);
	Output.PosR = mul(mProj, localTangent);

	Output.PosU /= Output.PosU.w;
	Output.PosR /= Output.PosR.w;
	Output.Pos /= Output.Pos.w;

	Output.Color = Input.Color;
	Output.UV = Input.UV;

	Output.UV.y = mUVInversed.x + mUVInversed.y * Input.UV.y;
    
#ifdef __EFFEKSEER_BUILD_VERSION16__
    Output.AlphaUV = Input.AlphaUV;
    Output.AlphaUV.y = mUVInversed.x + mUVInversed.y * Input.AlphaUV.y;
    
    // flipbook interpolation
    if(mflipbookParameter.x > 0)
    {
        Output.FlipbookRate = frac(Input.FlipbookIndex);
        
        float Index = floor(Input.FlipbookIndex);
        float IndexOffset = 1.0;
    
        float NextIndex = Input.FlipbookIndex + IndexOffset;
        
        // loop none 
        if(mflipbookParameter.y == 0)
        {
            if (NextIndex >= mflipbookParameter.z * mflipbookParameter.w)
			{
				NextIndex = (mflipbookParameter.z * mflipbookParameter.w) - 1;
                Index = (mflipbookParameter.z * mflipbookParameter.w) - 1;
			}
        }
        // loop
        else if(mflipbookParameter.y == 1)
        {
            NextIndex %= (mflipbookParameter.z * mflipbookParameter.w);
        }
        // loop reverse
        else if(mflipbookParameter.y == 2)
        {
            bool Reverse = (floor(NextIndex) / (mflipbookParameter.z * mflipbookParameter.w)) % 2 == 1;
            NextIndex = int(NextIndex) % (mflipbookParameter.z * mflipbookParameter.w);
            if(Reverse)
            {
                NextIndex = mflipbookParameter.z * mflipbookParameter.w - 1 - NextIndex;
            }
        }
        
        float2 OriginUV = GetFlipbookOriginUV(Input.UV, Index, mflipbookParameter.z, mflipbookParameter.w);
        Output.FlipbookNextIndexUV = GetFlipbookUVForIndex(OriginUV, NextIndex, mflipbookParameter.z, mflipbookParameter.w);
    }
    
    // alpha threshold
    Output.AlphaThreshold = Input.AlphaThreshold;
#endif

	return Output;
}
