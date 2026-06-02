#include "ShaderGenerator.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <regex>
#include <sstream>
#include <unordered_map>

namespace Effekseer
{
namespace WGSL
{
namespace
{

constexpr int32_t GradientUniformCount = 13;

std::string Trim(const std::string& value)
{
	size_t first = 0;
	while (first < value.size() && std::isspace(static_cast<unsigned char>(value[first])) != 0)
	{
		first++;
	}

	size_t last = value.size();
	while (last > first && std::isspace(static_cast<unsigned char>(value[last - 1])) != 0)
	{
		last--;
	}

	return value.substr(first, last - first);
}

bool StartsWith(const std::string& value, const std::string& prefix)
{
	return value.size() >= prefix.size() && std::equal(prefix.begin(), prefix.end(), value.begin());
}

bool IsVectorType(const std::string& type)
{
	return StartsWith(type, "vec2") || StartsWith(type, "vec3") || StartsWith(type, "vec4");
}

std::string ModFunctionForVectorType(const std::string& type)
{
	if (StartsWith(type, "vec2"))
	{
		return "mod_vec2";
	}
	if (StartsWith(type, "vec3"))
	{
		return "mod_vec3";
	}
	if (StartsWith(type, "vec4"))
	{
		return "mod_vec4";
	}
	return "mod_f32";
}

std::string VectorConstructorForType(const std::string& type)
{
	if (StartsWith(type, "vec2"))
	{
		return "vec2<f32>";
	}
	if (StartsWith(type, "vec3"))
	{
		return "vec3<f32>";
	}
	if (StartsWith(type, "vec4"))
	{
		return "vec4<f32>";
	}
	return "f32";
}

std::string ReplaceAll(std::string target, const std::string& from, const std::string& to)
{
	if (from.empty())
	{
		return target;
	}

	auto pos = target.find(from);
	while (pos != std::string::npos)
	{
		target.replace(pos, from.size(), to);
		pos = target.find(from, pos + to.size());
	}

	return target;
}

std::string ReplaceRegex(std::string target, const std::regex& pattern, const std::string& replacement)
{
	return std::regex_replace(target, pattern, replacement);
}

size_t FindTopLevelToken(const std::string& expression, char token, size_t begin = 0)
{
	int32_t parenDepth = 0;
	int32_t bracketDepth = 0;
	for (size_t i = begin; i < expression.size(); i++)
	{
		const auto c = expression[i];
		if (c == '(')
		{
			parenDepth++;
		}
		else if (c == ')')
		{
			parenDepth--;
		}
		else if (c == '[')
		{
			bracketDepth++;
		}
		else if (c == ']')
		{
			bracketDepth--;
		}
		else if (c == token && parenDepth == 0 && bracketDepth == 0)
		{
			return i;
		}
	}

	return std::string::npos;
}

std::string ConvertTernaryExpression(const std::string& expression)
{
	const auto question = FindTopLevelToken(expression, '?');
	if (question == std::string::npos)
	{
		return expression;
	}

	const auto colon = FindTopLevelToken(expression, ':', question + 1);
	if (colon == std::string::npos)
	{
		return expression;
	}

	const auto condition = Trim(expression.substr(0, question));
	const auto trueValue = Trim(expression.substr(question + 1, colon - question - 1));
	const auto falseValue = Trim(expression.substr(colon + 1));
	return "select(" + falseValue + ", " + trueValue + ", " + condition + ")";
}

std::string AdaptBoolExpression(std::string expression, const std::unordered_map<std::string, std::string>& variableTypes)
{
	for (const auto& variableType : variableTypes)
	{
		if (!IsVectorType(variableType.second))
		{
			continue;
		}

		const auto& name = variableType.first;
		expression = ReplaceRegex(
			expression,
			std::regex("\\b" + name + R"(\s*<\s*f32\(([^,\(\)]*)\))"),
			name + ".x < f32($1)");
		expression = ReplaceRegex(
			expression,
			std::regex("\\b" + name + R"(\s*>\s*f32\(([^,\(\)]*)\))"),
			name + ".x > f32($1)");
		expression = ReplaceRegex(
			expression,
			std::regex("\\b" + name + R"(\s*<=\s*f32\(([^,\(\)]*)\))"),
			name + ".x <= f32($1)");
		expression = ReplaceRegex(
			expression,
			std::regex("\\b" + name + R"(\s*>=\s*f32\(([^,\(\)]*)\))"),
			name + ".x >= f32($1)");
	}

	return expression;
}

std::string AdaptExpressionForDeclaredType(std::string expression,
										   const std::string& type,
										   const std::unordered_map<std::string, std::string>& variableTypes)
{
	expression = ConvertTernaryExpression(Trim(expression));
	expression = ReplaceAll(expression, "lerp(", "mix(");
	expression = ReplaceAll(expression, "LERP(", "mix(");
	expression = ReplaceAll(expression, "frac(", "fract(");
	expression = ReplaceAll(expression, "FRAC(", "fract(");

	if (type == "bool")
	{
		expression = AdaptBoolExpression(expression, variableTypes);
	}

	if (type == "f32")
	{
		expression = ReplaceAll(expression, "MOD(", "mod_f32(");
	}

	if (IsVectorType(type))
	{
		const auto vectorType = VectorConstructorForType(type);
		const auto modFunction = ModFunctionForVectorType(type);
		expression = ReplaceRegex(
			expression,
			std::regex(R"(MOD\(([^,]+),\s*f32\(([^,\(\)]*)\)\))"),
			modFunction + "($1, " + vectorType + "($2))");
		expression = ReplaceAll(expression, "MOD(", modFunction + "(");
		expression = ReplaceRegex(
			expression,
			std::regex(R"(clamp\(([^,]+),\s*f32\(([^,\(\)]*)\),\s*f32\(([^,\(\)]*)\)\))"),
			"clamp($1, " + vectorType + "($2), " + vectorType + "($3))");
		expression = ReplaceRegex(
			expression,
			std::regex(R"(min\(([^,]+),\s*f32\(([^,\(\)]*)\)\))"),
			"min($1, " + vectorType + "($2))");
		expression = ReplaceRegex(
			expression,
			std::regex(R"(max\(([^,]+),\s*f32\(([^,\(\)]*)\)\))"),
			"max($1, " + vectorType + "($2))");
		expression = ReplaceRegex(
			expression,
			std::regex(R"(pow\(([^,]+),\s*f32\(([^,\(\)]*)\)\))"),
			"pow($1, " + vectorType + "($2))");
	}

	return expression;
}

std::string ConvertMaterialCodeLines(const std::string& code)
{
	std::ostringstream dst;
	std::istringstream src(code);
	std::string line;
	std::unordered_map<std::string, std::string> variableTypes;

	const std::regex declarationWithInitializer(
		R"(^\s*(bool|f32|vec2<f32>|vec3<f32>|vec4<f32>|mat4x4<f32>)\s+([A-Za-z_][A-Za-z0-9_]*)\s*=\s*(.*);\s*$)");
	const std::regex declarationOnly(
		R"(^\s*(bool|f32|vec2<f32>|vec3<f32>|vec4<f32>|mat4x4<f32>)\s+([A-Za-z_][A-Za-z0-9_]*)\s*;\s*$)");

	while (std::getline(src, line))
	{
		if (!line.empty() && line.back() == '\r')
		{
			line.pop_back();
		}

		const auto trimmed = Trim(line);
		if (trimmed.empty())
		{
			dst << "\n";
			continue;
		}

		if (StartsWith(trimmed, "//"))
		{
			dst << line << "\n";
			continue;
		}

		std::smatch match;
		if (std::regex_match(line, match, declarationWithInitializer))
		{
			const auto type = match[1].str();
			const auto name = match[2].str();
			auto expression = AdaptExpressionForDeclaredType(match[3].str(), type, variableTypes);
			dst << "var " << name << " : " << type << " = " << expression << ";\n";
			variableTypes[name] = type;
			continue;
		}

		if (std::regex_match(line, match, declarationOnly))
		{
			const auto type = match[1].str();
			const auto name = match[2].str();
			dst << "var " << name << " : " << type << ";\n";
			variableTypes[name] = type;
			continue;
		}

		dst << line << "\n";
	}

	return dst.str();
}

bool UsesDepthFade(MaterialFile* materialFile)
{
	const auto code = std::string(materialFile->GetGenericCode());
	return code.find("CalcDepthFade") != std::string::npos;
}

bool HasRequiredMethod(MaterialFile* materialFile, MaterialFile::RequiredPredefinedMethodType method)
{
	return std::find(materialFile->RequiredMethods.begin(), materialFile->RequiredMethods.end(), method) != materialFile->RequiredMethods.end();
}

std::string ToWGSLFloat(float value)
{
	std::ostringstream ss;
	ss << value;
	const auto text = ss.str();
	if (text.find('.') == std::string::npos && text.find('e') == std::string::npos && text.find('E') == std::string::npos)
	{
		return text + ".0";
	}
	return text;
}

std::string ExportFixedGradient(const MaterialFile::GradientParameter& gradientParameter)
{
	std::ostringstream ss;
	const auto& gradient = gradientParameter.Data;

	const auto getColorKey = [](const Effekseer::Gradient& gradient, size_t index)
	{
		if (gradient.ColorCount == 0)
		{
			Effekseer::Gradient::ColorKey key;
			key.Color = {1.0f, 1.0f, 1.0f};
			key.Intensity = 1.0f;
			key.Position = 0.0f;
			return key;
		}

		if (gradient.ColorCount <= index)
		{
			auto key = gradient.Colors[gradient.ColorCount - 1];
			key.Position += static_cast<float>(index);
			return key;
		}

		return gradient.Colors[index];
	};

	const auto getAlphaKey = [](const Effekseer::Gradient& gradient, size_t index)
	{
		if (gradient.AlphaCount == 0)
		{
			Effekseer::Gradient::AlphaKey key;
			key.Alpha = 1.0f;
			key.Position = 0.0f;
			return key;
		}

		if (gradient.AlphaCount <= index)
		{
			auto key = gradient.Alphas[gradient.AlphaCount - 1];
			key.Position += static_cast<float>(index);
			return key;
		}

		return gradient.Alphas[index];
	};

	ss << "fn " << gradientParameter.Name << "() -> Gradient {\n";
	ss << "\tvar g : Gradient;\n";
	ss << "\tg.colorCount = " << gradient.ColorCount << ";\n";
	ss << "\tg.alphaCount = " << gradient.AlphaCount << ";\n";
	ss << "\tg.reserved1 = 0;\n";
	ss << "\tg.reserved2 = 0;\n";

	for (size_t i = 0; i < gradient.Colors.size(); i++)
	{
		const auto key = getColorKey(gradient, i);
		ss << "\tg.colors[" << i << "] = vec4<f32>(" << ToWGSLFloat(key.Color[0] * key.Intensity) << ", "
		   << ToWGSLFloat(key.Color[1] * key.Intensity) << ", " << ToWGSLFloat(key.Color[2] * key.Intensity) << ", "
		   << ToWGSLFloat(key.Position) << ");\n";
	}

	for (size_t i = 0; i < gradient.Alphas.size(); i++)
	{
		const auto key = getAlphaKey(gradient, i);
		ss << "\tg.alphas[" << i << "] = vec2<f32>(" << ToWGSLFloat(key.Alpha) << ", " << ToWGSLFloat(key.Position) << ");\n";
	}

	ss << "\treturn g;\n";
	ss << "}\n\n";
	return ss.str();
}

std::string GetCommonFunctions(bool isPixelShader, bool includeDepthFade)
{
	std::ostringstream ss;
	ss << R"(
fn PositivePow3(base : vec3<f32>, power : vec3<f32>) -> vec3<f32> {
	return pow(max(abs(base), vec3<f32>(0.0000001192092896)), power);
}

fn SRGBToLinear3(c : vec3<f32>) -> vec3<f32> {
	return min(c, c * (c * (c * 0.305306011 + vec3<f32>(0.682171111)) + vec3<f32>(0.012522878)));
}

fn SRGBToLinear4(c : vec4<f32>) -> vec4<f32> {
	return vec4<f32>(SRGBToLinear3(c.xyz), c.w);
}

fn LinearToSRGB3(c : vec3<f32>) -> vec3<f32> {
	return max(1.055 * PositivePow3(c, vec3<f32>(0.416666667)) - vec3<f32>(0.055), vec3<f32>(0.0));
}

fn LinearToSRGB4(c : vec4<f32>) -> vec4<f32> {
	return vec4<f32>(LinearToSRGB3(c.xyz), c.w);
}

fn ConvertFromSRGBTexture(c : vec4<f32>) -> vec4<f32> {
	if (v.predefined_uniform.z == 0.0) {
		return c;
	}
	return LinearToSRGB4(c);
}

fn ConvertToScreen(c : vec4<f32>) -> vec4<f32> {
	if (v.predefined_uniform.z == 0.0) {
		return c;
	}
	return SRGBToLinear4(c);
}

fn LinearStep(s : f32, e : f32, value : f32) -> f32 {
	return clamp((value - s) / (e - s), 0.0, 1.0);
}

fn mod_f32(x : f32, y : f32) -> f32 {
	return x - y * floor(x / y);
}

fn mod_vec2(x : vec2<f32>, y : vec2<f32>) -> vec2<f32> {
	return x - y * floor(x / y);
}

fn mod_vec3(x : vec3<f32>, y : vec3<f32>) -> vec3<f32> {
	return x - y * floor(x / y);
}

fn mod_vec4(x : vec4<f32>, y : vec4<f32>) -> vec4<f32> {
	return x - y * floor(x / y);
}

struct Gradient {
	colorCount : i32,
	alphaCount : i32,
	reserved1 : i32,
	reserved2 : i32,
	colors : array<vec4<f32>, 8>,
	alphas : array<vec2<f32>, 8>,
}

fn SampleGradient(gradient : Gradient, t : f32) -> vec4<f32> {
	let t_clamp = clamp(t, 0.0, 1.0);
	var color = gradient.colors[0].xyz;
	for (var i = 1; i < 8; i = i + 1) {
		let ratio = LinearStep(gradient.colors[i - 1].w, gradient.colors[i].w, t_clamp);
		color = mix(color, gradient.colors[i].xyz, ratio);
	}

	var alpha = gradient.alphas[0].x;
	for (var i = 1; i < 8; i = i + 1) {
		let ratio = LinearStep(gradient.alphas[i - 1].y, gradient.alphas[i].y, t_clamp);
		alpha = mix(alpha, gradient.alphas[i].x, ratio);
	}

	return vec4<f32>(color, alpha);
}

fn GradientParameter(
	param_v : vec4<f32>,
	param_c1 : vec4<f32>,
	param_c2 : vec4<f32>,
	param_c3 : vec4<f32>,
	param_c4 : vec4<f32>,
	param_c5 : vec4<f32>,
	param_c6 : vec4<f32>,
	param_c7 : vec4<f32>,
	param_c8 : vec4<f32>,
	param_a1 : vec4<f32>,
	param_a2 : vec4<f32>,
	param_a3 : vec4<f32>,
	param_a4 : vec4<f32>) -> Gradient {
	var g : Gradient;
	g.colorCount = i32(param_v.x);
	g.alphaCount = i32(param_v.y);
	g.reserved1 = i32(param_v.z);
	g.reserved2 = i32(param_v.w);
	g.colors[0] = param_c1;
	g.colors[1] = param_c2;
	g.colors[2] = param_c3;
	g.colors[3] = param_c4;
	g.colors[4] = param_c5;
	g.colors[5] = param_c6;
	g.colors[6] = param_c7;
	g.colors[7] = param_c8;
	g.alphas[0] = param_a1.xy;
	g.alphas[1] = param_a1.zw;
	g.alphas[2] = param_a2.xy;
	g.alphas[3] = param_a2.zw;
	g.alphas[4] = param_a3.xy;
	g.alphas[5] = param_a3.zw;
	g.alphas[6] = param_a4.xy;
	g.alphas[7] = param_a4.zw;
	return g;
}

fn Rand2(n : vec2<f32>) -> f32 {
	return fract(sin(dot(n, vec2<f32>(12.9898, 78.233))) * 43758.5453123);
}

fn SimpleNoise_Block(uv : vec2<f32>) -> f32 {
	let uvi = vec2<i32>(floor(uv));
	var uvf = fract(uv);
	uvf = uvf * uvf * (vec2<f32>(3.0) - 2.0 * uvf);
	let x0 = mix(Rand2(vec2<f32>(uvi + vec2<i32>(0, 0))), Rand2(vec2<f32>(uvi + vec2<i32>(1, 0))), uvf.x);
	let x1 = mix(Rand2(vec2<f32>(uvi + vec2<i32>(0, 1))), Rand2(vec2<f32>(uvi + vec2<i32>(1, 1))), uvf.x);
	return mix(x0, x1, uvf.y);
}

fn SimpleNoise(uv : vec2<f32>, scale : f32) -> f32 {
	var ret = 0.0;
	for (var i = 0; i < 3; i = i + 1) {
		let duration = pow(2.0, f32(i));
		let intensity = pow(0.5, f32(3 - i));
		ret = ret + SimpleNoise_Block(uv * scale / duration) * intensity;
	}
	return ret;
}

fn CellularNoise(uv_: vec2<f32>, scale : f32) -> f32 {
	var uv = uv_ * scale;
	let iuv = floor(uv);
	let fuv = fract(uv);
	var dist = 5.0;
	for (var y = -1; y <= 1; y = y + 1) {
		for (var x = -1; x <= 1; x = x + 1) {
			let neighbor = vec2<f32>(f32(x), f32(y));
			let diff = neighbor + vec2<f32>(Rand2(iuv + neighbor)) - fuv;
			dist = min(dist, length(diff));
		}
	}
	return dist;
}

fn RGBToHSV(rgb : vec3<f32>) -> vec3<f32> {
	let k = vec4<f32>(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
	let p = mix(vec4<f32>(rgb.bg, k.wz), vec4<f32>(rgb.gb, k.xy), step(rgb.b, rgb.g));
	let q = mix(vec4<f32>(p.xyw, rgb.r), vec4<f32>(rgb.r, p.yzx), step(p.x, rgb.r));
	let d = q.x - min(q.w, q.y);
	let e = 1.0e-10;
	return vec3<f32>(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

fn HSVToRGB(hsv : vec3<f32>) -> vec3<f32> {
	let k = vec4<f32>(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
	let p = abs(fract(hsv.xxx + k.xyz) * 6.0 - k.www);
	return hsv.z * mix(k.xxx, clamp(p - k.xxx, vec3<f32>(0.0), vec3<f32>(1.0)), hsv.y);
}
)";

	if (isPixelShader)
	{
		ss << R"(
fn GetUV(uvIn : vec2<f32>) -> vec2<f32> {
	var uv = uvIn;
	uv.y = v.mUVInversedBack.x + v.mUVInversedBack.y * uv.y;
	return uv;
}

fn GetUVBack(uvIn : vec2<f32>) -> vec2<f32> {
	var uv = uvIn;
	uv.y = v.mUVInversedBack.z + v.mUVInversedBack.w * uv.y;
	return uv;
}
)";
		if (includeDepthFade)
		{
			ss << R"(
fn CalcDepthFade(screenUV : vec2<f32>, meshZ : f32, softParticleParam : f32) -> f32 {
	let backgroundZ = textureSample(efk_depth_texture, efk_depth_sampler, GetUVBack(screenUV)).x;
	let distance = softParticleParam * v.predefined_uniform.y;
	let rescale = v.reconstructionParam1.xy;
	let params = v.reconstructionParam2;
	let zs = vec2<f32>(backgroundZ * rescale.x + rescale.y, meshZ);
	var depth = (zs * params.w - vec2<f32>(params.y)) / (vec2<f32>(params.x) - zs * params.z);
	let dir = sign(depth.x);
	depth = depth * dir;
	return min(max((depth.x - depth.y) / distance, 0.0), 1.0);
}
)";
		}
		else
		{
			ss << R"(
fn CalcDepthFade(screenUV : vec2<f32>, meshZ : f32, softParticleParam : f32) -> f32 {
	return 1.0;
}
)";
		}
	}
	else
	{
		ss << R"(
fn GetUV(uvIn : vec2<f32>) -> vec2<f32> {
	var uv = uvIn;
	uv.y = v.mUVInversed.x + v.mUVInversed.y * uv.y;
	return uv;
}

fn GetUVBack(uvIn : vec2<f32>) -> vec2<f32> {
	var uv = uvIn;
	uv.y = v.mUVInversed.z + v.mUVInversed.w * uv.y;
	return uv;
}

fn CalcDepthFade(screenUV : vec2<f32>, meshZ : f32, softParticleParam : f32) -> f32 {
	return 1.0;
}
)";
	}

