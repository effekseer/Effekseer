
struct PS_Input
{
	float4 Color		: TEXCOORD0;
	float2 UV		: TEXCOORD1;
};


float4 PS( const PS_Input Input ) : COLOR
{
	float4 Output = Input.Color;
	return Output;
}
