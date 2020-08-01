#ifdef ENABLE_LIGHTING
cbuffer PSConstantBuffer : register(b1)
{
float4	fLightDirection		: register( c0 );
float4	fLightColor		: register( c1 );
float4	fLightAmbient		: register( c2 );
};
#endif

Texture2D	g_colorTexture		: register( t0 );
SamplerState	g_colorSampler		: register( s0 );
Texture2D	g_normalTexture		: register( t1 );
SamplerState	g_normalSampler		: register( s1 );

struct PS_Input
{
	float4 Pos		: SV_POSITION;
	linear centroid float2 UV : TEXCOORD0;
	half3 Normal	: TEXCOORD1;
	half3 Binormal	: TEXCOORD2;
	half3 Tangent	: TEXCOORD3;
	linear centroid float4 Color : COLOR;
};

float4 PS( const PS_Input Input ) : SV_Target
{
	float4 Output = g_colorTexture.Sample(g_colorSampler, Input.UV) * Input.Color;

#if ENABLE_LIGHTING
	half3 texNormal = (g_normalTexture.Sample(g_normalSampler, Input.UV).xyz  - 0.5) * 2.0;
	half3 localNormal = (half3)normalize(
		mul(
		texNormal ,
		half3x3( (half3)Input.Tangent, (half3)Input.Binormal, (half3)Input.Normal ) ) );

	float diffuse = max(dot(fLightDirection.xyz, localNormal.xyz), 0.0);
	Output.xyz = Output.xyz * (fLightColor.xyz * diffuse + fLightAmbient.xyz);
#endif

	if(Output.a <= 0.0f) discard;

	return Output;
}
