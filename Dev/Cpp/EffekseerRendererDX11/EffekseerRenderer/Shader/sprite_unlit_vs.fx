
cbuffer VS_ConstantBuffer : register(b0)
{
	float4x4 mCamera;
	float4x4 mProj;
	float4 mUVInversed;

	// Unused
    float4 mflipbookParameter; // x:enable, y:loopType, z:divideX, w:divideY
}

struct VS_Input
{
	float3 Pos : POSITION0;
	float4 Color : NORMAL0;
	float2 UV : TEXCOORD0;
};

struct VS_Output
{
	float4 Position : SV_POSITION;
	float4 Color : COLOR;
	float2 UV : TEXCOORD0;

	//float4 Pos : TEXCOORD1;
	//float4 PosU : TEXCOORD2;
	//float4 PosR : TEXCOORD3;
};

VS_Output main(const VS_Input Input)
{
	VS_Output Output = (VS_Output)0;
	float4 pos4 = {Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0};

	float4 cameraPos = mul(mCamera, pos4);
	cameraPos = cameraPos / cameraPos.w;
	Output.Position = mul(mProj, cameraPos);

	//Output.Pos = Output.Position;

	float4 cameraPosU = cameraPos + float4(0.0, 1.0, 0.0, 0.0);
	float4 cameraPosR = cameraPos + float4(1.0, 0.0, 0.0, 0.0);
	//Output.PosU = mul(mProj, cameraPosU);
	//Output.PosR = mul(mProj, cameraPosR);
	//
	//Output.PosU /= Output.PosU.w;
	//Output.PosR /= Output.PosR.w;
	//Output.Pos /= Output.Pos.w;

	Output.Color = Input.Color;
	Output.UV = Input.UV;

	Output.UV.y = mUVInversed.x + mUVInversed.y * Input.UV.y;

	return Output;
}
