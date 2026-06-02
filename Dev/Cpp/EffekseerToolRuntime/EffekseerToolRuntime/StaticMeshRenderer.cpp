#include "StaticMeshRenderer.h"

#ifdef EFFEKSEER_TOOL_RUNTIME_WITH_LLGI
#include "LLGIShaderCompiler.h"
#endif

#include <vector>

namespace
{

const auto static_mesh_vs_dx11 = R"(
cbuffer VS_ConstantBuffer : register(b0)
{
	float4x4 projectionMatrix;
	float4x4 cameraMatrix;
	float4x4 worldMatrix;
}

struct VS_Input
{
	float3 Pos : POSITION0;
	float2 UV : TEXCOORD0;
	float4 Color : NORMAL0;
	float3 Normal : NORMAL1;
};

struct VS_Output
{
	float4 Pos : SV_POSITION;
	float2 UV : TEXCOORD0;
	float4 Color : NORMAL0;
	float3 Normal : NORMAL1;
	float4 Position : NORMAL2;
};

VS_Output main(const VS_Input input)
{
	VS_Output output;
	float4 localPos = float4(input.Pos, 1.0f);
	float4 worldPos = mul(worldMatrix, localPos);
	float4 cameraPos = mul(cameraMatrix, worldPos);
	output.Pos = mul(projectionMatrix, cameraPos);
	output.UV = input.UV;
	output.Color = input.Color;
	output.Normal = input.Normal;
	output.Position = output.Pos;
	return output;
}
)";

const auto static_mesh_ps_dx11 = R"(
Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);

cbuffer PS_ConstantBuffer : register(b0)
{
	float4 isLit;
	float4 directionalLightDirection;
	float4 directionalLightColor;
	float4 ambientLightColor;
};

struct PS_Input
{
	float4 Pos : SV_POSITION;
	float2 UV : TEXCOORD0;
	float4 Color : NORMAL0;
	float3 Normal : NORMAL1;
	float4 Position : NORMAL2;
};

struct PS_OUTPUT
{
	float4 o0 : SV_TARGET0;
	float4 o1 : SV_TARGET1;
};

PS_OUTPUT main(const PS_Input Input)
{
	PS_OUTPUT output;
	output.o0 = Input.Color * g_texture.Sample(g_sampler, Input.UV);

	if(isLit.x > 0.0f)
	{
		float diffuse = max(dot(directionalLightDirection.xyz, Input.Normal.xyz), 0.0);
		output.o0.xyz = output.o0.xyz * (directionalLightColor.xyz * diffuse + ambientLightColor.xyz);
	}

	output.o1.xyzw = 1.0f;
	output.o1.x = Input.Position.z / Input.Position.w;

	return output;
}
)";

const auto static_mesh_vs_gl = R"(
#version 330
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct VS_Input
{
    vec3 Pos;
    vec2 UV;
    vec4 Color;
    vec3 Normal;
};

struct VS_Output
{
    vec4 Pos;
    vec2 UV;
    vec4 Color;
    vec3 Normal;
    vec4 Position;
};

struct VS_ConstantBuffer
{
    mat4 projectionMatrix;
    mat4 cameraMatrix;
    mat4 worldMatrix;
};

uniform VS_ConstantBuffer CBVS0;

layout(location = 0) in vec3 input_Pos;
layout(location = 1) in vec2 input_UV;
layout(location = 2) in vec4 input_Color;
layout(location = 3) in vec3 input_Normal;
out vec2 _VSPS_UV;
out vec4 _VSPS_Color;
out vec3 _VSPS_Normal;
out vec4 _VSPS_Position;

mat4 spvWorkaroundRowMajor(mat4 wrap) { return wrap; }

