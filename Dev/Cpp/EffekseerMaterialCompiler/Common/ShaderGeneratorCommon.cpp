#include "ShaderGeneratorCommon.h"
#include <iostream>
#include <sstream>

namespace Effekseer
{
namespace Shader
{

namespace
{
const char* material_lineargamma_functions = R"(

#define FLT_EPSILON 1.192092896e-07

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
	return max(1.055 * PositivePow(c, float3(0.416666667,0.416666667,0.416666667)) - 0.055, 0.0);
}

half4 LinearToSRGB(half4 c)
{
	return half4(LinearToSRGB(c.rgb), c.a);
}

half4 ConvertFromSRGBTexture(half4 c)
{
	if (predefined_uniform.z == 0.0)
	{
		return c;
	}

	return LinearToSRGB(c);
}

half4 ConvertToScreen(half4 c)
{
	if (predefined_uniform.z == 0.0)
	{
		return c;
	}

	return SRGBToLinear(c);
}

)";

}

static const char* material_gradient_functions = R"(

float LinearStep(float s, float e, float v) {
	return clamp((v - s) / (e - s), 0.0, 1.0);
}

#ifdef EFK__OPENGL2__

struct Gradient
{
	int colorCount;
	int alphaCount;
	int reserved1;
	int reserved2;
	float4 c1;
	float4 c2;
	float4 c3;
	float4 c4;
	float4 c5;
	float4 c6;
	float4 c7;
	float4 c8;
	float2 a1;
	float2 a2;
	float2 a3;
	float2 a4;
	float2 a5;
	float2 a6;
	float2 a7;
	float2 a8;
};

float4 SampleGradient(Gradient gradient, float t) {
	float4 colors[8];
	float2 alphas[8];

	colors[0] = gradient.c1;
	colors[1] = gradient.c2;
	colors[2] = gradient.c3;
	colors[3] = gradient.c4;
	colors[4] = gradient.c5;
	colors[5] = gradient.c6;
	colors[6] = gradient.c7;
	colors[7] = gradient.c8;
	alphas[0].xy = gradient.a1;
	alphas[1].xy = gradient.a2;
	alphas[2].xy = gradient.a3;
	alphas[3].xy = gradient.a4;
	alphas[4].xy = gradient.a5;
	alphas[5].xy = gradient.a6;
	alphas[6].xy = gradient.a7;
	alphas[7].xy = gradient.a8;

	float t_clamp = clamp(t, 0.0, 1.0);
	float3 color = colors[0].xyz;
	for(int i = 1; i < 8; i++) {
		float ratio = LinearStep(colors[i-1].w, colors[i].w, t_clamp);
		color = LERP(color, colors[i].xyz, ratio);
	}

	float alpha = alphas[0].x;
	for(int i = 1; i < 8; i++) {
		float ratio = LinearStep(alphas[i-1].y, alphas[i].y, t_clamp);
		alpha = LERP(alpha, alphas[i].x, ratio);
	}
	return float4(color, alpha);
}

Gradient GradientParameter(float4 param_v, float4 param_c1, float4 param_c2, float4 param_c3, float4 param_c4, float4 param_c5, float4 param_c6, float4 param_c7, float4 param_c8, float4 param_a1, float4 param_a2, float4 param_a3, float4 param_a4)
{
	Gradient g;
	g.colorCount = int(param_v.x);
	g.alphaCount = int(param_v.y);
	g.reserved1 = int(param_v.z);
	g.reserved2 = int(param_v.w);

	g.c1 = param_c1;
	g.c2 = param_c2;
	g.c3 = param_c3;
	g.c4 = param_c4;
	g.c5 = param_c5;
	g.c6 = param_c6;
	g.c7 = param_c7;
	g.c8 = param_c8;

	g.a1 = param_a1.xy;
	g.a2 = param_a1.zw;
	g.a3 = param_a2.xy;
	g.a4 = param_a2.zw;
	g.a5 = param_a3.xy;
	g.a6 = param_a3.zw;
	g.a7 = param_a4.xy;
	g.a8 = param_a4.zw;

	return g;
}

#else

struct Gradient
{
	int colorCount;
	int alphaCount;
	int reserved1;
	int reserved2;
	float4 colors[8];
	float2 alphas[8];
};

float4 SampleGradient(Gradient gradient, float t) {
	float t_clamp = clamp(t, 0.0, 1.0);
	float3 color = gradient.colors[0].xyz;
	for(int i = 1; i < 8; i++) {
		float ratio = LinearStep(gradient.colors[i-1].w, gradient.colors[i].w, t_clamp);
		color = LERP(color, gradient.colors[i].xyz, ratio);
	}

	float alpha = gradient.alphas[0].x;
	for(int i = 1; i < 8; i++) {
		float ratio = LinearStep(gradient.alphas[i-1].y, gradient.alphas[i].y, t_clamp);
		alpha = LERP(alpha, gradient.alphas[i].x, ratio);
	}
	return float4(color, alpha);
}

Gradient GradientParameter(float4 param_v, float4 param_c1, float4 param_c2, float4 param_c3, float4 param_c4, float4 param_c5, float4 param_c6, float4 param_c7, float4 param_c8, float4 param_a1, float4 param_a2, float4 param_a3, float4 param_a4)
{
	Gradient g;
	g.colorCount = int(param_v.x);
	g.alphaCount = int(param_v.y);
	g.reserved1 = int(param_v.z);
	g.reserved2 = int(param_v.w);
	g.colors[0] = param_c1;
	g.colors[1] = param_c2;
	g.colors[2] = param_c3;
	g.colors[3] = param_c4;
	g.colors[4] = param_c5;
	g.colors[5] = param_c6;
	g.colors[6] = param_c7;
	g.colors[7] = param_c8;
	g.alphas[0].xy = param_a1.xy;
	g.alphas[1].xy = param_a1.zw;
	g.alphas[2].xy = param_a2.xy;
	g.alphas[3].xy = param_a2.zw;
	g.alphas[4].xy = param_a3.xy;
	g.alphas[5].xy = param_a3.zw;
	g.alphas[6].xy = param_a4.xy;
	g.alphas[7].xy = param_a4.zw;
	return g;
}

