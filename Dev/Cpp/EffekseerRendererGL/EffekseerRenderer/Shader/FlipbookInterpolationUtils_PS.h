
static const char g_flipbook_interpolation_ps_src[] = R"(

void ApplyFlipbook(inout float4 dst,
				   const sampler2D s,
				   const float4 flipbookParameter,
				   const float4 vcolor,
				   const float2 nextUV,
				   float flipbookRate)
{
	if (flipbookParameter.x > 0)
	{
		float4 NextPixelColor = TEX2D(s, nextUV) * vcolor;

		// lerp
		if (flipbookParameter.y == 1)
		{
			dst = mix(dst, NextPixelColor, flipbookRate);
		}
	}
}

)";