VS_Output _main(VS_Input _input)
{
    vec4 localPos = vec4(_input.Pos, 1.0);
    vec4 worldPos = localPos * spvWorkaroundRowMajor(CBVS0.worldMatrix);
    vec4 cameraPos = worldPos * spvWorkaroundRowMajor(CBVS0.cameraMatrix);
    VS_Output _output;
    _output.Pos = cameraPos * spvWorkaroundRowMajor(CBVS0.projectionMatrix);
    _output.UV = _input.UV;
    _output.Color = _input.Color;
    _output.Normal = _input.Normal;
    _output.Position = _output.Pos;
    return _output;
}

void main()
{
    VS_Input _input;
    _input.Pos = input_Pos;
    _input.UV = input_UV;
    _input.Color = input_Color;
    _input.Normal = input_Normal;
    VS_Output flattenTemp = _main(_input);
    gl_Position = flattenTemp.Pos;
    _VSPS_UV = flattenTemp.UV;
    _VSPS_Color = flattenTemp.Color;
    _VSPS_Normal = flattenTemp.Normal;
    _VSPS_Position = flattenTemp.Position;
}
)";

const auto static_mesh_ps_gl = R"(
#version 330
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_Input
{
    vec4 Pos;
    vec2 UV;
    vec4 Color;
    vec3 Normal;
    vec4 Position;
};

struct PS_OUTPUT
{
    vec4 o0;
    vec4 o1;
};

struct PS_ConstantBuffer
{
    vec4 isLit;
    vec4 directionalLightDirection;
    vec4 directionalLightColor;
    vec4 ambientLightColor;
};

uniform PS_ConstantBuffer CBPS0;

uniform sampler2D Sampler_g_sampler;

in vec2 _VSPS_UV;
in vec4 _VSPS_Color;
in vec3 _VSPS_Normal;
in vec4 _VSPS_Position;
layout(location = 0) out vec4 _entryPointOutput_o0;
layout(location = 1) out vec4 _entryPointOutput_o1;

PS_OUTPUT _main(PS_Input Input)
{
    PS_OUTPUT _output;
    _output.o0 = Input.Color * texture(Sampler_g_sampler, Input.UV);
    if (CBPS0.isLit.x > 0.0)
    {
        float diffuse = max(dot(CBPS0.directionalLightDirection.xyz, Input.Normal), 0.0);
        vec3 _73 = _output.o0.xyz * ((CBPS0.directionalLightColor.xyz * diffuse) + CBPS0.ambientLightColor.xyz);
        _output.o0 = vec4(_73.x, _73.y, _73.z, _output.o0.w);
    }
    _output.o1 = vec4(1.0);
    _output.o1.x = Input.Position.z / Input.Position.w;
    return _output;
}

void main()
{
    PS_Input Input;
    Input.Pos = gl_FragCoord;
    Input.UV = _VSPS_UV;
    Input.Color = _VSPS_Color;
    Input.Normal = _VSPS_Normal;
    Input.Position = _VSPS_Position;
    PS_OUTPUT flattenTemp = _main(Input);
    _entryPointOutput_o0 = flattenTemp.o0;
    _entryPointOutput_o1 = flattenTemp.o1;
}
)";

#ifdef EFFEKSEER_TOOL_RUNTIME_WITH_LLGI

const auto static_mesh_vs_dx12 = R"(
cbuffer VS_ConstantBuffer : register(b0)
{
	float4x4 projectionMatrix;
	float4x4 cameraMatrix;
	float4x4 worldMatrix;
}

struct VS_Input
{
	float3 Pos : TEXCOORD0;
	float2 UV : TEXCOORD1;
	float4 Color : TEXCOORD2;
	float3 Normal : TEXCOORD3;
};

struct VS_Output
{
	float4 Pos : SV_POSITION;
	float2 UV : TEXCOORD0;
	float4 Color : TEXCOORD1;
	float3 Normal : TEXCOORD2;
	float4 Position : TEXCOORD3;
};

