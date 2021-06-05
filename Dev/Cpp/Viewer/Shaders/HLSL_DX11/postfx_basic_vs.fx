
struct VS_Input
{
	float2 Pos : POSITION0;
	float2 UV : TEXCOORD0;
};

struct VS_Output
{
	float4 Position : SV_POSITION;
	float2 UV : TEXCOORD0;
};

VS_Output main(const VS_Input Input)
{
	VS_Output Output;
	float4 pos4 = {Input.Pos.x, Input.Pos.y, 0.0, 1.0};
	Output.Position = pos4;
	Output.UV = Input.UV;

#ifdef __OPENGL__
	Output.UV.y = 1.0 - Output.UV.y;
#endif

	return Output;
}
