
texture Tex0;
sampler Smp0 = sampler_state
{
	Texture   = <Tex0>;
};

struct VS_Input
{
    float3 Pos		: POSITION0;
    float2 UV		: TEXCOORD0;
};

struct VS_Output
{
    float4 Pos		: POSITION0;
    float2 UV		: TEXCOORD0;
};

struct PS_Input
{
    float2 UV		: TEXCOORD0;
};



VS_Output VS( const VS_Input Input )
{
	VS_Output Output = (VS_Output)0;
	float4 pos4 = { Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0 };
	Output.Pos = pos4;
	Output.UV = Input.UV;
    return Output;
}

float4 PS( const PS_Input Input ) : COLOR
{
	float4 Output = tex2D(Smp0, Input.UV);
	Output.w = 1.0;

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