VS_Output main(const VS_Input input)
{
	VS_Output output;
	float4 localPos = float4(input.Pos, 1.0f);
	float4 worldPos = mul(worldMatrix, localPos);
	float4 cameraPos = mul(cameraMatrix, worldPos);
	output.Pos = mul(projectionMatrix, cameraPos);
	output.UV = input.UV;
	output.Color = input.Color;
	output.Normal = input.Normal;
	output.Position = output.Pos;
	return output;
}
)";

const auto static_mesh_ps_dx12 = R"(
Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);

cbuffer PS_ConstantBuffer : register(b1)
{
	float4 isLit;
	float4 directionalLightDirection;
	float4 directionalLightColor;
	float4 ambientLightColor;
};

struct PS_Input
{
	float4 Pos : SV_POSITION;
	float2 UV : TEXCOORD0;
	float4 Color : TEXCOORD1;
	float3 Normal : TEXCOORD2;
	float4 Position : TEXCOORD3;
};

struct PS_OUTPUT
{
	float4 o0 : SV_TARGET0;
	float4 o1 : SV_TARGET1;
};

PS_OUTPUT main(const PS_Input Input)
{
	PS_OUTPUT output;
	output.o0 = Input.Color * g_texture.Sample(g_sampler, Input.UV);

	if(isLit.x > 0.0f)
	{
		float diffuse = max(dot(directionalLightDirection.xyz, Input.Normal.xyz), 0.0);
		output.o0.xyz = output.o0.xyz * (directionalLightColor.xyz * diffuse + ambientLightColor.xyz);
	}

	output.o1.xyzw = 1.0f;
	output.o1.x = Input.Position.z / Input.Position.w;

	return output;
}
)";

const auto static_mesh_vs_vulkan = R"(
#version 420

struct VS_Input
{
	vec3 Pos;
	vec2 UV;
	vec4 Color;
	vec3 Normal;
};

struct VS_Output
{
	vec4 Pos;
	vec2 UV;
	vec4 Color;
	vec3 Normal;
	vec4 Position;
};

layout(set = 0, binding = 0) uniform VS_ConstantBuffer
{
	mat4 projectionMatrix;
	mat4 cameraMatrix;
	mat4 worldMatrix;
} CBVS0;

layout(location = 0) in vec3 input_Pos;
layout(location = 1) in vec2 input_UV;
layout(location = 2) in vec4 input_Color;
layout(location = 3) in vec3 input_Normal;
layout(location = 0) out vec2 output_UV;
layout(location = 1) out vec4 output_Color;
layout(location = 2) out vec3 output_Normal;
layout(location = 3) out vec4 output_Position;

VS_Output _main(VS_Input input)
{
	vec4 localPos = vec4(input.Pos, 1.0);
	vec4 worldPos = localPos * CBVS0.worldMatrix;
	vec4 cameraPos = worldPos * CBVS0.cameraMatrix;
	VS_Output output;
	output.Pos = cameraPos * CBVS0.projectionMatrix;
	output.UV = input.UV;
	output.Color = input.Color;
	output.Normal = input.Normal;
	output.Position = output.Pos;
	return output;
}

void main()
{
	VS_Input input;
	input.Pos = input_Pos;
	input.UV = input_UV;
	input.Color = input_Color;
	input.Normal = input_Normal;
	VS_Output output = _main(input);
	vec4 position = output.Pos;
	position.y = -position.y;
	gl_Position = position;
	output_UV = output.UV;
	output_Color = output.Color;
	output_Normal = output.Normal;
	output_Position = output.Position;
}
)";

const auto static_mesh_ps_vulkan = R"(
#version 420

layout(set = 0, binding = 1) uniform PS_ConstantBuffer
{
	vec4 isLit;
	vec4 directionalLightDirection;
	vec4 directionalLightColor;
	vec4 ambientLightColor;
} CBPS0;

layout(set = 1, binding = 0) uniform sampler2D Sampler_g_sampler;

