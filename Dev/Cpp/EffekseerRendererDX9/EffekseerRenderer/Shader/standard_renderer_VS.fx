// 1.6
#ifdef __EFFEKSEER_BUILD_VERSION16__
#include "FlipbookInterpolationUtils.fx"
#endif


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
#ifdef __EFFEKSEER_BUILD_VERSION16__
	float2 AlphaUV : TEXCOORD1;
	float FlipbookIndex : TEXCOORD2;
	float AlphaThreshold : TEXCOORD3;
#endif
};

struct VS_Output
{
	float4 Position		: POSITION0;
	float4 Color		: COLOR;
	float2 UV		: TEXCOORD0;

	float4 Pos		: TEXCOORD1;
	float4 PosU		: TEXCOORD2;
	float4 PosR		: TEXCOORD3;

#ifdef __EFFEKSEER_BUILD_VERSION16__
	float2 AlphaUV : TEXCOORD4;
	float FlipbookRate : TEXCOORD5;
	float2 FlipbookNextIndexUV : TEXCOORD6;
	float AlphaThreshold : TEXCOORD7;
#endif
};

VS_Output VS( const VS_Input Input )
{
	VS_Output Output = (VS_Output)0;
	float4 pos4 = { Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0 };

	float4 cameraPos = mul(mCamera, pos4);
	cameraPos = cameraPos / cameraPos.w;
	Output.Position = mul(mProj, cameraPos);

	Output.Pos = Output.Position;

	float4 cameraPosU = cameraPos + float4(0.0, 1.0, 0.0, 0.0);
	float4 cameraPosR = cameraPos + float4(1.0, 0.0, 0.0, 0.0);
	Output.PosU = mul(mProj, cameraPosU);
	Output.PosR = mul(mProj, cameraPosR);

	Output.PosU /= Output.PosU.w;
	Output.PosR /= Output.PosR.w;
	Output.Pos /= Output.Pos.w;

	Output.Color = Input.Color;
	Output.UV = Input.UV;

	Output.UV.y = mUVInversed.x + mUVInversed.y * Input.UV.y;

#ifdef __EFFEKSEER_BUILD_VERSION16__
    // alpha texture
	Output.AlphaUV = Input.AlphaUV;
	Output.AlphaUV.y = mUVInversed.x + mUVInversed.y * Input.AlphaUV.y;
    
	ApplyFlipbookVS(Output.FlipbookRate, Output.FlipbookNextIndexUV, mflipbookParameter, Input.FlipbookIndex, Output.UV);

    Output.AlphaThreshold = Input.AlphaThreshold;
#endif

	return Output;
}
