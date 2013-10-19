
struct PS_Input
{
	float4 Color		: COLOR;
	float2 UV		: TEXCOORD0;
};


float4 PS( const PS_Input Input ) : COLOR
{
	float4 Output = Input.Color;
	return Output;
}