layout(location = 0) in vec2 input_UV;
layout(location = 1) in vec4 input_Color;
layout(location = 2) in vec3 input_Normal;
layout(location = 3) in vec4 input_Position;
layout(location = 0) out vec4 output_Color;
layout(location = 1) out vec4 output_Depth;

void main()
{
	vec4 color = input_Color * texture(Sampler_g_sampler, input_UV);
	if (CBPS0.isLit.x > 0.0)
	{
		float diffuse = max(dot(CBPS0.directionalLightDirection.xyz, input_Normal), 0.0);
		color.xyz = color.xyz * ((CBPS0.directionalLightColor.xyz * diffuse) + CBPS0.ambientLightColor.xyz);
	}
	output_Color = color;
	output_Depth = vec4(input_Position.z / input_Position.w, 1.0, 1.0, 1.0);
}
)";

const auto static_mesh_vs_metal = R"(
#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct VS_ConstantBuffer
{
	float4x4 projectionMatrix;
	float4x4 cameraMatrix;
	float4x4 worldMatrix;
};

struct main0_in
{
	float3 Pos [[attribute(0)]];
	float2 UV [[attribute(1)]];
	float4 Color [[attribute(2)]];
	float3 Normal [[attribute(3)]];
};

struct main0_out
{
	float2 UV [[user(locn0)]];
	float4 Color [[user(locn1)]];
	float3 Normal [[user(locn2)]];
	float4 ClipPosition [[user(locn3)]];
	float4 Position [[position]];
};

vertex main0_out main0(main0_in input [[stage_in]], constant VS_ConstantBuffer& CBVS0 [[buffer(0)]])
{
	main0_out output = {};
	float4 localPos = float4(input.Pos, 1.0);
	float4 worldPos = localPos * CBVS0.worldMatrix;
	float4 cameraPos = worldPos * CBVS0.cameraMatrix;
	output.Position = cameraPos * CBVS0.projectionMatrix;
	output.UV = input.UV;
	output.Color = input.Color;
	output.Normal = input.Normal;
	output.ClipPosition = output.Position;
	return output;
}
)";

const auto static_mesh_ps_metal = R"(
#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct PS_ConstantBuffer
{
	float4 isLit;
	float4 directionalLightDirection;
	float4 directionalLightColor;
	float4 ambientLightColor;
};

struct main0_in
{
	float2 UV [[user(locn0)]];
	float4 Color [[user(locn1)]];
	float3 Normal [[user(locn2)]];
	float4 ClipPosition [[user(locn3)]];
};

struct main0_out
{
	float4 Color [[color(0)]];
	float4 Depth [[color(1)]];
};

fragment main0_out main0(
	main0_in input [[stage_in]],
	constant PS_ConstantBuffer& CBPS0 [[buffer(1)]],
	texture2d<float> g_texture [[texture(0)]],
	sampler g_sampler [[sampler(0)]])
{
	main0_out output = {};
	float4 color = input.Color * g_texture.sample(g_sampler, input.UV);
	if (CBPS0.isLit.x > 0.0)
	{
		float diffuse = max(dot(CBPS0.directionalLightDirection.xyz, input.Normal), 0.0);
		color.xyz = color.xyz * ((CBPS0.directionalLightColor.xyz * diffuse) + CBPS0.ambientLightColor.xyz);
	}
	output.Color = color;
	output.Depth = float4(input.ClipPosition.z / input.ClipPosition.w, 1.0, 1.0, 1.0);
	return output;
}
)";

const auto static_mesh_vs_webgpu = R"(
struct VSConstantBuffer {
	projectionMatrix: mat4x4<f32>,
	cameraMatrix: mat4x4<f32>,
	worldMatrix: mat4x4<f32>,
};

@group(0) @binding(0) var<uniform> CBVS0: VSConstantBuffer;

struct VSInput {
	@location(0) pos: vec3<f32>,
	@location(1) uv: vec2<f32>,
	@location(2) color: vec4<f32>,
	@location(3) normal: vec3<f32>,
};

