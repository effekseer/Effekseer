#include "Linear_sRGB.fx"

void ApplyFlipbook(inout float4 dst,
				   const Texture2D t,
				   const SamplerState s,
				   const float4 flipbookParameter,
				   const float4 vcolor,
				   const float2 nextUV,
				   float flipbookRate,
				   bool convertFromSRGB)
{
	if (flipbookParameter.x > 0)
	{
		float4 sampledColor = t.Sample(s, nextUV);

		if (convertFromSRGB) {
			sampledColor = ConvertFromSRGBTexture(sampledColor, convertFromSRGB);
		}
		
		float4 NextPixelColor = sampledColor * vcolor;

		// lerp
		if (flipbookParameter.y == 1)
		{
			dst = lerp(dst, NextPixelColor, flipbookRate);
		}
	}
}