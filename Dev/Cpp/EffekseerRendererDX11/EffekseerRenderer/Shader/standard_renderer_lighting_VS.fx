
struct VS_Input
{
	float3 Pos : POSITION0;
	float4 Color : NORMAL0;
	float4 Normal : NORMAL1;
	float4 Tangent : NORMAL2;
	float2 UV1 : TEXCOORD0;
	float2 UV2 : TEXCOORD1;
#ifdef __EFFEKSEER_BUILD_VERSION16__
    float2 AlphaUV : TEXCOORD2;
    float FlipbookIndex : TEXCOORD3;
    float AlphaThreshold : TEXCOORD4;
#endif
};

struct VS_Output
{
	float4 Position : SV_POSITION;
	float4 VColor : COLOR;
	float2 UV1 : TEXCOORD0;
	float2 UV2 : TEXCOORD1;
	float3 WorldP : TEXCOORD2;
	float3 WorldN : TEXCOORD3;
	float3 WorldT : TEXCOORD4;
	float3 WorldB : TEXCOORD5;
	float2 ScreenUV : TEXCOORD6;
#ifdef __EFFEKSEER_BUILD_VERSION16__
    float2 AlphaUV : TEXCOORD7;
    float FlipbookRate : TEXCOORD8;
    float2 FlipbookNextIndexUV : TEXCOORD9;
    float AlphaThreshold : TEXCOORD10;
#endif
};

#ifdef __EFFEKSEER_BUILD_VERSION16__
cbuffer VS_ConstantBuffer : register(b0)
{
    float4x4 mCamera;
    float4x4 mProj;
    float4 mUVInversed;

    float4 mflipbookParameter; // x:enable, y:loopType, z:divideX, w:divideY
};

#include "FlipbookInterpolationUtils.fx"
#else
float4x4 mCamera : register(c0);
float4x4 mProj : register(c4);
float4 mUVInversed : register(c8);
#endif

VS_Output VS( const VS_Input Input )
{
	VS_Output Output = (VS_Output)0;
	float3 worldPos = Input.Pos;
	float3 worldNormal = (Input.Normal - float3(0.5, 0.5, 0.5)) * 2.0;
	float3 worldTangent = (Input.Tangent - float3(0.5, 0.5, 0.5)) * 2.0;
	float3 worldBinormal = cross(worldNormal, worldTangent);

	// UV
	float2 uv1 = Input.UV1;
	float2 uv2 = Input.UV1;
	uv1.y = mUVInversed.x + mUVInversed.y * uv1.y;
	uv2.y = mUVInversed.x + mUVInversed.y * uv2.y;
#ifdef __EFFEKSEER_BUILD_VERSION16__
    // alpha texture
    float2 alphaUV = Input.AlphaUV;
    alphaUV.y = mUVInversed.x + mUVInversed.y * alphaUV.y;
    
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
        
        float2 OriginUV = GetFlipbookOriginUV(Input.UV1, Index, mflipbookParameter.z, mflipbookParameter.w);
        Output.FlipbookNextIndexUV = GetFlipbookUVForIndex(OriginUV, NextIndex, mflipbookParameter.z, mflipbookParameter.w);
    }
    
    // alpha threshold
    Output.AlphaThreshold = Input.AlphaThreshold;
#endif

	// NBT
	Output.WorldN = worldNormal;
	Output.WorldB = worldBinormal;
	Output.WorldT = worldTangent;

	float3 pixelNormalDir = float3(0.5, 0.5, 1.0);

	float4 cameraPos = mul(mCamera, float4(worldPos, 1.0));
	cameraPos = cameraPos / cameraPos.w;
	Output.Position = mul(mProj, cameraPos);

	Output.WorldP = worldPos;
	Output.VColor = Input.Color;
	Output.UV1 = uv1;
	Output.UV2 = uv2;
#ifdef __EFFEKSEER_BUILD_VERSION16__
    Output.AlphaUV = alphaUV;
#endif
	Output.ScreenUV = Output.Position.xy / Output.Position.w;
	Output.ScreenUV.xy = float2(Output.ScreenUV.x + 1.0, 1.0 - Output.ScreenUV.y) * 0.5;

	return Output;
}