struct VSOutput {
	@builtin(position) position: vec4<f32>,
	@location(0) uv: vec2<f32>,
	@location(1) color: vec4<f32>,
	@location(2) normal: vec3<f32>,
	@location(3) clipPosition: vec4<f32>,
};

@vertex
fn main(input: VSInput) -> VSOutput {
	var output: VSOutput;
	let localPos = vec4<f32>(input.pos, 1.0);
	let worldPos = localPos * CBVS0.worldMatrix;
	let cameraPos = worldPos * CBVS0.cameraMatrix;
	output.position = cameraPos * CBVS0.projectionMatrix;
	output.uv = input.uv;
	output.color = input.color;
	output.normal = input.normal;
	output.clipPosition = output.position;
	return output;
}
)";

const auto static_mesh_ps_webgpu = R"(
struct PSConstantBuffer {
	isLit: vec4<f32>,
	directionalLightDirection: vec4<f32>,
	directionalLightColor: vec4<f32>,
	ambientLightColor: vec4<f32>,
};

@group(0) @binding(1) var<uniform> CBPS0: PSConstantBuffer;
@group(1) @binding(0) var g_texture: texture_2d<f32>;
@group(2) @binding(0) var g_sampler: sampler;

struct PSInput {
	@builtin(position) position: vec4<f32>,
	@location(0) uv: vec2<f32>,
	@location(1) color: vec4<f32>,
	@location(2) normal: vec3<f32>,
	@location(3) clipPosition: vec4<f32>,
};

struct PSOutput {
	@location(0) color: vec4<f32>,
	@location(1) depth: vec4<f32>,
};

@fragment
fn main(input: PSInput) -> PSOutput {
	var output: PSOutput;
	var color = input.color * textureSample(g_texture, g_sampler, input.uv);
	if (CBPS0.isLit.x > 0.0) {
		let diffuse = max(dot(CBPS0.directionalLightDirection.xyz, input.normal), 0.0);
		color = vec4<f32>(color.xyz * ((CBPS0.directionalLightColor.xyz * diffuse) + CBPS0.ambientLightColor.xyz), color.w);
	}
	output.color = color;
	output.depth = vec4<f32>(input.clipPosition.z / input.clipPosition.w, 1.0, 1.0, 1.0);
	return output;
}
)";

bool GetStaticMeshShaderCode(LLGI::DeviceType deviceType, const char*& vertexCode, const char*& pixelCode)
{
	switch (deviceType)
	{
	case LLGI::DeviceType::DirectX12:
		vertexCode = static_mesh_vs_dx12;
		pixelCode = static_mesh_ps_dx12;
		return true;
	case LLGI::DeviceType::Vulkan:
		vertexCode = static_mesh_vs_vulkan;
		pixelCode = static_mesh_ps_vulkan;
		return true;
	case LLGI::DeviceType::Metal:
		vertexCode = static_mesh_vs_metal;
		pixelCode = static_mesh_ps_metal;
		return true;
	case LLGI::DeviceType::WebGPU:
		vertexCode = static_mesh_vs_webgpu;
		pixelCode = static_mesh_ps_webgpu;
		return true;
	default:
		return false;
	}
}

Effekseer::Backend::ShaderRef CreateStaticMeshShaderForLLGI(
	Effekseer::RefPtr<Effekseer::Backend::GraphicsDevice> graphicsDevice,
	LLGI::DeviceType deviceType)
{
	const char* vertexCode = nullptr;
	const char* pixelCode = nullptr;
	if (!GetStaticMeshShaderCode(deviceType, vertexCode, pixelCode))
	{
		Effekseer::Log(Effekseer::LogType::Error, "StaticMeshRenderer doesn't support this LLGI backend.");
		return nullptr;
	}

	std::string message;
	auto shader = Effekseer::ToolRuntime::CreateBackendShaderFromLLGICode(
		graphicsDevice,
		deviceType,
		vertexCode,
		pixelCode,
		message,
		"StaticMeshRenderer");
	if (shader == nullptr)
	{
		Effekseer::Log(Effekseer::LogType::Error, "Failed to compile StaticMeshRenderer shaders.\n" + message);
		return nullptr;
	}
	return shader;
}

