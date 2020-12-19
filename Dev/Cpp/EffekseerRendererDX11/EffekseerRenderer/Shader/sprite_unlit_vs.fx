
cbuffer VS_ConstantBuffer : register(b0)
{
	float4x4 mCamera;
	float4x4 mProj;
	float4 mUVInversed;

	// Unused
	float4 mflipbookParameter; // x:enable, y:loopType, z:divideX, w:divideY
}

#if defined(ENABLE_LIGHTING) || defined(ENABLE_DISTORTION)

struct VS_Input
{
	float3 Pos : POSITION0;
	float4 Color : NORMAL0;
	float4 Normal : NORMAL1;
	float4 Tangent : NORMAL2;
	float2 UV1 : TEXCOORD0;
	float2 UV2 : TEXCOORD1;
};

#else

struct VS_Input
{
	float3 Pos : POSITION0;
	float4 Color : NORMAL0;
	float2 UV : TEXCOORD0;
};

#endif

#if defined(ENABLE_DISTORTION)

struct VS_Output
{
	float4 PosVS : SV_POSITION;
	linear centroid float2 UV : TEXCOORD0;
	float4 ProjBinormal : TEXCOORD1;
	float4 ProjTangent : TEXCOORD2;
	float4 PosP : TEXCOORD3;
	linear centroid float4 Color : COLOR0;
};

#else

struct VS_Output
{
	float4 PosVS : SV_POSITION;
	linear centroid float4 Color : COLOR;
	linear centroid float2 UV : TEXCOORD0;

#ifdef ENABLE_LIGHTING
	float3 WorldN : TEXCOORD1;
	float3 WorldB : TEXCOORD2;
	float3 WorldT : TEXCOORD3;
#endif

	float4 PosP : TEXCOORD4;
};

#endif

VS_Output main(const VS_Input Input)
{
	VS_Output Output = (VS_Output)0;
	float3 worldPos = Input.Pos;

#if defined(ENABLE_LIGHTING) || defined(ENABLE_DISTORTION)
	float3 worldNormal = (Input.Normal - float3(0.5, 0.5, 0.5)) * 2.0;
	float3 worldTangent = (Input.Tangent - float3(0.5, 0.5, 0.5)) * 2.0;
	float3 worldBinormal = cross(worldNormal, worldTangent);
#endif

	float4 pos4 = {Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0};
	float4 cameraPos = mul(mCamera, pos4);
	Output.PosVS = mul(mProj, cameraPos);

	Output.PosP = Output.PosVS;

	// UV
#if defined(ENABLE_LIGHTING) || defined(ENABLE_DISTORTION)
	float2 uv1 = Input.UV1;
#else
	float2 uv1 = Input.UV;
#endif
	uv1.y = mUVInversed.x + mUVInversed.y * uv1.y;
	Output.UV = uv1;

	// NBT or distortion
#ifdef ENABLE_LIGHTING
	Output.WorldN = worldNormal;
	Output.WorldB = worldBinormal;
	Output.WorldT = worldTangent;

#elif defined(ENABLE_DISTORTION)
	float4 localTangent = pos4;
	float4 localBinormal = pos4;
	localTangent.xyz += worldTangent;
	localBinormal.xyz += worldBinormal;
	
	Output.ProjTangent = mul(mProj, mul(mCamera, localTangent));
	Output.ProjBinormal = mul(mProj, mul(mCamera, localBinormal));
#endif

	Output.Color = Input.Color;


	return Output;
}
