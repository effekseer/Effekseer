
cbuffer VS_ConstantBuffer : register(b0)
{
	float4x4 projectionMatrix;
	float4x4 cameraMatrix;
	float4x4 worldMatrix;
}

struct VS_Input
{
	float3 Pos : POSITION0;
	float2 UV : TEXCOORD0;
	float4 Color : NORMAL0;
	float3 Normal : NORMAL1;
};

struct VS_Output
{
	float4 Pos : SV_POSITION;
	float2 UV : TEXCOORD0;
	float4 Color : NORMAL0;
	float3 Normal : NORMAL1;
	float4 Position : NORMAL2;
};

VS_Output main(const VS_Input input)
{
	VS_Output output;
	float4 localPos = float4(input.Pos, 1.0f);
	float4 worldPos = mul(worldMatrix, localPos);
	float4 cameraPos = mul(cameraMatrix, worldPos);
	output.Pos = mul(projectionMatrix, cameraPos);
	output.UV = input.UV;
	output.Color = input.Color;
	output.Normal = input.Normal;
	output.Position = output.Pos;
	return output;
}
