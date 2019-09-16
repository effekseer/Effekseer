
float4x4 mCameraProj		: register( c0 );
float4x4 mModel[40]		: register( c4 );
float4	fUV[40]			: register( c164 );
float4	fModelColor[40]		: register( c204 );
float4 mUVInversed		: register(c247);

struct VS_Input
{
	float3 Pos		: POSITION0;
	float3 Normal		: NORMAL0;
	float3 Binormal		: NORMAL1;
	float3 Tangent		: NORMAL2;
	float2 UV		: TEXCOORD0;
	uint4 Index		: BLENDINDICES0;

};

struct VS_Output
{
	float4 Position		: SV_POSITION;
	float2 UV		: TEXCOORD0;
	float4 Normal		: TEXCOORD1;
	float4 Binormal		: TEXCOORD2;
	float4 Tangent		: TEXCOORD3;
	float4 Pos		: TEXCOORD4;
	float4 Color		: COLOR0;
};

VS_Output VS( const VS_Input Input )
{
	float4x4 matModel = mModel[Input.Index.x];
		float4 uv = fUV[Input.Index.x];
		float4 modelColor = fModelColor[Input.Index.x];

		VS_Output Output = (VS_Output) 0;
	float4 localPosition = { Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0 };
		float4 localNormal = { Input.Pos.x + Input.Normal.x, Input.Pos.y + Input.Normal.y, Input.Pos.z + Input.Normal.z, 1.0 };
		float4 localBinormal = { Input.Pos.x + Input.Binormal.x, Input.Pos.y + Input.Binormal.y, Input.Pos.z + Input.Binormal.z, 1.0 };
		float4 localTangent = { Input.Pos.x + Input.Tangent.x, Input.Pos.y + Input.Tangent.y, Input.Pos.z + Input.Tangent.z, 1.0 };


	localPosition = mul(matModel, localPosition);
	localNormal = mul(matModel, localNormal);
	localBinormal = mul(matModel, localBinormal);
	localTangent = mul(matModel, localTangent);

	localNormal = localPosition + normalize(localNormal - localPosition);
	localBinormal = localPosition + normalize(localBinormal - localPosition);
	localTangent = localPosition + normalize(localTangent - localPosition);

	Output.Position = mul(mCameraProj, localPosition);

	Output.UV.x = Input.UV.x * uv.z + uv.x;
	Output.UV.y = Input.UV.y * uv.w + uv.y;

	Output.Normal = mul(mCameraProj, localNormal);
	Output.Binormal = mul(mCameraProj, localBinormal);
	Output.Tangent = mul(mCameraProj, localTangent);
	Output.Pos = Output.Position;

	Output.Color = modelColor;

	Output.UV.y = mUVInversed.x + mUVInversed.y * Output.UV.y;

	return Output;
}