	return ss.str();
}

std::string GetLightFunctions(bool isPixelShader)
{
	if (!isPixelShader)
	{
		return R"(
fn GetLightDirection() -> vec3<f32> {
	return vec3<f32>(0.0);
}

fn GetLightColor() -> vec3<f32> {
	return vec3<f32>(0.0);
}

fn GetLightAmbientColor() -> vec3<f32> {
	return vec3<f32>(0.0);
}
)";
	}

	return R"(
fn GetLightDirection() -> vec3<f32> {
	return v.lightDirection.xyz;
}

fn GetLightColor() -> vec3<f32> {
	return v.lightColor.xyz;
}

fn GetLightAmbientColor() -> vec3<f32> {
	return v.lightAmbientColor.xyz;
}
)";
}

std::string GetPixelLightingFunctions()
{
	return R"(
const lightScale = 3.14;

fn calcD_GGX(roughness : f32, dotNH : f32) -> f32 {
	let alpha = roughness * roughness;
	let alphaSqr = alpha * alpha;
	let pi = 3.14159;
	let denom = dotNH * dotNH * (alphaSqr - 1.0) + 1.0;
	return (alpha / denom) * (alpha / denom) / pi;
}

fn calcF(F0 : f32, dotLH : f32) -> f32 {
	let dotLH5 = pow(1.0 - dotLH, 5.0);
	return F0 + (1.0 - F0) * dotLH5;
}

fn calcG_Schlick(roughness : f32, dotNV : f32, dotNL : f32) -> f32 {
	let k = (roughness + 1.0) * (roughness + 1.0) / 8.0;
	let gV = dotNV * (1.0 - k) + k;
	let gL = dotNL * (1.0 - k) + k;
	return 1.0 / (gV * gL);
}

fn calcLightingGGX(N : vec3<f32>, V : vec3<f32>, L : vec3<f32>, roughness : f32, F0 : f32) -> f32 {
	let H = normalize(V + L);
	let dotNL = clamp(dot(N, L), 0.0, 1.0);
	let dotLH = clamp(dot(L, H), 0.0, 1.0);
	let dotNH = clamp(dot(N, H), 0.0, 1.0) - 0.001;
	let dotNV = clamp(dot(N, V), 0.0, 1.0) + 0.001;
	let D = calcD_GGX(roughness, dotNH);
	let F = calcF(F0, dotLH);
	let G = calcG_Schlick(roughness, dotNV, dotNL);
	return dotNL * D * F * G / 4.0;
}

fn calcDirectionalLightDiffuseColor(diffuseColor : vec3<f32>, normal : vec3<f32>, lightDir : vec3<f32>, ao : f32) -> vec3<f32> {
	let NoL = dot(normal, lightDir);
	var color = v.lightColor.xyz * lightScale * max(NoL, 0.0) * ao / 3.14;
	color = color * diffuseColor;
	return color;
}
)";
}

} // namespace