#endif

} // namespace

namespace Effekseer
{
namespace ToolRuntime
{

std::shared_ptr<StaticMesh> StaticMesh::Create(Effekseer::RefPtr<Backend::GraphicsDevice> graphicsDevice, Effekseer::CustomVector<StaticMeshVertex> vertexes, Effekseer::CustomVector<int32_t> indexes, bool isDyanmic)
{
	auto vb = graphicsDevice->CreateVertexBuffer(static_cast<int32_t>(sizeof(StaticMeshVertex) * vertexes.size()), vertexes.data(), isDyanmic);
	auto ib = graphicsDevice->CreateIndexBuffer(static_cast<int32_t>(indexes.size()), indexes.data(), Effekseer::Backend::IndexBufferStrideType::Stride4);

	if (vb == nullptr || ib == nullptr)
	{
		return nullptr;
	}

	auto ret = std::make_shared<StaticMesh>();
	ret->vb_ = vb;
	ret->ib_ = ib;
	ret->indexCount_ = ib->GetElementCount();
	return ret;
}

StaticMeshRenderer::StaticMeshRenderer()
{
}

std::shared_ptr<StaticMeshRenderer> StaticMeshRenderer::Create(RefPtr<Backend::GraphicsDevice> graphicsDevice)
{
	Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement> uniformLayoutElements;
	uniformLayoutElements.resize(7);
	uniformLayoutElements[0].Name = "CBVS0.projectionMatrix";
	uniformLayoutElements[0].Offset = 0;
	uniformLayoutElements[0].Stage = Effekseer::Backend::ShaderStageType::Vertex;
	uniformLayoutElements[0].Type = Effekseer::Backend::UniformBufferLayoutElementType::Matrix44;
	uniformLayoutElements[1].Name = "CBVS0.cameraMatrix";
	uniformLayoutElements[1].Offset = sizeof(float) * 16;
	uniformLayoutElements[1].Stage = Effekseer::Backend::ShaderStageType::Vertex;
	uniformLayoutElements[1].Type = Effekseer::Backend::UniformBufferLayoutElementType::Matrix44;
	uniformLayoutElements[2].Name = "CBVS0.worldMatrix";
	uniformLayoutElements[2].Offset = sizeof(float) * 32;
	uniformLayoutElements[2].Stage = Effekseer::Backend::ShaderStageType::Vertex;
	uniformLayoutElements[2].Type = Effekseer::Backend::UniformBufferLayoutElementType::Matrix44;

	uniformLayoutElements[3].Name = "CBPS0.isLit";
	uniformLayoutElements[3].Offset = 0;
	uniformLayoutElements[3].Stage = Effekseer::Backend::ShaderStageType::Pixel;
	uniformLayoutElements[3].Type = Effekseer::Backend::UniformBufferLayoutElementType::Vector4;
	uniformLayoutElements[4].Name = "CBPS0.directionalLightDirection";
	uniformLayoutElements[4].Offset = sizeof(float) * 4;
	uniformLayoutElements[4].Stage = Effekseer::Backend::ShaderStageType::Pixel;
	uniformLayoutElements[4].Type = Effekseer::Backend::UniformBufferLayoutElementType::Vector4;
	uniformLayoutElements[5].Name = "CBPS0.directionalLightColor";
	uniformLayoutElements[5].Offset = sizeof(float) * 8;
	uniformLayoutElements[5].Stage = Effekseer::Backend::ShaderStageType::Pixel;
	uniformLayoutElements[5].Type = Effekseer::Backend::UniformBufferLayoutElementType::Vector4;
	uniformLayoutElements[6].Name = "CBPS0.ambientLightColor";
	uniformLayoutElements[6].Offset = sizeof(float) * 12;
	uniformLayoutElements[6].Stage = Effekseer::Backend::ShaderStageType::Pixel;
	uniformLayoutElements[6].Type = Effekseer::Backend::UniformBufferLayoutElementType::Vector4;

	auto vcb = graphicsDevice->CreateUniformBuffer(sizeof(UniformBufferVS), nullptr);
	auto pcb = graphicsDevice->CreateUniformBuffer(sizeof(UniformBufferPS), nullptr);
	auto uniformLayout = Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(CustomVector<Effekseer::CustomString<char>>{"Sampler_g_sampler"}, std::move(uniformLayoutElements));

	Effekseer::Backend::ShaderRef shader;

#ifdef EFFEKSEER_TOOL_RUNTIME_WITH_LLGI
	LLGI::DeviceType deviceType = LLGI::DeviceType::Default;
	if (TryGetLLGIDeviceType(graphicsDevice, deviceType))
	{
		shader = CreateStaticMeshShaderForLLGI(graphicsDevice, deviceType);
	}
	else
#endif
		if (graphicsDevice->GetDeviceName() == "DirectX11")
	{
		shader = graphicsDevice->CreateShaderFromCodes({{static_mesh_vs_dx11}}, {{static_mesh_ps_dx11}}, uniformLayout);
	}
	else
	{
		shader = graphicsDevice->CreateShaderFromCodes({{static_mesh_vs_gl}}, {{static_mesh_ps_gl}}, uniformLayout);
	}

	if (vcb == nullptr || pcb == nullptr || shader == nullptr)
	{
		return nullptr;
	}

	std::vector<Effekseer::Backend::VertexLayoutElement> vertexLayoutElements;
	vertexLayoutElements.resize(4);
	vertexLayoutElements[0].Format = Effekseer::Backend::VertexLayoutFormat::R32G32B32_FLOAT;
	vertexLayoutElements[0].Name = "input_Pos";
	vertexLayoutElements[0].SemanticIndex = 0;
	vertexLayoutElements[0].SemanticName = "POSITION";
	vertexLayoutElements[1].Format = Effekseer::Backend::VertexLayoutFormat::R32G32_FLOAT;
	vertexLayoutElements[1].Name = "input_UV";
	vertexLayoutElements[1].SemanticIndex = 0;
	vertexLayoutElements[1].SemanticName = "TEXCOORD";
	vertexLayoutElements[2].Format = Effekseer::Backend::VertexLayoutFormat::R8G8B8A8_UNORM;
	vertexLayoutElements[2].Name = "input_Color";
	vertexLayoutElements[2].SemanticIndex = 0;
	vertexLayoutElements[2].SemanticName = "NORMAL";
	vertexLayoutElements[3].Format = Effekseer::Backend::VertexLayoutFormat::R32G32B32_FLOAT;
	vertexLayoutElements[3].Name = "input_Normal";
	vertexLayoutElements[3].SemanticIndex = 1;
	vertexLayoutElements[3].SemanticName = "NORMAL";

	auto vertexLayout = graphicsDevice->CreateVertexLayout(vertexLayoutElements.data(), static_cast<int32_t>(vertexLayoutElements.size()));
	if (vertexLayout == nullptr)
	{
		return nullptr;
	}

	Effekseer::Backend::PipelineStateParameter pipParam;
	pipParam.FrameBufferPtr = nullptr;
	pipParam.VertexLayoutPtr = vertexLayout;
	pipParam.ShaderPtr = shader;
	pipParam.IsDepthTestEnabled = true;
	pipParam.IsDepthWriteEnabled = true;
	pipParam.IsBlendEnabled = false;

	auto pip = graphicsDevice->CreatePipelineState(pipParam);
	if (pip == nullptr)
	{
		return nullptr;
	}

	auto ret = std::make_shared<StaticMeshRenderer>();

	ret->graphicsDevice_ = graphicsDevice;
	ret->pip_ = pip;
	ret->shader_ = shader;
	ret->uniformBufferVS_ = vcb;
	ret->uniformBufferPS_ = pcb;
	ret->vertexLayout_ = vertexLayout;

	Backend::TextureParameter texParam;
	const int32_t textureSize = 16;
	texParam.Format = Backend::TextureFormatType::R8G8B8A8_UNORM;
	texParam.Size = {textureSize, textureSize};

	CustomVector<uint8_t> initialTexData;
	initialTexData.resize(textureSize * textureSize * 4);
	for (auto& v : initialTexData)
	{
		v = 255;
	}

	ret->dummyTexture_ = graphicsDevice->CreateTexture(texParam, initialTexData);

	return ret;
}

void StaticMeshRenderer::Render(const RendererParameter& rendererParameter)
{
	if (staticMesh_ == nullptr)
	{
		return;
	}

	UniformBufferVS uvs{};
	UniformBufferPS ups{};

	uvs.projectionMatrix = rendererParameter.ProjectionMatrix;
	uvs.cameraMatrix = rendererParameter.CameraMatrix;
	uvs.worldMatrix = rendererParameter.WorldMatrix;

	const auto deviceName = graphicsDevice_->GetDeviceName();
	if (deviceName == "DirectX11" || deviceName == "DirectX12")
	{
	}
	else
	{
		uvs.cameraMatrix.Transpose();
		uvs.projectionMatrix.Transpose();
		uvs.worldMatrix.Transpose();
	}

	ups.isLit[0] = staticMesh_->IsLit ? 1.0f : 0.0f;
	ups.directionalLightDirection = rendererParameter.DirectionalLightDirection;
	ups.directionalLightColor = rendererParameter.DirectionalLightColor;
	ups.ambientLightColor = rendererParameter.AmbientLightColor;

	graphicsDevice_->UpdateUniformBuffer(uniformBufferVS_, sizeof(UniformBufferVS), 0, &uvs);
	graphicsDevice_->UpdateUniformBuffer(uniformBufferPS_, sizeof(UniformBufferPS), 0, &ups);

	Effekseer::Backend::DrawParameter drawParam;

	drawParam.SetTexture(0, staticMesh_->Texture != nullptr ? staticMesh_->Texture : dummyTexture_, Backend::TextureWrapType::Repeat, Backend::TextureSamplingType::Linear);

	drawParam.VertexBufferPtr = staticMesh_->GetVertexBuffer();
	drawParam.IndexBufferPtr = staticMesh_->GetIndexBuffer();
	drawParam.PipelineStatePtr = pip_;
	drawParam.VertexStride = sizeof(StaticMeshVertex);
	drawParam.VertexUniformBufferPtrs[0] = uniformBufferVS_;
#ifdef EFFEKSEER_TOOL_RUNTIME_WITH_LLGI
	LLGI::DeviceType deviceType = LLGI::DeviceType::Default;
	if (TryGetLLGIDeviceType(graphicsDevice_, deviceType))
	{
		drawParam.VertexUniformBufferPtrs[1] = uniformBufferPS_;
	}
	else
#endif
	{
		drawParam.PixelUniformBufferPtrs[0] = uniformBufferPS_;
	}
	drawParam.PrimitiveCount = staticMesh_->GetIndexCount() / 3;
	drawParam.InstanceCount = 1;
	graphicsDevice_->Draw(drawParam);
}

void StaticMeshRenderer::SetStaticMesh(const std::shared_ptr<StaticMesh>& mesh)
{
	staticMesh_ = mesh;
}

std::shared_ptr<StaticMesh>& StaticMeshRenderer::GetStaticMesh()
{
	return staticMesh_;
}

} // namespace ToolRuntime
} // namespace Effekseer
