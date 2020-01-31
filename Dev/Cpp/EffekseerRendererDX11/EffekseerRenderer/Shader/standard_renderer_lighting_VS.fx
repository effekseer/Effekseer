
struct VS_Input
{
	float3 Pos : POSITION0;
	float4 Color : NORMAL0;
	float4 Normal : NORMAL1;
	float4 Tangent : NORMAL2;
	float2 UV1 : TEXCOORD0;
	float2 UV2 : TEXCOORD1;
#ifdef __EFFEKSEER_BUILD_VERSION16__
    float2 AlphaUV : TEXCOORD2;
#endif
};

struct VS_Output
{
	float4 Position : SV_POSITION;
	float4 VColor : COLOR;
	float2 UV1 : TEXCOORD0;
	float2 UV2 : TEXCOORD1;
	float3 WorldP : TEXCOORD2;
	float3 WorldN : TEXCOORD3;
	float3 WorldT : TEXCOORD4;
	float3 WorldB : TEXCOORD5;
	float2 ScreenUV : TEXCOORD6;
#ifdef __EFFEKSEER_BUILD_VERSION16__
    float2 AlphaUV : TEXCOORD7;
#endif
};

float4x4 mCamera : register(c0);
float4x4 mProj : register(c4);
float4 mUVInversed : register(c8);

VS_Output VS( const VS_Input Input )
{
	VS_Output Output = (VS_Output)0;
	float3 worldPos = Input.Pos;
	float3 worldNormal = (Input.Normal - float3(0.5, 0.5, 0.5)) * 2.0;
	float3 worldTangent = (Input.Tangent - float3(0.5, 0.5, 0.5)) * 2.0;
	float3 worldBinormal = cross(worldNormal, worldTangent);

	// UV
	float2 uv1 = Input.UV1;
	float2 uv2 = Input.UV1;
	uv1.y = mUVInversed.x + mUVInversed.y * uv1.y;
	uv2.y = mUVInversed.x + mUVInversed.y * uv2.y;
#ifdef __EFFEKSEER_BUILD_VERSION16__
    float2 alphaUV = Input.AlphaUV;
    alphaUV.y = mUVInversed.x + mUVInversed.y * alphaUV.y;
#endif

	// NBT
	Output.WorldN = worldNormal;
	Output.WorldB = worldBinormal;
	Output.WorldT = worldTangent;

	float3 pixelNormalDir = float3(0.5, 0.5, 1.0);

	float4 cameraPos = mul(mCamera, float4(worldPos, 1.0));
	cameraPos = cameraPos / cameraPos.w;
	Output.Position = mul(mProj, cameraPos);

	Output.WorldP = worldPos;
	Output.VColor = Input.Color;
	Output.UV1 = uv1;
	Output.UV2 = uv2;
#ifdef __EFFEKSEER_BUILD_VERSION16__
    Output.AlphaUV = alphaUV;
#endif
	Output.ScreenUV = Output.Position.xy / Output.Position.w;
	Output.ScreenUV.xy = float2(Output.ScreenUV.x + 1.0, 1.0 - Output.ScreenUV.y) * 0.5;

	return Output;
}

