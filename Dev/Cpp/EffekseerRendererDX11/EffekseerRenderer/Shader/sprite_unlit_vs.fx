
cbuffer VS_ConstantBuffer : register(b0)
{
	float4x4 mCamera;
	float4x4 mProj;
	float4 mUVInversed;

	// Unused
	float4 mflipbookParameter; // x:enable, y:loopType, z:divideX, w:divideY
}

#ifdef ENABLE_LIGHTING

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

VS_Output main(const VS_Input Input)
{
	VS_Output Output = (VS_Output)0;
	float3 worldPos = Input.Pos;

#ifdef ENABLE_LIGHTING
	float3 worldNormal = (Input.Normal - float3(0.5, 0.5, 0.5)) * 2.0;
	float3 worldTangent = (Input.Tangent - float3(0.5, 0.5, 0.5)) * 2.0;
	float3 worldBinormal = cross(worldNormal, worldTangent);
#endif

	// UV
#ifdef ENABLE_LIGHTING
	float2 uv1 = Input.UV1;
#else
	float2 uv1 = Input.UV;
#endif
	uv1.y = mUVInversed.x + mUVInversed.y * uv1.y;

	// NBT
#ifdef ENABLE_LIGHTING
	Output.WorldN = worldNormal;
	Output.WorldB = worldBinormal;
	Output.WorldT = worldTangent;
#endif

	float4 cameraPos = mul(mCamera, float4(worldPos, 1.0));
	Output.PosVS = mul(mProj, cameraPos);

	Output.Color = Input.Color;
	Output.UV = uv1;

	Output.PosP = Output.PosVS;

	return Output;
}
