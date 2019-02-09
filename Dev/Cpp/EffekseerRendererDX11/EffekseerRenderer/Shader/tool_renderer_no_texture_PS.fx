
struct PS_Input
{
	float4 Pos		: SV_POSITION;
	float4 Color		: COLOR;
	float2 UV		: TEXCOORD0;
};


float4 PS( const PS_Input Input ) : SV_Target
{
	float4 Output = Input.Color;
	return Output;
}
