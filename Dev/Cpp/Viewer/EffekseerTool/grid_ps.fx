
struct PS_Input
{
    float4 Color	: COLOR;
};

float4 PS( const PS_Input Input ) : COLOR
{
	float4 Output = Input.Color;
	return Output;
}
