struct VS_Input
{
	float3 Pos : POSITION0;
	float2 UV : TEXCOORD0;
	float4 Color : NORMAL0;
};

struct VS_Output
{
	float4 Pos : SV_POSITION;
	float2 UV : TEXCOORD0;
	float4 Color : COLOR0;
};

float4 shift_vertex : register(c0);

VS_Output main(const VS_Input input)
{
	VS_Output output = (VS_Output)0;
	output.Pos =float4(input.Pos.x, input.Pos.y, input.Pos.z, 1.0) + shift_vertex;
	output.UV = input.UV;
	output.Color = input.Color;
	return output;
}
