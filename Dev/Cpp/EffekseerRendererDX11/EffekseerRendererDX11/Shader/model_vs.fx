
cbuffer VS_ConstantBuffer : register(b0)
{
	float4x4 mCameraProj;
#ifdef DISABLE_INSTANCE
	float4x4 mModel;
	float4 fUV;
	float4 fModelColor;
#else
	float4x4 mModel_Inst[__INST__];
	float4 fUV[__INST__];
	float4 fModelColor[__INST__];
#endif
	float4 fLightDirection;
	float4 fLightColor;
	float4 fLightAmbient;
	float4 mUVInversed;
}

struct VS_Input
{
	float3 Pos : POSITION0;
	float3 Normal : NORMAL0;
	float3 Binormal : NORMAL1;
	float3 Tangent : NORMAL2;
	float2 UV : TEXCOORD0;
	float4 Color : NORMAL3;
#if defined(ENABLE_DIVISOR)
	float Index		: BLENDINDICES0;
#elif !defined(DISABLE_INSTANCE)
	uint Index	: SV_InstanceID;
#endif
};

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
#ifdef DISABLE_INSTANCE
	float4 uv = fUV;
	float4 modelColor = fModelColor * Input.Color;
#else

#if defined(ENABLE_DIVISOR)
	int index = (int)Input.Index;
#else
	uint index = Input.Index;
#endif

	float4x4 mModel = mModel_Inst[index];
	float4 uv = fUV[index];
	float4 modelColor = fModelColor[index] * Input.Color;
#endif

	VS_Output Output = (VS_Output)0;
	float4 localPos = {Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0};

	float4 worldPos = mul(mModel, localPos);
	Output.PosVS = mul(mCameraProj, worldPos);
	Output.Color = modelColor;

	float2 outputUV = Input.UV;
	outputUV.x = outputUV.x * uv.z + uv.x;
	outputUV.y = outputUV.y * uv.w + uv.y;
	outputUV.y = mUVInversed.x + mUVInversed.y * outputUV.y;
	Output.UV = outputUV;

#if defined(ENABLE_LIGHTING) || defined(ENABLE_DISTORTION)

	float4 localNormal = {Input.Normal.x, Input.Normal.y, Input.Normal.z, 0.0};
	float4 localBinormal = {Input.Binormal.x, Input.Binormal.y, Input.Binormal.z, 0.0};
	float4 localTangent = {Input.Tangent.x, Input.Tangent.y, Input.Tangent.z, 0.0};

	float4 worldNormal = mul(mModel, localNormal);
	float4 worldBinormal = mul(mModel, localBinormal);
	float4 worldTangent = mul(mModel, localTangent);

	worldNormal = normalize(worldNormal);
	worldBinormal = normalize(worldBinormal);
	worldTangent = normalize(worldTangent);

#if defined(ENABLE_LIGHTING)

	Output.WorldN = worldNormal.xyz;
	Output.WorldB = worldBinormal.xyz;
	Output.WorldT = worldTangent.xyz;

#elif defined(ENABLE_DISTORTION)
	Output.ProjBinormal = mul(mCameraProj, worldPos + worldBinormal);
	Output.ProjTangent = mul(mCameraProj, worldPos + worldTangent);
#endif

#endif

	Output.PosP = Output.PosVS;

	return Output;
}