std::string ShaderGenerator::Replace(std::string target, const std::string& from, const std::string& to) const
{
	return ReplaceAll(std::move(target), from, to);
}

std::string ShaderGenerator::GetType(int32_t i) const
{
	if (i == 1)
		return "f32";
	if (i == 2)
		return "vec2<f32>";
	if (i == 3)
		return "vec3<f32>";
	if (i == 4)
		return "vec4<f32>";
	if (i == 16)
		return "mat4x4<f32>";
	assert(0);
	return "";
}

std::string ShaderGenerator::GetElement(int32_t i) const
{
	if (i == 1)
		return "";
	if (i == 2)
		return ".xy";
	if (i == 3)
		return ".xyz";
	if (i == 4)
		return ".xyzw";
	assert(0);
	return "";
}

std::string ShaderGenerator::ExportResources(MaterialFile* materialFile, int stage, int32_t maximumTextureCount) const
{
	std::ostringstream ss;

	const auto actualTextureCount = std::min(maximumTextureCount, materialFile->GetTextureCount());
	for (int32_t i = 0; i < actualTextureCount; i++)
	{
		const auto name = materialFile->GetTextureName(i);
		ss << "@group(1) @binding(" << i << ") var " << name << "_texture : texture_2d<f32>;\n";
		ss << "@group(2) @binding(" << i << ") var " << name << "_sampler : sampler;\n\n";
	}

	const auto backgroundBinding = actualTextureCount;
	ss << "@group(1) @binding(" << backgroundBinding << ") var efk_background_texture : texture_2d<f32>;\n";
	ss << "@group(2) @binding(" << backgroundBinding << ") var efk_background_sampler : sampler;\n\n";

	if (stage == 1 && UsesDepthFade(materialFile))
	{
		const auto depthBinding = actualTextureCount + 1;
		ss << "@group(1) @binding(" << depthBinding << ") var efk_depth_texture : texture_2d<f32>;\n";
		ss << "@group(2) @binding(" << depthBinding << ") var efk_depth_sampler : sampler;\n\n";
	}

	return ss.str();
}

