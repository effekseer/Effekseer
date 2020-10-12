
cbuffer VS_ConstantBuffer : register(b0)
{
	float4 fLightDirection;
	float4 fLightColor;
	float4 fLightAmbient;
}

#ifdef ENABLE_COLOR_TEXTURE
Texture2D g_colorTexture : register(t0);
SamplerState g_colorSampler : register(s0);
#endif

#ifdef ENABLE_NORMAL_TEXTURE
Texture2D g_normalTexture : register(t1);
SamplerState g_normalSampler : register(s1);
#endif

struct PS_Input
{
	float4 Pos : SV_POSITION;
	linear centroid float2 UV : TEXCOORD0;
#if ENABLE_NORMAL_TEXTURE
	half3 Normal : TEXCOORD1;
	half3 Binormal : TEXCOORD2;
	half3 Tangent : TEXCOORD3;
#endif
	linear centroid float4 Color : COLOR;
};

float4 main(const PS_Input Input)
	: SV_Target
{
	float4 Output = g_colorTexture.Sample(g_colorSampler, Input.UV) * Input.Color;

#if ENABLE_LIGHTING && ENABLE_NORMAL_TEXTURE
	half3 texNormal = (g_normalTexture.Sample(g_normalSampler, Input.UV).xyz - 0.5) * 2.0;
	half3 localNormal = (half3)normalize(
		mul(
			texNormal,
			half3x3((half3)Input.Tangent, (half3)Input.Binormal, (half3)Input.Normal)));

	float diffuse = max(dot(fLightDirection.xyz, localNormal.xyz), 0.0);
	Output.xyz = Output.xyz * (fLightColor.xyz * diffuse + fLightAmbient.xyz);
#endif

	if (Output.a == 0.0)
		discard;

	return Output;
}
