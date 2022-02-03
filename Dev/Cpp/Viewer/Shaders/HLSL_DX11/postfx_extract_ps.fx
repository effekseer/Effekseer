
Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);

float4 g_filterParams : register(c0);
float4 g_intensity : register(c1);

struct PS_Input
{
	float4 Pos : SV_POSITION;
	float2 UV : TEXCOORD0;
};

bool isNan(float val)
{
	return (val < 0.0 || 0.0 < val || val == 0.0) ? false : true;
}

float4 main(const PS_Input Input)
	: SV_Target
{
	float3 color = g_texture.Sample(g_sampler, Input.UV).rgb;

	if (isNan(color.r))
	{
		color.r = 0.0f;
	}

	if (isNan(color.g))
	{
		color.g = 0.0f;
	}

	if (isNan(color.b))
	{
		color.b = 0.0f;
	}

	float brightness = dot(color, float3(0.299, 0.587, 0.114));
	float soft = brightness - g_filterParams.y;
	soft = clamp(soft, 0.0, g_filterParams.z);
	soft = soft * soft * g_filterParams.w;
	float contribution = max(soft, brightness - g_filterParams.x);
	contribution /= max(brightness, 0.00001);
	return float4(color.rgb * contribution * g_intensity.x, 1.0);
}
