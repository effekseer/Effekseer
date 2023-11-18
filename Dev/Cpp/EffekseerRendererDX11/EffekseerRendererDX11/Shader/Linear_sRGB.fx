#ifndef LINEAR_SRGB_FX
#define LINEAR_SRGB_FX


#define FLT_EPSILON 1.192092896e-07f

float3 PositivePow(float3 base, float3 power)
{
	return pow(max(abs(base), float3(FLT_EPSILON, FLT_EPSILON, FLT_EPSILON)), power);
}

// based on http://chilliant.blogspot.com/2012/08/srgb-approximations-for-hlsl.html
half3 SRGBToLinear(half3 c)
{
	return min(c, c * (c * (c * 0.305306011 + 0.682171111) + 0.012522878));
}

half4 SRGBToLinear(half4 c)
{
	return half4(SRGBToLinear(c.rgb), c.a);
}

half3 LinearToSRGB(half3 c)
{
	return max(1.055 * PositivePow(c, 0.416666667) - 0.055, 0.0);
}

half4 LinearToSRGB(half4 c)
{
	return half4(LinearToSRGB(c.rgb), c.a);
}

half4 ConvertFromSRGBTexture(half4 c, bool isValid)
{
	if (!isValid)
	{
		return c;
	}

	return LinearToSRGB(c);
}

half4 ConvertToScreen(half4 c, bool isValid)
{
	if (!isValid)
	{
		return c;
	}

	return SRGBToLinear(c);
}

#endif