std::string ShaderGenerator::ExportUniformBlock(MaterialFile* materialFile,
												MaterialShaderType shaderType,
												int stage,
												bool isSprite,
												int32_t maximumUniformCount,
												int32_t instanceCount) const
{
	std::ostringstream ss;

	if (stage == 0)
	{
		ss << "struct VSConstantBuffer {\n";
		if (isSprite)
		{
			ss << "\tmCamera : mat4x4<f32>,\n";
			ss << "\tmProj : mat4x4<f32>,\n";
			ss << "\tmUVInversed : vec4<f32>,\n";
			ss << "\tpredefined_uniform : vec4<f32>,\n";
			ss << "\tcameraPosition : vec4<f32>,\n";
		}
		else
		{
			ss << "\tmCameraProj : mat4x4<f32>,\n";
			ss << "\tmModel : array<mat4x4<f32>, " << instanceCount << ">,\n";
			ss << "\tfUV : array<vec4<f32>, " << instanceCount << ">,\n";
			ss << "\tfModelColor : array<vec4<f32>, " << instanceCount << ">,\n";
			ss << "\tfModelParticleTime : array<vec4<f32>, " << instanceCount << ">,\n";
			ss << "\tmUVInversed : vec4<f32>,\n";
			ss << "\tpredefined_uniform : vec4<f32>,\n";
			ss << "\tcameraPosition : vec4<f32>,\n";
			if (materialFile->GetCustomData1Count() > 0)
			{
				ss << "\tcustomData1 : array<vec4<f32>, " << instanceCount << ">,\n";
			}
			if (materialFile->GetCustomData2Count() > 0)
			{
				ss << "\tcustomData2 : array<vec4<f32>, " << instanceCount << ">,\n";
			}
		}
	}
	else
	{
		ss << "struct PSConstantBuffer {\n";
		ss << "\tmUVInversedBack : vec4<f32>,\n";
		ss << "\tpredefined_uniform : vec4<f32>,\n";
		ss << "\tcameraPosition : vec4<f32>,\n";
		ss << "\treconstructionParam1 : vec4<f32>,\n";
		ss << "\treconstructionParam2 : vec4<f32>,\n";
		ss << "\tlightDirection : vec4<f32>,\n";
		ss << "\tlightColor : vec4<f32>,\n";
		ss << "\tlightAmbientColor : vec4<f32>,\n";
		if (shaderType == MaterialShaderType::Refraction || shaderType == MaterialShaderType::RefractionModel)
		{
			ss << "\tcameraMat : mat4x4<f32>,\n";
		}
	}

	const auto actualUniformCount = std::min(maximumUniformCount, materialFile->GetUniformCount());
	for (int32_t i = 0; i < actualUniformCount; i++)
	{
		ss << "\t" << materialFile->GetUniformName(i) << " : vec4<f32>,\n";
	}

	for (size_t i = 0; i < materialFile->Gradients.size(); i++)
	{
		for (int32_t j = 0; j < GradientUniformCount; j++)
		{
			ss << "\t" << materialFile->Gradients[i].Name << "_" << j << " : vec4<f32>,\n";
		}
	}

	ss << "}\n\n";
	ss << "@group(0) @binding(" << stage << ") var<uniform> v : " << (stage == 0 ? "VSConstantBuffer" : "PSConstantBuffer") << ";\n\n";

	return ss.str();
}

