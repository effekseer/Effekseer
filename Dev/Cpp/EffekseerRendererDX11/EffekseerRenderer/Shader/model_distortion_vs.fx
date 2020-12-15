
cbuffer VS_ConstantBuffer : register(b0)
{
	float4x4 mCameraProj;
#ifdef DISABLE_INSTANCE
	float4x4 mModel;
	float4 fUV;
	float4 fModelColor;
#else
	float4x4 mModel[__INST__];
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
	float Index : BLENDINDICES0;
#elif !defined(DISABLE_INSTANCE)
	uint Index : SV_InstanceID;
#endif
};

struct VS_Output
{
	float4 PosVS : SV_POSITION;
	linear centroid float2 UV : TEXCOORD0;
	float4 ProjBinormal : TEXCOORD1;
	float4 ProjTangent : TEXCOORD2;
	float4 PosP : TEXCOORD3;
	linear centroid float4 Color : COLOR0;
};

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

	float4x4 matModel = mModel[index];
	float4 uv = fUV[index];
	float4 modelColor = fModelColor[index] * Input.Color;
#endif

	VS_Output Output = (VS_Output)0;
	float4 localPosition = {Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0};
	float4 localNormal = {Input.Pos.x + Input.Normal.x, Input.Pos.y + Input.Normal.y, Input.Pos.z + Input.Normal.z, 1.0};
	float4 localBinormal = {Input.Pos.x + Input.Binormal.x, Input.Pos.y + Input.Binormal.y, Input.Pos.z + Input.Binormal.z, 1.0};
	float4 localTangent = {Input.Pos.x + Input.Tangent.x, Input.Pos.y + Input.Tangent.y, Input.Pos.z + Input.Tangent.z, 1.0};

#ifdef DISABLE_INSTANCE
	localPosition = mul(mModel, localPosition);
	localNormal = mul(mModel, localNormal);
	localBinormal = mul(mModel, localBinormal);
	localTangent = mul(mModel, localTangent);
#else
	localPosition = mul(matModel, localPosition);
	localNormal = mul(matModel, localNormal);
	localBinormal = mul(matModel, localBinormal);
	localTangent = mul(matModel, localTangent);
#endif

	localNormal = localPosition + normalize(localNormal - localPosition);
	localBinormal = localPosition + normalize(localBinormal - localPosition);
	localTangent = localPosition + normalize(localTangent - localPosition);

	Output.PosVS = mul(mCameraProj, localPosition);

	Output.UV.x = Input.UV.x * uv.z + uv.x;
	Output.UV.y = Input.UV.y * uv.w + uv.y;

	Output.ProjBinormal = mul(mCameraProj, localBinormal);
	Output.ProjTangent = mul(mCameraProj, localTangent);
	Output.PosP = Output.PosVS;

	Output.Color = modelColor;

	Output.UV.y = mUVInversed.x + mUVInversed.y * Output.UV.y;

	return Output;
}
