
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
	float4 PosVS : SV_POSITION;
	linear centroid float4 Color : COLOR;
	linear centroid float2 UV : TEXCOORD0;
	float4 PosP : TEXCOORD1;
};

VS_Output main(const VS_Input Input)
{
	VS_Output Output = (VS_Output)0;
	float4 pos4 = {Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0};

	float4 cameraPos = mul(mCamera, pos4);
	Output.PosVS = mul(mProj, cameraPos);

	Output.Color = Input.Color;
	Output.UV = Input.UV;

	Output.UV.y = mUVInversed.x + mUVInversed.y * Input.UV.y;

	Output.PosP = Output.PosVS;

	return Output;
}
