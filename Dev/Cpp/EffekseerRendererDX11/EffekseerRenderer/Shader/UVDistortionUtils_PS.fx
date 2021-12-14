#include "Linear_sRGB.fx"

float2 UVDistortionOffset(const Texture2D t, const SamplerState s, float2 uv, float2 uvInversed, bool convertFromSRGB)
{
	float4 sampledColor = t.Sample(s, uv);

	if (convertFromSRGB)
	{
		sampledColor = ConvertFromSRGBTexture(sampledColor, convertFromSRGB);
	}

    float2 UVOffset = sampledColor.rg * 2.0 - 1.0;
    UVOffset.y *= -1.0;
    UVOffset.y = uvInversed.x + uvInversed.y * UVOffset.y;
    return UVOffset;
}