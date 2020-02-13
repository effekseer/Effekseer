
float4x4 mCameraProj		: register( c0 );
float4x4 mModel[40]		: register( c4 );
float4	fUV[40]			: register( c164 );
float4	fModelColor[40]		: register( c204 );

#ifdef ENABLE_LIGHTING
float4	fLightDirection		: register( c244 );
float4	fLightColor		: register( c245 );
float4	fLightAmbient		: register( c246 );
#endif
float4 mUVInversed		: register(c247);


struct VS_Input
{
	float3 Pos		: POSITION0;
	float3 Normal		: NORMAL0;
	float3 Binormal		: NORMAL1;
	float3 Tangent		: NORMAL2;
	float2 UV		: TEXCOORD0;
	float4 Color		: NORMAL3;
	uint4 Index		: BLENDINDICES0;

};

struct VS_Output
{
	float4 Pos		: SV_POSITION;
	float2 UV		: TEXCOORD0;
#if ENABLE_NORMAL_TEXTURE
	 half3 Normal		: TEXCOORD1;
	half3 Binormal		: TEXCOORD2;
	half3 Tangent		: TEXCOORD3;
#endif
	float4 Color		: COLOR;
};

VS_Output VS( const VS_Input Input )
{
	float4x4 matModel = mModel[Input.Index.x];
	float4 uv = fUV[Input.Index.x];
	float4 modelColor = fModelColor[Input.Index.x] * Input.Color;

	VS_Output Output = (VS_Output)0;
	float4 localPosition = { Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0 }; 
	localPosition = mul( matModel, localPosition );
	Output.Pos = mul( mCameraProj, localPosition );
	Output.Color = modelColor;

	Output.UV.x = Input.UV.x * uv.z + uv.x;
	Output.UV.y = Input.UV.y * uv.w + uv.y;

#if ENABLE_LIGHTING
	float3x3 lightMat = (float3x3)matModel;
	//lightMat = transpose( lightMat );

	float4 localNormal = { 0.0, 0.0, 0.0, 1.0 };
	localNormal.xyz = normalize( mul( lightMat, Input.Normal ) );

	float4 localBinormal = { 0.0, 0.0, 0.0, 1.0 };
	localBinormal.xyz = normalize( mul( lightMat, Input.Binormal ) );

	float4 localTangent = { 0.0, 0.0, 0.0, 1.0 };
	localTangent.xyz = normalize( mul( lightMat, Input.Tangent ) );
	Output.Normal = localNormal.xyz;
	Output.Binormal = localBinormal.xyz;
	Output.Tangent = localTangent.xyz;
#endif

	Output.UV.y = mUVInversed.x + mUVInversed.y * Output.UV.y;

	return Output;
}

