#include "GroundRendering.h"

#include <cassert>

namespace Effekseer
{
namespace ToolRuntime
{
namespace
{

const auto ground_vs_dx9 = R"(
struct VS_INPUT
{
	float4 Position : POSITION0;
	float2 WorldXZ : TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 Position : POSITION0;
	float2 WorldXZ : TEXCOORD0;
	float4 Clip : TEXCOORD1;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	output.Position = input.Position;
	output.WorldXZ = input.WorldXZ;
	output.Clip = input.Position;
	return output;
}
)";

const auto ground_ps_dx9 = R"(
struct PS_INPUT
{
	float2 WorldXZ : TEXCOORD0;
};

float4 main(PS_INPUT input) : COLOR0
{
	float checker = frac((floor(input.WorldXZ.x) + floor(input.WorldXZ.y)) * 0.5);
	float3 darkColor = float3(0.24, 0.32, 0.27);
	float3 brightColor = float3(0.39, 0.50, 0.42);
	float3 color = lerp(darkColor, brightColor, checker >= 0.5 ? 1.0 : 0.0);
	float distanceFade = saturate(length(input.WorldXZ) * 0.025);
	color *= 1.0 - distanceFade * 0.35;
	return float4(color, 1.0);
}
)";

const auto ground_depth_ps_dx9 = R"(
struct PS_INPUT
{
	float4 Clip : TEXCOORD1;
};

float4 main(PS_INPUT input) : COLOR0
{
	return float4(input.Clip.z / input.Clip.w, 1.0, 1.0, 1.0);
}
)";

const auto ground_vs_dx11 = R"(
struct VS_INPUT
{
	float4 Position : POSITION0;
	float2 WorldXZ : TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float2 WorldXZ : TEXCOORD0;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	output.Position = input.Position;
	output.WorldXZ = input.WorldXZ;
	return output;
}
)";

const auto ground_ps_dx11 = R"(
struct PS_INPUT
{
	float4 Position : SV_POSITION;
	float2 WorldXZ : TEXCOORD0;
};

float4 main(PS_INPUT input) : SV_TARGET
{
	float checker = frac((floor(input.WorldXZ.x) + floor(input.WorldXZ.y)) * 0.5);
	float3 darkColor = float3(0.24, 0.32, 0.27);
	float3 brightColor = float3(0.39, 0.50, 0.42);
	float3 color = lerp(darkColor, brightColor, checker >= 0.5 ? 1.0 : 0.0);
	float distanceFade = saturate(length(input.WorldXZ) * 0.025);
	color *= 1.0 - distanceFade * 0.35;
	return float4(color, 1.0);
}
)";

const auto ground_depth_ps_dx11 = R"(
struct PS_INPUT
{
	float4 Position : SV_POSITION;
	float2 WorldXZ : TEXCOORD0;
};

float4 main(PS_INPUT input) : SV_TARGET
{
	return float4(input.Position.z, 1.0, 1.0, 1.0);
}
)";

const auto ground_vs_dx12 = R"(
struct VS_INPUT
{
	float4 Position : POSITION0;
	float2 WorldXZ : UV0;
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float2 WorldXZ : UV0;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	output.Position = input.Position;
	output.WorldXZ = input.WorldXZ;
	return output;
}
)";

const auto ground_ps_dx12 = R"(
struct PS_INPUT
{
	float4 Position : SV_POSITION;
	float2 WorldXZ : UV0;
};

float4 main(PS_INPUT input) : SV_TARGET
{
	float checker = frac((floor(input.WorldXZ.x) + floor(input.WorldXZ.y)) * 0.5);
	float3 darkColor = float3(0.24, 0.32, 0.27);
	float3 brightColor = float3(0.39, 0.50, 0.42);
	float3 color = lerp(darkColor, brightColor, step(0.5, checker));
	float distanceFade = saturate(length(input.WorldXZ) * 0.025);
	color *= 1.0 - distanceFade * 0.35;
	return float4(color, 1.0);
}
)";

