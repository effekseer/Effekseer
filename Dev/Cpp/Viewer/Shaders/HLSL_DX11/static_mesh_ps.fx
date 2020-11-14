
Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);

cbuffer PS_ConstantBuffer : register(b0)
{
	float4 isLit;
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
	float4 Position : NORMAL2;
};

struct PS_OUTPUT
{
      float4 o0 : SV_TARGET0;
      float4 o1 : SV_TARGET1;
};

PS_OUTPUT main(const PS_Input Input)
{
	PS_OUTPUT output;
	output.o0 = Input.Color * g_texture.Sample(g_sampler, Input.UV);

	if(isLit.x > 0.0f)
	{
		float diffuse = max(dot(directionalLightDirection.xyz, Input.Normal.xyz), 0.0);
		output.o0.xyz = output.o0.xyz * (directionalLightColor.xyz * diffuse + ambientLightColor.xyz);
	}

	output.o1.xyzw = 1.0f;
	output.o1.x = Input.Position.z / Input.Position.w;

	return output;
}