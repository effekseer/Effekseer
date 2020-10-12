struct PS_Input
{
	float4 Position : SV_POSITION;
	linear centroid float4 VColor : COLOR;
	linear centroid float2 UV1 : TEXCOORD0;
	linear centroid float2 UV2 : TEXCOORD1;
	float3 WorldP : TEXCOORD2;
	float3 WorldN : TEXCOORD3;
	float3 WorldT : TEXCOORD4;
	float3 WorldB : TEXCOORD5;
	float2 ScreenUV : TEXCOORD6;
};

cbuffer VS_ConstantBuffer : register(b0)
{
	float4 fLightDirection;
	float4 fLightColor;
	float4 fLightAmbient;

	// Unused
	float4 fFlipbookParameter; // x:enable, y:interpolationType

	float4 fUVDistortionParameter; // x:intensity, y: blendIntensity, zw:uvInversed
	float4 fBlendTextureParameter; // x:blendType

	float4 fEmissiveScaling; // x:emissiveScaling

	float4 fEdgeColor;
	float4 fEdgeParameter; // x:threshold, y:colorScaling
}

Texture2D g_colorTexture : register(t0);
SamplerState g_colorSampler : register(s0);

Texture2D g_normalTexture : register(t1);
SamplerState g_normalSampler : register(s1);

float4 main(const PS_Input Input)
	: SV_Target
{
	half3 loN = g_normalTexture.Sample(g_normalSampler, Input.UV1).xyz;
	half3 texNormal = (loN - 0.5) * 2.0;
	half3 localNormal = (half3)normalize(mul(texNormal, half3x3((half3)Input.WorldT, (half3)Input.WorldB, (half3)Input.WorldN)));

	float diffuse = max(dot(fLightDirection.xyz, localNormal.xyz), 0.0);

	float4 Output = g_colorTexture.Sample(g_colorSampler, Input.UV1) * Input.VColor;
	Output.xyz = Output.xyz * (fLightColor.xyz * diffuse + fLightAmbient);

	if (Output.a == 0.0)
		discard;

	return Output;
}
