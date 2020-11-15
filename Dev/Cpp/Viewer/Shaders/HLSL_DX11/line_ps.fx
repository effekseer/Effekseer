
struct PS_Input
{
	float4 Pos		: SV_POSITION;
	float4 Color		: COLOR;
	float2 UV		: TEXCOORD0;
	float4 Position		: TEXCOORD1;
};

struct PS_Output
{
      float4 o0 : SV_TARGET0;
      float4 o1 : SV_TARGET1;
};

PS_Output main(const PS_Input Input)
{
	PS_Output output;
	output.o0 = Input.Color;

	output.o1.xyzw = 1.0f;
	output.o1.x = Input.Position.z / Input.Position.w;

	if (output.o0.a == 0.0)
		discard;

	return output;
}