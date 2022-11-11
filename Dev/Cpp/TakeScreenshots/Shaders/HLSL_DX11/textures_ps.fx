
Texture2D g_texture1 : register(t0);
SamplerState g_sampler1 : register(s0);

Texture2DArray g_texture2 : register(t1);
SamplerState g_sampler2 : register(s1);

Texture3D g_texture3 : register(t2);
SamplerState g_sampler3 : register(s2);

struct PS_Input
{
	float4 Pos : SV_POSITION;
	float2 UV : TEXCOORD0;
	float4 Color : COLOR0;
};

float4 main(const PS_Input Input): SV_Target
{
	if(Input.UV.x < 0.3)
	{
		return g_texture1.Sample(g_sampler1, Input.UV);
	}
	else if(Input.UV.x < 0.6)
	{
		return g_texture2.Sample(g_sampler2, float3(Input.UV, 1));
	}
	return g_texture3.Sample(g_sampler3, float3(Input.UV, 0.5f));
}