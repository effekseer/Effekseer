
cbuffer PS_ConstantBuffer : register(b0)
{
	float4 directionalLightDirection;
	float4 directionalLightColor;
	float4 ambientLightColor;
}

struct PS_Input
{
	float4 Pos : SV_POSITION;
	float2 UV : TEXCOORD0;
	float4 Color : NORMAL0;
	float3 Normal : NORMAL1;
};

float4 main(const PS_Input Input)
	: SV_Target
{
	float4 output = Input.Color;
	float diffuse = max(dot(directionalLightDirection.xyz, Input.Normal.xyz), 0.0);
	output.xyz = output.xyz * (directionalLightColor.xyz * diffuse + ambientLightColor.xyz);

	if (output.a == 0.0)
		discard;

	return output;
}