const auto ground_depth_ps_dx12 = R"(
struct PS_INPUT
{
	float4 Position : SV_POSITION;
	float2 WorldXZ : UV0;
};

float4 main(PS_INPUT input) : SV_TARGET
{
	return float4(input.Position.z, 1.0, 1.0, 1.0);
}
)";

const auto ground_vs_gl = R"(
#version 330

layout(location = 0) in vec4 input_Position;
layout(location = 1) in vec2 input_WorldXZ;
out vec2 output_WorldXZ;

void main()
{
	gl_Position = input_Position;
	output_WorldXZ = input_WorldXZ;
}
)";

const auto ground_ps_gl = R"(
#version 330

in vec2 output_WorldXZ;
layout(location = 0) out vec4 output_Color;

void main()
{
	float checker = fract((floor(output_WorldXZ.x) + floor(output_WorldXZ.y)) * 0.5);
	vec3 darkColor = vec3(0.24, 0.32, 0.27);
	vec3 brightColor = vec3(0.39, 0.50, 0.42);
	vec3 color = mix(darkColor, brightColor, step(0.5, checker));
	float distanceFade = clamp(length(output_WorldXZ) * 0.025, 0.0, 1.0);
	color *= 1.0 - distanceFade * 0.35;
	output_Color = vec4(color, 1.0);
}
)";

const auto ground_depth_ps_gl = R"(
#version 330

in vec2 output_WorldXZ;
layout(location = 0) out vec4 output_Color;

void main()
{
	output_Color = vec4(gl_FragCoord.z, 1.0, 1.0, 1.0);
}
)";

const auto ground_vs_vulkan = R"(
#version 420

layout(location = 0) in vec4 input_Position;
layout(location = 1) in vec2 input_WorldXZ;
layout(location = 0) out vec2 output_WorldXZ;

void main()
{
	vec4 position = input_Position;
	position.y = -position.y;
	gl_Position = position;
	output_WorldXZ = input_WorldXZ;
}
)";

const auto ground_ps_vulkan = R"(
#version 420

layout(location = 0) in vec2 input_WorldXZ;
layout(location = 0) out vec4 output_Color;

void main()
{
	float checker = fract((floor(input_WorldXZ.x) + floor(input_WorldXZ.y)) * 0.5);
	vec3 darkColor = vec3(0.24, 0.32, 0.27);
	vec3 brightColor = vec3(0.39, 0.50, 0.42);
	vec3 color = mix(darkColor, brightColor, step(0.5, checker));
	float distanceFade = clamp(length(input_WorldXZ) * 0.025, 0.0, 1.0);
	color *= 1.0 - distanceFade * 0.35;
	output_Color = vec4(color, 1.0);
}
)";

const auto ground_depth_ps_vulkan = R"(
#version 420

layout(location = 0) in vec2 input_WorldXZ;
layout(location = 0) out vec4 output_Color;

void main()
{
	output_Color = vec4(gl_FragCoord.z, 1.0, 1.0, 1.0);
}
)";

const auto ground_vs_metal = R"(
#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct main0_in
{
	float4 Position [[attribute(0)]];
	float2 WorldXZ [[attribute(1)]];
};

struct main0_out
{
	float2 WorldXZ [[user(locn0)]];
	float4 Position [[position]];
};

vertex main0_out main0(main0_in input [[stage_in]])
{
	main0_out output = {};
	output.Position = input.Position;
	output.WorldXZ = input.WorldXZ;
	return output;
}
)";

const auto ground_ps_metal = R"(
#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct main0_in
{
	float2 WorldXZ [[user(locn0)]];
};

fragment float4 main0(main0_in input [[stage_in]])
{
	float checker = fract((floor(input.WorldXZ.x) + floor(input.WorldXZ.y)) * 0.5);
	float3 darkColor = float3(0.24, 0.32, 0.27);
	float3 brightColor = float3(0.39, 0.50, 0.42);
	float3 color = mix(darkColor, brightColor, step(0.5, checker));
	float distanceFade = clamp(length(input.WorldXZ) * 0.025, 0.0, 1.0);
	color *= 1.0 - distanceFade * 0.35;
	return float4(color, 1.0);
}
)";

