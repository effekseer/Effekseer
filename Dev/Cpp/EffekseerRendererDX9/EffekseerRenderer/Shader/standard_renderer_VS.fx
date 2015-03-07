
float4x4 mCameraProj		: register( c0 );

struct VS_Input
{
	float3 Pos		: POSITION0;
	float4 Color		: NORMAL0;
	float2 UV		: TEXCOORD0;
};

struct VS_Output
{
	float4 Position		: POSITION0;
	float4 Color		: COLOR;
	float2 UV		: TEXCOORD0;

	float4 Pos		: TEXCOORD1;
};

VS_Output VS( const VS_Input Input )
{
	VS_Output Output = (VS_Output)0;
	float4 pos4 = { Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0 };
	
	Output.Position = mul( mCameraProj, pos4 );
	Output.Pos = Output.Position;

	Output.Color = Input.Color;
	Output.UV = Input.UV;
	return Output;
}
