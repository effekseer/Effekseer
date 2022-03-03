void ApplyFlipbook(inout float4 dst,
				   const Texture2D t,
				   const SamplerState s,
				   const float4 flipbookParameter,
				   const float4 vcolor,
				   const float2 nextUV,
				   float flipbookRate)
{
	if (flipbookParameter.x > 0)
	{
		float4 NextPixelColor = t.Sample(s, nextUV) * vcolor;

		// lerp
		if (flipbookParameter.y == 1)
		{
			dst = lerp(dst, NextPixelColor, flipbookRate);
		}
	}
}