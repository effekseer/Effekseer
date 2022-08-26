#pragma once

namespace Effekseer
{
namespace Shader
{

static const char* material_gradient = R"(

struct Gradient
{
	int colorCount;
	int alphaCount;
	int reserved1;
	int reserved2;
	float4 colors[8];
	float2 alphas[8];
};

float LinearStep(float s, float e, float v) {
	return clamp((v - s) / (e - s), 0.0, 1.0);
}

float4 SampleGradient(Gradient gradient, float t) {
	float3 color = gradient.colors[0].xyz;
	for(int i = 1; i < 8; i++) {
		float inRange = step(float(i), float(gradient.colorCount-1));
		float ls = LinearStep(gradient.colors[i-1].w, gradient.colors[i].w, t);
		color = LERP(color, gradient.colors[i].xyz, ls * inRange); 
	}

	float alpha = gradient.alphas[0].x;
	for(int i = 1; i < 8; i++) {
		float inRange = step(float(i), float(gradient.alphaCount-1));
		float ls = LinearStep(gradient.alphas[i-1].y, gradient.alphas[i].y, t);
		alpha = LERP(alpha, gradient.alphas[i].x, ls * inRange); 
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

)";

static const char* material_noise = R"(

float Rand2(float2 n) { 
	return FRAC(sin(dot(n, float2(12.9898, 78.233))) * 43758.5453123);
}

float SimpleNoise_Block(float2 uv) {
	int2 uvi = int2(floor(uv));
	float2 uvf = FRAC(uv);
	uvf = uvf * uvf * (3.0 - 2.0 * uvf);

	float x0 = LERP(Rand2(float2(uvi + int2(0,0))), Rand2(float2(uvi + int2(1,0))), uvf.x);
	float x1 = LERP(Rand2(float2(uvi + int2(0,1))), Rand2(float2(uvi + int2(1,1))), uvf.x);
	return LERP(x0, x1, uvf.y);
}

float SimpleNoise(float2 uv, float scale) {
	const int loop = 3;
	float ret = 0.0;
	for(int i = 0; i < loop; i++) {
		float freq = pow(2, float(i));
		float intensity = pow(0.5, float(loop - i));
		ret += SimpleNoise_Block(uv * scale / freq) * intensity;
	}

	return ret;
}

)";

} // namespace Shader
} // namespace Effekseer
