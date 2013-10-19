
float2 Size : register( c0 );


struct VS_Input
{
    float2 Pos		: POSITION0;
};

struct VS_Output
{
    float4 Pos		: POSITION0;
};


VS_Output VS( const VS_Input Input )
{
	VS_Output Output = (VS_Output)0;
	Output.Pos.x = (Input.Pos.x - 0.5) / Size.x * 2.0 - 1.0;
	Output.Pos.y = 1.0 - (Input.Pos.y - 0.5) / Size.y * 2.0;
	Output.Pos.z =  0.0;
	Output.Pos.w =  1.0;
	return Output;
}

