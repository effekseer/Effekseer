
float4x4 mCamera		: register(c0);
float4x4 mProj			: register(c4);
float4 mUVInversed		: register(c8);

struct VS_Input
{
	float3 Pos		: POSITION0;
	float4 Color		: NORMAL0;
	float2 UV		: TEXCOORD0;

	float3 Binormal		: NORMAL1;
	float3 Tangent		: NORMAL2;
};

struct VS_Output
{
	float4 Position		: SV_POSITION;
	float4 Color		: COLOR;
	float2 UV		: TEXCOORD0;

	float4 Pos		: TEXCOORD1;
	float4 PosU		: TEXCOORD2;
	float4 PosR		: TEXCOORD3;
};

VS_Output VS( const VS_Input Input )
{
	VS_Output Output = (VS_Output)0;
	float4 pos4 = { Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0 };

	float4 localBinormal = { Input.Pos.x + Input.Binormal.x, Input.Pos.y + Input.Binormal.y, Input.Pos.z + Input.Binormal.z, 1.0 };
	float4 localTangent = { Input.Pos.x + Input.Tangent.x, Input.Pos.y + Input.Tangent.y, Input.Pos.z + Input.Tangent.z, 1.0 };
	localBinormal = mul(mCamera, localBinormal);
	localTangent = mul(mCamera, localTangent);

	float4 cameraPos = mul(mCamera, pos4);
		cameraPos = cameraPos / cameraPos.w;

	localBinormal = localBinormal / localBinormal.w;
	localTangent = localTangent / localTangent.w;

	localBinormal = cameraPos + normalize(localBinormal - cameraPos);
	localTangent = cameraPos + normalize(localTangent - cameraPos);


	Output.Position = mul(mProj, cameraPos);

	Output.Pos = Output.Position;

	Output.PosU = mul(mProj, localBinormal);
	Output.PosR = mul(mProj, localTangent);

	Output.PosU /= Output.PosU.w;
	Output.PosR /= Output.PosR.w;
	Output.Pos /= Output.Pos.w;

	Output.Color = Input.Color;
	Output.UV = Input.UV;

	Output.UV.y = mUVInversed.x + mUVInversed.y * Input.UV.y;

	return Output;
}
