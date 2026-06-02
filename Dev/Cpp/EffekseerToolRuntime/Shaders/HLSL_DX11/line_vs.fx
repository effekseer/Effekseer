
float4x4 mCamera		: register(c0);
float4x4 mProj			: register(c4);

struct VS_Input
{
	float3 Pos		: POSITION0;
	float4 Color		: NORMAL0;
	float2 UV		: TEXCOORD0;
};

struct VS_Output
{
	float4 Pos		: SV_POSITION;
	float4 Color		: COLOR;
	float2 UV		: TEXCOORD0;
	float4 Position		: TEXCOORD1;
};

VS_Output main( const VS_Input Input )
{
	VS_Output Output = (VS_Output)0;
	float4 pos4 = { Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0 };

	float4 cameraPos = mul(mCamera, pos4);
	cameraPos = cameraPos / cameraPos.w;
	Output.Position = mul(mProj, cameraPos);
	Output.Pos = Output.Position;
	Output.UV = Input.UV;
	Output.Color = Input.Color;

	return Output;
}
