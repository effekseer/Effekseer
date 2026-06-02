
struct VS_Input
{
	float3 Pos : POSITION0;
	float2 UV : TEXCOORD0;
	float4 Color : NORMAL0;
};

struct VS_Output
{
	float4 Position : SV_POSITION;
	float4 Color : COLOR;
	float2 UV : TEXCOORD0;
};

VS_Output main(const VS_Input Input)
{
	VS_Output Output = (VS_Output)0;
	float4 pos4 = {Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0};
	Output.Position = pos4;
	Output.Color = Input.Color;
	Output.UV = Input.UV;

	return Output;
}