std::string ShaderGenerator::ConvertGenericCode(MaterialFile* materialFile, int stage, int32_t maximumTextureCount) const
{
	auto baseCode = std::string(materialFile->GetGenericCode());
	baseCode = Replace(baseCode, "$F1$", "f32");
	baseCode = Replace(baseCode, "$F2$", "vec2<f32>");
	baseCode = Replace(baseCode, "$F3$", "vec3<f32>");
	baseCode = Replace(baseCode, "$F4$", "vec4<f32>");
	baseCode = Replace(baseCode, "$TIME$", "predefined_uniform.x");
	baseCode = Replace(baseCode, "$EFFECTSCALE$", "predefined_uniform.y");
	baseCode = Replace(baseCode, "$LOCALTIME$", "predefined_uniform.w");
	baseCode = Replace(baseCode, "$UV$", "uv");
	baseCode = Replace(baseCode, "$MOD", "mod_f32");
	baseCode = Replace(baseCode, "$PARTICLE_TIME_NORMALIZED$", "particleTime.x");
	baseCode = Replace(baseCode, "$PARTICLE_TIME_SECONDS$", "particleTime.y");

	const auto actualTextureCount = std::min(maximumTextureCount, materialFile->GetTextureCount());
	for (int32_t i = 0; i < actualTextureCount; i++)
	{
		std::string prefix;
		std::string suffix;

		if (materialFile->GetTextureColorType(i) == Effekseer::TextureColorType::Color)
		{
			prefix = "ConvertFromSRGBTexture(";
			suffix = ")";
		}

		const auto textureIndex = materialFile->GetTextureIndex(i);
		const auto textureName = std::string(materialFile->GetTextureName(i));
		const auto keyP = "$TEX_P" + std::to_string(textureIndex) + "$";
		const auto keyS = "$TEX_S" + std::to_string(textureIndex) + "$";

		if (stage == 0)
		{
			baseCode = Replace(baseCode,
							   keyP,
							   prefix + "textureSampleLevel(" + textureName + "_texture, " + textureName + "_sampler, GetUV(");
			baseCode = Replace(baseCode, keyS, "), 0.0)" + suffix);
		}
		else
		{
			baseCode =
				Replace(baseCode, keyP, prefix + "textureSample(" + textureName + "_texture, " + textureName + "_sampler, GetUV(");
			baseCode = Replace(baseCode, keyS, "))" + suffix);
		}
	}

	for (int32_t i = actualTextureCount; i < materialFile->GetTextureCount(); i++)
	{
		const auto textureIndex = materialFile->GetTextureIndex(i);
		const auto keyP = "$TEX_P" + std::to_string(textureIndex) + "$";
		const auto keyS = "$TEX_S" + std::to_string(textureIndex) + "$";

		baseCode = Replace(baseCode, keyP, "vec4<f32>(");
		baseCode = Replace(baseCode, keyS, ", 0.0, 1.0)");
	}

	return ConvertMaterialCodeLines(baseCode);
}

