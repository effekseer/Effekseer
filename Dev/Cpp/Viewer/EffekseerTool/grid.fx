
float4x4 mCameraProj;

struct VS_Input
{
    float3 Pos		: POSITION0;
    float4 Color	: NORMAL0;
};

struct VS_Output
{
    float4 Pos		: POSITION0;
    float4 Color	: COLOR;
};

struct PS_Input
{
    float4 Color	: COLOR;
};

VS_Output VS( const VS_Input Input )
{
    VS_Output Output = (VS_Output)0;
	float4 pos4 = { Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0 };
	Output.Pos = mul( pos4, mCameraProj );
	Output.Color = Input.Color;
    return Output;
}

float4 PS( const PS_Input Input ) : COLOR
{
	float4 Output = Input.Color;
	return Output;
}

technique Texhnique
{
	pass P0
{
		VertexShader = compile vs_2_0 VS();
		PixelShader  = compile ps_2_0 PS();
	}
}
