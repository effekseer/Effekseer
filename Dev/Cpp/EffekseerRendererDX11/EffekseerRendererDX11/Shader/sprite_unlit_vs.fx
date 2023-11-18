
cbuffer VS_ConstantBuffer : register(b0)
{
	float4x4 mCamera;
	float4x4 mCameraProj;
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

#if defined(ENABLE_LIGHTING) || defined(ENABLE_DISTORTION)
	float4 worldNormal = float4((Input.Normal.xyz - float3(0.5, 0.5, 0.5)) * 2.0, 0.0);
	float4 worldTangent = float4((Input.Tangent.xyz - float3(0.5, 0.5, 0.5)) * 2.0, 0.0);
	float4 worldBinormal = float4(cross(worldNormal.xyz, worldTangent.xyz), 0.0);
#endif

	float4 worldPos = {Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0};
	Output.PosVS = mul(mCameraProj, worldPos);
	Output.Color = Input.Color;

	// UV
#if defined(ENABLE_LIGHTING) || defined(ENABLE_DISTORTION)
	float2 uv1 = Input.UV1;
#else
	float2 uv1 = Input.UV;
#endif
	uv1.y = mUVInversed.x + mUVInversed.y * uv1.y;
	Output.UV = uv1;

#ifdef ENABLE_LIGHTING
	// NBT
	Output.WorldN = worldNormal.xyz;
	Output.WorldB = worldBinormal.xyz;
	Output.WorldT = worldTangent.xyz;

#elif defined(ENABLE_DISTORTION)
	Output.ProjTangent = mul(mCameraProj, worldPos + worldTangent);
	Output.ProjBinormal = mul(mCameraProj, worldPos + worldBinormal);
#endif

	Output.PosP = Output.PosVS;

	return Output;
}