std::string ShaderGenerator::GenerateVertexShader(MaterialFile* materialFile,
												  MaterialShaderType shaderType,
												  int32_t maximumUniformCount,
												  int32_t maximumTextureCount,
												  int32_t instanceCount) const
{
	const bool isSprite = shaderType == MaterialShaderType::Standard || shaderType == MaterialShaderType::Refraction;
	const bool isSimple = isSprite && materialFile->GetIsSimpleVertex();
	std::ostringstream ss;

	ss << "diagnostic(off, derivative_uniformity);\n\n";
	ss << ExportUniformBlock(materialFile, shaderType, 0, isSprite, maximumUniformCount, instanceCount);
	ss << ExportResources(materialFile, 0, maximumTextureCount);
	ss << GetCommonFunctions(false, false);
	if (HasRequiredMethod(materialFile, MaterialFile::RequiredPredefinedMethodType::Light))
	{
		ss << GetLightFunctions(false);
	}
	for (const auto& gradient : materialFile->FixedGradients)
	{
		ss << ExportFixedGradient(gradient);
	}

	ss << "struct VSOutput {\n";
	ss << "\t@builtin(position) Position : vec4<f32>,\n";
	ss << "\t@location(0) VColor : vec4<f32>,\n";
	ss << "\t@location(1) UV1 : vec2<f32>,\n";
	ss << "\t@location(2) UV2 : vec2<f32>,\n";
	ss << "\t@location(3) WorldP : vec3<f32>,\n";
	ss << "\t@location(4) WorldN : vec3<f32>,\n";
	ss << "\t@location(5) WorldT : vec3<f32>,\n";
	ss << "\t@location(6) WorldB : vec3<f32>,\n";
	ss << "\t@location(7) PosP : vec4<f32>,\n";
	ss << "\t@location(8) ParticleTime : vec2<f32>,\n";
	if (materialFile->GetCustomData1Count() > 0)
	{
		ss << "\t@location(9) CustomData1 : " << GetType(materialFile->GetCustomData1Count()) << ",\n";
	}
	if (materialFile->GetCustomData2Count() > 0)
	{
		ss << "\t@location(10) CustomData2 : " << GetType(materialFile->GetCustomData2Count()) << ",\n";
	}
	ss << "}\n\n";

	ss << "@vertex\n";
	if (isSimple)
	{
		ss << "fn main(@location(0) atPosition : vec3<f32>, @location(1) atColor : vec4<f32>, @location(2) atTexCoord : vec2<f32>, @location(3) atParticleTime : vec2<f32>) -> VSOutput {\n";
	}
	else if (isSprite)
	{
		ss << "fn main(@location(0) atPosition : vec3<f32>, @location(1) atColor : vec4<f32>, @location(2) atNormal : vec4<f32>, @location(3) atTangent : vec4<f32>, @location(4) atTexCoord : vec2<f32>, @location(5) atTexCoord2 : vec2<f32>, @location(6) atParticleTime : vec2<f32>";
		auto nextLocation = 7;
		if (materialFile->GetCustomData1Count() > 0)
		{
			ss << ", @location(" << nextLocation++ << ") atCustomData1 : " << GetType(materialFile->GetCustomData1Count());
		}
		if (materialFile->GetCustomData2Count() > 0)
		{
			ss << ", @location(" << nextLocation++ << ") atCustomData2 : " << GetType(materialFile->GetCustomData2Count());
		}
		ss << ") -> VSOutput {\n";
	}
	else
	{
		ss << "fn main(@location(0) a_Position : vec3<f32>, @location(1) a_Normal : vec3<f32>, @location(2) a_Binormal : vec3<f32>, @location(3) a_Tangent : vec3<f32>, @location(4) a_TexCoord1 : vec2<f32>, @location(5) a_TexCoord2 : vec2<f32>, @location(6) a_Color : vec4<f32>, @builtin(instance_index) instanceIndex : u32) -> VSOutput {\n";
	}

	ss << "\tvar Output : VSOutput;\n";
	ss << "\tlet predefined_uniform = v.predefined_uniform;\n";
	ss << "\tlet cameraPosition = v.cameraPosition;\n";

	const auto actualUniformCount = std::min(maximumUniformCount, materialFile->GetUniformCount());
	for (int32_t i = 0; i < actualUniformCount; i++)
	{
		ss << "\tlet " << materialFile->GetUniformName(i) << " = v." << materialFile->GetUniformName(i) << ";\n";
	}
	for (int32_t i = actualUniformCount; i < materialFile->GetUniformCount(); i++)
	{
		ss << "\tlet " << materialFile->GetUniformName(i) << " = vec4<f32>(0.0, 0.0, 0.0, 0.0);\n";
	}
	for (const auto& gradient : materialFile->Gradients)
	{
		for (int32_t j = 0; j < GradientUniformCount; j++)
		{
			ss << "\tlet " << gradient.Name << "_" << j << " = v." << gradient.Name << "_" << j << ";\n";
		}
	}

	if (isSimple)
	{
		ss << R"(
	var worldPos = atPosition;
	var worldNormal = vec3<f32>(0.0);
	var worldBinormal = vec3<f32>(0.0);
	var worldTangent = vec3<f32>(0.0);
	var objectScale = vec3<f32>(1.0);
	var uv1 = atTexCoord;
	var uv2 = atTexCoord;
	var pixelNormalDir = worldNormal;
	var vcolor = atColor;
	var particleTime = atParticleTime;
	let isFrontFace = false;
	let screenUV = vec2<f32>(0.0);
	let meshZ = 0.0;
)";
	}
	else if (isSprite)
	{
		ss << R"(
	var worldPos = atPosition;
	var worldNormal = (atNormal.xyz - vec3<f32>(0.5)) * 2.0;
	var worldTangent = (atTangent.xyz - vec3<f32>(0.5)) * 2.0;
	var worldBinormal = cross(worldNormal, worldTangent);
	var objectScale = vec3<f32>(1.0);
	var uv1 = atTexCoord;
	var uv2 = atTexCoord2;
	var pixelNormalDir = worldNormal;
	var vcolor = atColor;
	var particleTime = atParticleTime;
	let isFrontFace = false;
	let screenUV = vec2<f32>(0.0);
	let meshZ = 0.0;
)";
		if (materialFile->GetCustomData1Count() > 0)
		{
			ss << "\tlet customData1 = atCustomData1;\n";
			ss << "\tOutput.CustomData1 = customData1" << GetElement(materialFile->GetCustomData1Count()) << ";\n";
		}
		if (materialFile->GetCustomData2Count() > 0)
		{
			ss << "\tlet customData2 = atCustomData2;\n";
			ss << "\tOutput.CustomData2 = customData2" << GetElement(materialFile->GetCustomData2Count()) << ";\n";
		}
	}
	else
	{
		ss << R"(
	let index = instanceIndex;
	let matModel = v.mModel[index];
	let uv = v.fUV[index];
	let modelColor = v.fModelColor[index] * a_Color;
	var particleTime = v.fModelParticleTime[index].xy;
	let localPosition = vec4<f32>(a_Position.x, a_Position.y, a_Position.z, 1.0);
	var worldPos = (matModel * localPosition).xyz;
	var worldNormal = normalize((matModel * vec4<f32>(a_Normal, 0.0)).xyz);
	var worldBinormal = normalize((matModel * vec4<f32>(a_Binormal, 0.0)).xyz);
	var worldTangent = normalize((matModel * vec4<f32>(a_Tangent, 0.0)).xyz);
	var objectScale = vec3<f32>(
		length((matModel * vec4<f32>(1.0, 0.0, 0.0, 0.0)).xyz),
		length((matModel * vec4<f32>(0.0, 1.0, 0.0, 0.0)).xyz),
		length((matModel * vec4<f32>(0.0, 0.0, 1.0, 0.0)).xyz));
	var uv1 = a_TexCoord1 * uv.zw + uv.xy;
	var uv2 = a_TexCoord2 * uv.zw + uv.xy;
	var pixelNormalDir = worldNormal;
	var vcolor = modelColor;
	let isFrontFace = false;
	let screenUV = vec2<f32>(0.0);
	let meshZ = 0.0;
)";
		if (materialFile->GetCustomData1Count() > 0)
		{
			ss << "\tlet customData1 = v.customData1[index]" << GetElement(materialFile->GetCustomData1Count()) << ";\n";
			ss << "\tOutput.CustomData1 = customData1;\n";
		}
		if (materialFile->GetCustomData2Count() > 0)
		{
			ss << "\tlet customData2 = v.customData2[index]" << GetElement(materialFile->GetCustomData2Count()) << ";\n";
			ss << "\tOutput.CustomData2 = customData2;\n";
		}
	}

	ss << ConvertGenericCode(materialFile, 0, maximumTextureCount);

	if (isSprite)
	{
		ss << R"(
	worldPos = worldPos + worldPositionOffset;
	let cameraPos = v.mCamera * vec4<f32>(worldPos, 1.0);
	Output.Position = v.mProj * (cameraPos / cameraPos.w);
	Output.WorldP = worldPos;
	Output.WorldN = worldNormal;
	Output.WorldB = worldBinormal;
	Output.WorldT = worldTangent;
	Output.VColor = vcolor;
	Output.UV1 = uv1;
	Output.UV2 = uv2;
	Output.ParticleTime = particleTime;
	Output.PosP = Output.Position;
	return Output;
}
)";
	}
	else
	{
		ss << R"(
	worldPos = worldPos + worldPositionOffset;
	Output.Position = v.mCameraProj * vec4<f32>(worldPos, 1.0);
	Output.WorldP = worldPos;
	Output.WorldN = worldNormal;
	Output.WorldB = worldBinormal;
	Output.WorldT = worldTangent;
	Output.VColor = vcolor;
	Output.UV1 = uv1;
	Output.UV2 = uv2;
	Output.ParticleTime = particleTime;
	Output.PosP = Output.Position;
	return Output;
}
)";
	}

	return ss.str();
}