#endif

)";

static const char* material_noise_functions = R"(

float Rand2(float2 n) { 
	return FRAC(sin(dot(n, float2(12.9898, 78.233))) * 43758.5453123);
}

float SimpleNoise_Block(float2 uv) {
	int2 uvi = int2(floor(uv));
	float2 uvf = FRAC(uv);
	uvf = uvf * uvf * (3.0 - 2.0 * uvf);
	float x0 = LERP(Rand2(float2(uvi + int2(0, 0))), Rand2(float2(uvi + int2(1, 0))), uvf.x);
	float x1 = LERP(Rand2(float2(uvi + int2(0, 1))), Rand2(float2(uvi + int2(1, 1))), uvf.x);
	return LERP(x0, x1, uvf.y);
}

float SimpleNoise(float2 uv, float scale) {
	const int iter = 3;
	float ret = 0.0;
	for(int i = 0; i < iter; i++) {
		float duration = pow(2.0, float(i));
		float intensity = pow(0.5, float(iter-i));
		ret += SimpleNoise_Block(uv * scale / duration) * intensity;
	}

	return ret;
}

)";

std::string GetFixedGradient(const char* name, const Gradient& gradient)
{
	std::stringstream ss;

	ss << "Gradient " << name << "() {" << std::endl;
	ss << "Gradient g;" << std::endl;
	ss << "g.colorCount = " << gradient.ColorCount << ";" << std::endl;
	ss << "g.alphaCount = " << gradient.AlphaCount << ";" << std::endl;
	ss << "g.reserved1 = 0;" << std::endl;
	ss << "g.reserved2 = 0;" << std::endl;

	const auto getColorKey = [](const Effekseer::Gradient& gradient, size_t index)
	{
		if (gradient.ColorCount == 0)
		{
			Effekseer::Gradient::ColorKey key;
			key.Color = {1.0f, 1.0f, 1.0f};
			key.Intensity = 1.0f;
			key.Position = 0.0;
			return key;
		}
		else
		{
			if (gradient.ColorCount <= index)
			{
				auto key = gradient.Colors[gradient.ColorCount - 1];
				key.Position += index;
				return key;
			}

			return gradient.Colors[index];
		}
	};

	const auto getAlphaKey = [](const Effekseer::Gradient& gradient, size_t index)
	{
		if (gradient.AlphaCount == 0)
		{
			Effekseer::Gradient::AlphaKey key;
			key.Alpha = 1.0f;
			key.Position = 0.0;
			return key;
		}
		else
		{
			if (gradient.AlphaCount <= index)
			{
				auto key = gradient.Alphas[gradient.AlphaCount - 1];
				key.Position += index;
				return key;
			}

			return gradient.Alphas[index];
		}
	};

	ss << "#ifdef EFK__OPENGL2__" << std::endl;

	for (int32_t i = 0; i < gradient.Colors.size(); i++)
	{
		const auto key = getColorKey(gradient, i);
		ss << "g.c" << i + 1 << ".x = " << std::to_string(key.Color[0] * key.Intensity) << ";" << std::endl;
		ss << "g.c" << i + 1 << ".y = " << std::to_string(key.Color[1] * key.Intensity) << ";" << std::endl;
		ss << "g.c" << i + 1 << ".z = " << std::to_string(key.Color[2] * key.Intensity) << ";" << std::endl;
		ss << "g.c" << i + 1 << ".w = " << std::to_string(key.Position) << ";" << std::endl;
	}

	for (int32_t i = 0; i < gradient.Alphas.size(); i++)
	{
		const auto key = getAlphaKey(gradient, i);
		ss << "g.a" << i + 1 << ".x = " << std::to_string(key.Alpha) << ";" << std::endl;
		ss << "g.a" << i + 1 << ".y = " << std::to_string(key.Position) << ";" << std::endl;
	}

	ss << "#else" << std::endl;

	for (int32_t i = 0; i < gradient.Colors.size(); i++)
	{
		const auto key = getColorKey(gradient, i);
		ss << "g.colors[" << i << "].x = " << std::to_string(key.Color[0] * key.Intensity) << ";" << std::endl;
		ss << "g.colors[" << i << "].y = " << std::to_string(key.Color[1] * key.Intensity) << ";" << std::endl;
		ss << "g.colors[" << i << "].z = " << std::to_string(key.Color[2] * key.Intensity) << ";" << std::endl;
		ss << "g.colors[" << i << "].w = " << std::to_string(key.Position) << ";" << std::endl;
	}

	for (int32_t i = 0; i < gradient.Alphas.size(); i++)
	{
		const auto key = getAlphaKey(gradient, i);
		ss << "g.alphas[" << i << "].x = " << std::to_string(key.Alpha) << ";" << std::endl;
		ss << "g.alphas[" << i << "].y = " << std::to_string(key.Position) << ";" << std::endl;
	}

	ss << "#endif" << std::endl;

	ss << "return g; }" << std::endl;

	return ss.str();
}

const char* GetGradientFunctions()
{
	return material_gradient_functions;
}

const char* GetNoiseFunctions()
{
	return material_noise_functions;
}

const char* GetLinearGammaFunctions()
{
	return material_lineargamma_functions;
}

} // namespace Shader
} // namespace Effekseer