const auto ground_depth_ps_metal = R"(
#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct main0_in
{
	float2 WorldXZ [[user(locn0)]];
};

fragment float4 main0(main0_in input [[stage_in]], float4 position [[position]])
{
	return float4(position.z, 1.0, 1.0, 1.0);
}
)";

const auto ground_vs_webgpu = R"(
struct VSOutput {
	@builtin(position) position: vec4<f32>,
	@location(0) worldXZ: vec2<f32>,
};

@vertex
fn main(
	@location(0) position: vec4<f32>,
	@location(1) worldXZ: vec2<f32>
) -> VSOutput {
	var output: VSOutput;
	output.position = position;
	output.worldXZ = worldXZ;
	return output;
}
)";

const auto ground_ps_webgpu = R"(
struct PSInput {
	@location(0) worldXZ: vec2<f32>,
};

@fragment
fn main(input: PSInput) -> @location(0) vec4<f32> {
	let checker = fract((floor(input.worldXZ.x) + floor(input.worldXZ.y)) * 0.5);
	let darkColor = vec3<f32>(0.24, 0.32, 0.27);
	let brightColor = vec3<f32>(0.39, 0.50, 0.42);
	var color = mix(darkColor, brightColor, step(0.5, checker));
	let distanceFade = clamp(length(input.worldXZ) * 0.025, 0.0, 1.0);
	color *= 1.0 - distanceFade * 0.35;
	return vec4<f32>(color, 1.0);
}
)";

const auto ground_depth_ps_webgpu = R"(
struct PSInput {
	@builtin(position) position: vec4<f32>,
	@location(0) worldXZ: vec2<f32>,
};

@fragment
fn main(input: PSInput) -> @location(0) vec4<f32> {
	return vec4<f32>(input.position.z, 1.0, 1.0, 1.0);
}
)";

const GroundShaderCode dx9Code{ground_vs_dx9, nullptr, ground_ps_dx9, ground_depth_ps_dx9, "vs_3_0", "ps_3_0"};
const GroundShaderCode dx11Code{ground_vs_dx11, nullptr, ground_ps_dx11, ground_depth_ps_dx11, "vs_4_0", "ps_4_0"};
const GroundShaderCode dx12Code{ground_vs_dx12, nullptr, ground_ps_dx12, ground_depth_ps_dx12, nullptr, nullptr};
const GroundShaderCode glCode{ground_vs_gl, nullptr, ground_ps_gl, ground_depth_ps_gl, nullptr, nullptr};
const GroundShaderCode vulkanCode{ground_vs_vulkan, nullptr, ground_ps_vulkan, ground_depth_ps_vulkan, nullptr, nullptr};
const GroundShaderCode metalCode{ground_vs_metal, nullptr, ground_ps_metal, ground_depth_ps_metal, nullptr, nullptr};
const GroundShaderCode webgpuCode{ground_vs_webgpu, nullptr, ground_ps_webgpu, ground_depth_ps_webgpu, nullptr, nullptr};

} // namespace

const GroundShaderCode& GetGroundShaderCode(GroundShaderBackend backend)
{
	switch (backend)
	{
	case GroundShaderBackend::DirectX9:
		return dx9Code;
	case GroundShaderBackend::DirectX11:
		return dx11Code;
	case GroundShaderBackend::DirectX12:
		return dx12Code;
	case GroundShaderBackend::OpenGL:
		return glCode;
	case GroundShaderBackend::Vulkan:
		return vulkanCode;
	case GroundShaderBackend::Metal:
		return metalCode;
	case GroundShaderBackend::WebGPU:
		return webgpuCode;
	default:
		assert(false);
		return dx11Code;
	}
}

} // namespace ToolRuntime
} // namespace Effekseer
