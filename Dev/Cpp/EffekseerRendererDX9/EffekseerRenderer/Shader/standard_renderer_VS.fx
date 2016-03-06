
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
	float4 Position		: POSITION0;
	float4 Color		: TEXCOORD0;
	float2 UV		: TEXCOORD1;

	float4 Pos		: TEXCOORD2;
	float4 PosU		: TEXCOORD3;
	float4 PosR		: TEXCOORD4;
};

VS_Output VS( const VS_Input Input )
{
	VS_Output Output = (VS_Output)0;
	float4 pos4 = { Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0 };

	float4 cameraPos = mul(mCamera, pos4);
	cameraPos = cameraPos / cameraPos.w;
	Output.Position = mul(mProj, cameraPos);

	Output.Pos = Output.Position;

	float4 cameraPosU = cameraPos + float4(0.0, 1.0, 0.0, 0.0);
	float4 cameraPosR = cameraPos + float4(1.0, 0.0, 0.0, 0.0);
	Output.PosU = mul(mProj, cameraPosU);
	Output.PosR = mul(mProj, cameraPosR);

	Output.PosU /= Output.PosU.w;
	Output.PosR /= Output.PosR.w;
	Output.Pos /= Output.Pos.w;

	Output.Color = Input.Color;
	Output.UV = Input.UV;
	return Output;
}
