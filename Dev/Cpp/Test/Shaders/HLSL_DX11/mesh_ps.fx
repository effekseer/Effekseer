
struct PS_Input
{
	float4 Pos : SV_POSITION;
	float2 UV : TEXCOORD0;
	float4 Color : COLOR0;
};

float4 main(const PS_Input Input): SV_Target
{
	return Input.Color;
}