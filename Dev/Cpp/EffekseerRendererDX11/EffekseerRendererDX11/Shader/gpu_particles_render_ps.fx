#include "gpu_particles_utils.fx"

cbuffer cb0 : register(b0)
{
    RenderConstants constants;
};
cbuffer cb1 : register(b1)
{
    ParameterData paramData;
};

Texture2D<float4> ColorTex : register(t2);
SamplerState ColorSamp : register(s2);
Texture2D<float4> NormalTex : register(t3);
SamplerState NormalSamp : register(s3);

struct PS_Input
{
	float4 Pos : SV_POSITION;
    float2 UV : TEXCOORD0;
    float4 Color : COLOR0;
	float3 WorldN : TEXCOORD1;
	float3 WorldB : TEXCOORD2;
	float3 WorldT : TEXCOORD3;
};

float4 main(const PS_Input input) : SV_Target
{
    float4 color = input.Color * ColorTex.Sample(ColorSamp, input.UV);

	if (paramData.MaterialType == 1) {
		float3 texNormal = NormalTex.Sample(NormalSamp, input.UV).xyz * 2.0f - 1.0f;
		float3 normal = normalize(mul(texNormal, float3x3(input.WorldT, input.WorldB, input.WorldN)));
		float diffuse = max(dot(constants.LightDir, normal), 0.0);
		color.xyz *= constants.LightColor.xyz * diffuse + constants.LightAmbient.xyz;
	}

    return color;
}