std::string ShaderGenerator::GeneratePixelShader(MaterialFile* materialFile,
												 MaterialShaderType shaderType,
												 int32_t maximumUniformCount,
												 int32_t maximumTextureCount) const
{
	const bool isRefraction = shaderType == MaterialShaderType::Refraction || shaderType == MaterialShaderType::RefractionModel;
	std::ostringstream ss;

	ss << "diagnostic(off, derivative_uniformity);\n\n";
	ss << ExportUniformBlock(materialFile, shaderType, 1, shaderType == MaterialShaderType::Standard || shaderType == MaterialShaderType::Refraction, maximumUniformCount, 0);
	ss << ExportResources(materialFile, 1, maximumTextureCount);
	ss << GetCommonFunctions(true, UsesDepthFade(materialFile));
	if (HasRequiredMethod(materialFile, MaterialFile::RequiredPredefinedMethodType::Light))
	{
		ss << GetLightFunctions(true);
	}
	if (materialFile->GetShadingModel() == Effekseer::ShadingModelType::Lit)
	{
		ss << GetPixelLightingFunctions();
	}
	for (const auto& gradient : materialFile->FixedGradients)
	{
		ss << ExportFixedGradient(gradient);
	}

	ss << "struct PSInput {\n";
	ss << "\t@location(0) VColor : vec4<f32>,\n";
	ss << "\t@location(1) UV1 : vec2<f32>,\n";
	ss << "\t@location(2) UV2 : vec2<f32>,\n";
	ss << "\t@location(3) WorldP : vec3<f32>,\n";
	ss << "\t@location(4) WorldN : vec3<f32>,\n";
	ss << "\t@location(5) WorldT : vec3<f32>,\n";
	ss << "\t@location(6) WorldB : vec3<f32>,\n";
	ss << "\t@location(7) PosP : vec4<f32>,\n";
	ss << "\t@location(8) ParticleTime : vec2<f32>,\n";
	if (materialFile->GetCustomData1Count() > 0)
	{
		ss << "\t@location(9) CustomData1 : " << GetType(materialFile->GetCustomData1Count()) << ",\n";
	}
	if (materialFile->GetCustomData2Count() > 0)
	{
		ss << "\t@location(10) CustomData2 : " << GetType(materialFile->GetCustomData2Count()) << ",\n";
	}
	ss << "}\n\n";

	ss << "@fragment\n";
	ss << "fn main(Input : PSInput, @builtin(front_facing) face : bool) -> @location(0) vec4<f32> {\n";
	ss << "\tlet predefined_uniform = v.predefined_uniform;\n";
	ss << "\tlet cameraPosition = v.cameraPosition;\n";
	ss << "\tlet reconstructionParam1 = v.reconstructionParam1;\n";
	ss << "\tlet reconstructionParam2 = v.reconstructionParam2;\n";
	ss << "\tlet lightDirection = v.lightDirection;\n";
	ss << "\tlet lightColor = v.lightColor;\n";
	ss << "\tlet lightAmbientColor = v.lightAmbientColor;\n";

	const auto actualUniformCount = std::min(maximumUniformCount, materialFile->GetUniformCount());
	for (int32_t i = 0; i < actualUniformCount; i++)
	{
		ss << "\tlet " << materialFile->GetUniformName(i) << " = v." << materialFile->GetUniformName(i) << ";\n";
	}
	for (int32_t i = actualUniformCount; i < materialFile->GetUniformCount(); i++)
	{
		ss << "\tlet " << materialFile->GetUniformName(i) << " = vec4<f32>(0.0, 0.0, 0.0, 0.0);\n";
	}
	for (const auto& gradient : materialFile->Gradients)
	{
		for (int32_t j = 0; j < GradientUniformCount; j++)
		{
			ss << "\tlet " << gradient.Name << "_" << j << " = v." << gradient.Name << "_" << j << ";\n";
		}
	}

	ss << R"(
	var uv1 = Input.UV1;
	var uv2 = Input.UV2;
	var worldPos = Input.WorldP;
	var worldNormal = Input.WorldN;
	var worldBinormal = Input.WorldB;
	var worldTangent = Input.WorldT;
	var objectScale = vec3<f32>(1.0);
	var pixelNormalDir = worldNormal;
	var vcolor = Input.VColor;
	var particleTime = Input.ParticleTime;
	let isFrontFace = face;
	var screenUV = Input.PosP.xy / Input.PosP.w;
	let meshZ = Input.PosP.z / Input.PosP.w;
	screenUV = vec2<f32>(screenUV.x + 1.0, 1.0 - screenUV.y) * 0.5;
)";
	if (materialFile->GetCustomData1Count() > 0)
	{
		ss << "\tlet customData1 = Input.CustomData1;\n";
	}
	if (materialFile->GetCustomData2Count() > 0)
	{
		ss << "\tlet customData2 = Input.CustomData2;\n";
	}

	ss << ConvertGenericCode(materialFile, 1, maximumTextureCount);

	if (isRefraction)
	{
		ss << R"(
	let airRefraction = 1.0;
	var dir = (v.cameraMat * vec4<f32>(pixelNormalDir, 0.0)).xyz;
	dir.y = -dir.y;
	var distortUV = dir.xy * (refraction - airRefraction);
	distortUV = distortUV + screenUV;
	distortUV = GetUVBack(distortUV);
	let Output = textureSample(efk_background_texture, efk_background_sampler, distortUV);
	if (opacityMask <= 0.0) {
		discard;
	}
	if (opacity <= 0.0) {
		discard;
	}
	return Output;
}
)";
	}
	else if (materialFile->GetShadingModel() == Effekseer::ShadingModelType::Lit)
	{
		ss << R"(
	let viewDir = normalize(v.cameraPosition.xyz - worldPos);
	let diffuse = calcDirectionalLightDiffuseColor(baseColor, pixelNormalDir, v.lightDirection.xyz, ambientOcclusion);
	let specular = v.lightColor.xyz * lightScale * calcLightingGGX(pixelNormalDir, viewDir, v.lightDirection.xyz, roughness, 0.9);
	var Output = vec4<f32>(metallic * specular + (1.0 - metallic) * diffuse + baseColor * v.lightAmbientColor.xyz * ambientOcclusion, opacity);
	Output = vec4<f32>(Output.xyz + emissive.xyz, Output.w);
	if (opacityMask <= 0.0) {
		discard;
	}
	if (opacity <= 0.0) {
		discard;
	}
	return ConvertToScreen(Output);
}
)";
	}
	else
	{
		ss << R"(
	let Output = vec4<f32>(emissive, opacity);
	if (opacityMask <= 0.0) {
		discard;
	}
	if (opacity <= 0.0) {
		discard;
	}
	return ConvertToScreen(Output);
}
)";
	}

	return ss.str();
}

ShaderData ShaderGenerator::GenerateShader(MaterialFile* materialFile,
										   MaterialShaderType shaderType,
										   int32_t maximumUniformCount,
										   int32_t maximumTextureCount,
										   int32_t instanceCount) const
{
	ShaderData shaderData;
	shaderData.CodeVS = GenerateVertexShader(materialFile, shaderType, maximumUniformCount, maximumTextureCount, instanceCount);
	shaderData.CodePS = GeneratePixelShader(materialFile, shaderType, maximumUniformCount, maximumTextureCount);
	return shaderData;
}

} // namespace WGSL
} // namespace Effekseer
