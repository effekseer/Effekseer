#include "LineRenderer.h"

#ifdef EFFEKSEER_TOOL_RUNTIME_WITH_LLGI
#include <EffekseerToolRuntime/LLGIShaderCompiler.h>
#endif

namespace
{
const auto line_vs_dx11 = R"(
float4x4 mCamera : register(c0);
float4x4 mProj : register(c4);

struct VS_Input
{
	float3 Pos : POSITION0;
	float4 Color : NORMAL0;
	float2 UV : TEXCOORD0;
};

struct VS_Output
{
	float4 Pos : SV_POSITION;
	float4 Color : COLOR;
	float2 UV : TEXCOORD0;
	float4 Position : TEXCOORD1;
};

VS_Output main(const VS_Input Input)
{
	VS_Output Output = (VS_Output)0;
	float4 pos4 = {Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0};
	float4 cameraPos = mul(mCamera, pos4);
	cameraPos = cameraPos / cameraPos.w;
	Output.Position = mul(mProj, cameraPos);
	Output.Pos = Output.Position;
	Output.UV = Input.UV;
	Output.Color = Input.Color;
	return Output;
}
)";

const auto line_ps_dx11 = R"(
struct PS_Input
{
	float4 Pos : SV_POSITION;
	float4 Color : COLOR;
	float2 UV : TEXCOORD0;
	float4 Position : TEXCOORD1;
};

struct PS_Output
{
	float4 o0 : SV_TARGET0;
};

PS_Output main(const PS_Input Input)
{
	PS_Output output;
	output.o0 = Input.Color;
	if (output.o0.a == 0.0)
	{
		discard;
	}
	return output;
}
)";

const auto line_vs_gl = R"(
#version 330
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct VS_Input
{
    vec3 Pos;
    vec4 Color;
    vec2 UV;
};

struct VS_Output
{
    vec4 Pos;
    vec4 Color;
    vec2 UV;
    vec4 Position;
};

struct _Global
{
    mat4 mCamera;
    mat4 mProj;
};

uniform _Global CBVS0;

layout(location = 0) in vec3 Input_Pos;
layout(location = 1) in vec4 Input_Color;
layout(location = 2) in vec2 Input_UV;
out vec4 _VSPS_Color;
out vec2 _VSPS_UV;
out vec4 _VSPS_Position;

mat4 spvWorkaroundRowMajor(mat4 wrap) { return wrap; }

VS_Output _main(VS_Input Input)
{
    VS_Output Output = VS_Output(vec4(0.0), vec4(0.0), vec2(0.0), vec4(0.0));
    vec4 pos4 = vec4(Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0);
    vec4 cameraPos = pos4 * spvWorkaroundRowMajor(CBVS0.mCamera);
    cameraPos /= vec4(cameraPos.w);
    Output.Position = cameraPos * spvWorkaroundRowMajor(CBVS0.mProj);
    Output.Pos = Output.Position;
    Output.UV = Input.UV;
    Output.Color = Input.Color;
    return Output;
}

void main()
{
    VS_Input Input;
    Input.Pos = Input_Pos;
    Input.Color = Input_Color;
    Input.UV = Input_UV;
    VS_Output flattenTemp = _main(Input);
    gl_Position = flattenTemp.Pos;
    _VSPS_Color = flattenTemp.Color;
    _VSPS_UV = flattenTemp.UV;
    _VSPS_Position = flattenTemp.Position;
}
)";

const auto line_ps_gl = R"(
#version 330
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_Input
{
    vec4 Pos;
    vec4 Color;
    vec2 UV;
    vec4 Position;
};

struct PS_Output
{
    vec4 o0;
};

in vec4 _VSPS_Color;
in vec2 _VSPS_UV;
in vec4 _VSPS_Position;
layout(location = 0) out vec4 _entryPointOutput_o0;

PS_Output _main(PS_Input Input)
{
    PS_Output _output;
    _output.o0 = Input.Color;
    if (_output.o0.w == 0.0)
    {
        discard;
    }
    return _output;
}

void main()
{
    PS_Input Input;
    Input.Pos = gl_FragCoord;
    Input.Color = _VSPS_Color;
    Input.UV = _VSPS_UV;
    Input.Position = _VSPS_Position;
    PS_Output _67 = _main(Input);
    PS_Output flattenTemp = _67;
    _entryPointOutput_o0 = flattenTemp.o0;
}
)";

#ifdef EFFEKSEER_TOOL_RUNTIME_WITH_LLGI

const auto line_vs_dx12 = R"(
cbuffer VS_ConstantBuffer : register(b0)
{
	float4x4 mCamera;
	float4x4 mProj;
};

struct VS_Input
{
	float3 Pos : TEXCOORD0;
	float4 Color : TEXCOORD1;
	float2 UV : TEXCOORD2;
};

struct VS_Output
{
	float4 Pos : SV_POSITION;
	float4 Color : TEXCOORD0;
	float2 UV : TEXCOORD1;
	float4 Position : TEXCOORD2;
};

VS_Output main(const VS_Input input)
{
	VS_Output output;
	float4 pos4 = float4(input.Pos, 1.0f);
	float4 cameraPos = mul(mCamera, pos4);
	cameraPos /= cameraPos.w;
	output.Position = mul(mProj, cameraPos);
	output.Pos = output.Position;
	output.UV = input.UV;
	output.Color = input.Color;
	return output;
}
)";

const auto line_ps_dx12 = R"(
struct PS_Input
{
	float4 Pos : SV_POSITION;
	float4 Color : TEXCOORD0;
	float2 UV : TEXCOORD1;
	float4 Position : TEXCOORD2;
};

struct PS_Output
{
	float4 o0 : SV_TARGET0;
};

PS_Output main(const PS_Input input)
{
	PS_Output output;
	output.o0 = input.Color;
	if (output.o0.w == 0.0f)
	{
		discard;
	}
	return output;
}
)";

const auto line_vs_vulkan = R"(
#version 420

layout(set = 0, binding = 0) uniform VS_ConstantBuffer
{
	mat4 mCamera;
	mat4 mProj;
} CBVS0;

layout(location = 0) in vec3 input_Pos;
layout(location = 1) in vec4 input_Color;
layout(location = 2) in vec2 input_UV;
layout(location = 0) out vec4 output_Color;
layout(location = 1) out vec2 output_UV;
layout(location = 2) out vec4 output_Position;

void main()
{
	vec4 pos4 = vec4(input_Pos, 1.0);
	vec4 cameraPos = pos4 * CBVS0.mCamera;
	cameraPos /= cameraPos.w;
	output_Position = cameraPos * CBVS0.mProj;
	output_Position.y = -output_Position.y;
	gl_Position = output_Position;
	output_UV = input_UV;
	output_Color = input_Color;
}
)";

const auto line_ps_vulkan = R"(
#version 420

layout(location = 0) in vec4 input_Color;
layout(location = 1) in vec2 input_UV;
layout(location = 2) in vec4 input_Position;
layout(location = 0) out vec4 output_Color;

void main()
{
	output_Color = input_Color;
	if (output_Color.w == 0.0)
	{
		discard;
	}
}
)";

const auto line_vs_metal = R"(
#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct VS_ConstantBuffer
{
	float4x4 mCamera;
	float4x4 mProj;
};

struct main0_in
{
	float3 Pos [[attribute(0)]];
	float4 Color [[attribute(1)]];
	float2 UV [[attribute(2)]];
};

struct main0_out
{
	float4 Color [[user(locn0)]];
	float2 UV [[user(locn1)]];
	float4 ClipPosition [[user(locn2)]];
	float4 Position [[position]];
};

vertex main0_out main0(main0_in input [[stage_in]], constant VS_ConstantBuffer& CBVS0 [[buffer(0)]])
{
	main0_out output = {};
	float4 pos4 = float4(input.Pos, 1.0);
	float4 cameraPos = pos4 * CBVS0.mCamera;
	cameraPos /= cameraPos.w;
	output.Position = cameraPos * CBVS0.mProj;
	output.ClipPosition = output.Position;
	output.UV = input.UV;
	output.Color = input.Color;
	return output;
}
)";

const auto line_ps_metal = R"(
#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct main0_in
{
	float4 Color [[user(locn0)]];
	float2 UV [[user(locn1)]];
	float4 ClipPosition [[user(locn2)]];
};

struct main0_out
{
	float4 Color [[color(0)]];
};

fragment main0_out main0(main0_in input [[stage_in]])
{
	main0_out output = {};
	output.Color = input.Color;
	if (output.Color.w == 0.0)
	{
		discard_fragment();
	}
	return output;
}
)";

const auto line_vs_webgpu = R"(
struct VSConstantBuffer {
	mCamera: mat4x4<f32>,
	mProj: mat4x4<f32>,
};

@group(0) @binding(0) var<uniform> CBVS0: VSConstantBuffer;

struct VSInput {
	@location(0) pos: vec3<f32>,
	@location(1) color: vec4<f32>,
	@location(2) uv: vec2<f32>,
};

struct VSOutput {
	@builtin(position) position: vec4<f32>,
	@location(0) color: vec4<f32>,
	@location(1) uv: vec2<f32>,
	@location(2) clipPosition: vec4<f32>,
};

@vertex
fn main(input: VSInput) -> VSOutput {
	var output: VSOutput;
	let pos4 = vec4<f32>(input.pos, 1.0);
	var cameraPos = pos4 * CBVS0.mCamera;
	cameraPos = cameraPos / cameraPos.w;
	output.position = cameraPos * CBVS0.mProj;
	output.clipPosition = output.position;
	output.uv = input.uv;
	output.color = input.color;
	return output;
}
)";

const auto line_ps_webgpu = R"(
struct PSInput {
	@builtin(position) position: vec4<f32>,
	@location(0) color: vec4<f32>,
	@location(1) uv: vec2<f32>,
	@location(2) clipPosition: vec4<f32>,
};

struct PSOutput {
	@location(0) color: vec4<f32>,
};

@fragment
fn main(input: PSInput) -> PSOutput {
	var output: PSOutput;
	output.color = input.color;
	if (output.color.w == 0.0) {
		discard;
	}
	return output;
}
)";

bool GetLineShaderCode(LLGI::DeviceType deviceType, const char*& vertexCode, const char*& pixelCode)
{
	switch (deviceType)
	{
	case LLGI::DeviceType::DirectX12:
		vertexCode = line_vs_dx12;
		pixelCode = line_ps_dx12;
		return true;
	case LLGI::DeviceType::Vulkan:
		vertexCode = line_vs_vulkan;
		pixelCode = line_ps_vulkan;
		return true;
	case LLGI::DeviceType::Metal:
		vertexCode = line_vs_metal;
		pixelCode = line_ps_metal;
		return true;
	case LLGI::DeviceType::WebGPU:
		vertexCode = line_vs_webgpu;
		pixelCode = line_ps_webgpu;
		return true;
	default:
		return false;
	}
}

Effekseer::Backend::ShaderRef CreateLineShaderForLLGI(
	Effekseer::RefPtr<Effekseer::Backend::GraphicsDevice> graphicsDevice,
	LLGI::DeviceType deviceType)
{
	const char* vertexCode = nullptr;
	const char* pixelCode = nullptr;
	if (!GetLineShaderCode(deviceType, vertexCode, pixelCode))
	{
		Effekseer::Log(Effekseer::LogType::Error, "LineRenderer doesn't support this LLGI backend.");
		return nullptr;
	}

	std::string message;
	auto shader = Effekseer::ToolRuntime::CreateBackendShaderFromLLGICode(
		graphicsDevice,
		deviceType,
		vertexCode,
		pixelCode,
		message,
		"LineRenderer");
	if (shader == nullptr)
	{
		Effekseer::Log(Effekseer::LogType::Error, "Failed to compile LineRenderer shaders.\n" + message);
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

LineRenderer::LineRenderer(const Backend::GraphicsDeviceRef graphicsDevice, bool isDepthTestEnabled)
{
	auto vb = graphicsDevice->CreateVertexBuffer(static_cast<int32_t>(sizeof(Vertex) * 2 * 1024), nullptr, true);

	std::array<int32_t, 2 * 1024> indexes;

	for (int32_t i = 0; i < 1024; i++)
	{
		indexes[0 + i * 2] = 0 + i * 2;
		indexes[1 + i * 2] = 1 + i * 2;
	}

	auto ib = graphicsDevice->CreateIndexBuffer(static_cast<int32_t>(2 * 1024), indexes.data(), Effekseer::Backend::IndexBufferStrideType::Stride4);

	// shader
	using namespace Effekseer::Backend;
	Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement> uniformLayoutElm;
	uniformLayoutElm.emplace_back(UniformLayoutElement{ShaderStageType::Vertex, "CBVS0.mCamera", UniformBufferLayoutElementType::Matrix44, 1, 0});
	uniformLayoutElm.emplace_back(UniformLayoutElement{ShaderStageType::Vertex, "CBVS0.mProj", UniformBufferLayoutElementType::Matrix44, 1, sizeof(Effekseer::Matrix44)});
	auto uniformLayout = Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(Effekseer::CustomVector<Effekseer::CustomString<char>>{}, uniformLayoutElm);

	Effekseer::Backend::ShaderRef shader;

#ifdef EFFEKSEER_TOOL_RUNTIME_WITH_LLGI
	LLGI::DeviceType deviceType = LLGI::DeviceType::Default;
	if (TryGetLLGIDeviceType(graphicsDevice, deviceType))
	{
		shader = CreateLineShaderForLLGI(graphicsDevice, deviceType);
	}
	else
#endif
	if (graphicsDevice->GetDeviceName() == "DirectX11")
	{
		shader = graphicsDevice->CreateShaderFromCodes({{line_vs_dx11}}, {{line_ps_dx11}}, uniformLayout);
	}
	else
	{
		shader = graphicsDevice->CreateShaderFromCodes({{line_vs_gl}}, {{line_ps_gl}}, uniformLayout);
	}

	if (vb == nullptr || ib == nullptr || shader == nullptr)
	{
		return;
	}

	std::vector<Effekseer::Backend::VertexLayoutElement> vertexLayoutElements;
	vertexLayoutElements.resize(3);
	vertexLayoutElements[0].Format = Effekseer::Backend::VertexLayoutFormat::R32G32B32_FLOAT;
	vertexLayoutElements[0].Name = "Input_Pos";
	vertexLayoutElements[0].SemanticIndex = 0;
	vertexLayoutElements[0].SemanticName = "POSITION";
	vertexLayoutElements[1].Format = Effekseer::Backend::VertexLayoutFormat::R8G8B8A8_UNORM;
	vertexLayoutElements[1].Name = "Input_Color";
	vertexLayoutElements[1].SemanticIndex = 0;
	vertexLayoutElements[1].SemanticName = "NORMAL";
	vertexLayoutElements[2].Format = Effekseer::Backend::VertexLayoutFormat::R32G32_FLOAT;
	vertexLayoutElements[2].Name = "Input_UV";
	vertexLayoutElements[2].SemanticIndex = 0;
	vertexLayoutElements[2].SemanticName = "TEXCOORD";

	auto vertexLayout = graphicsDevice->CreateVertexLayout(vertexLayoutElements.data(), static_cast<int32_t>(vertexLayoutElements.size()));
	if (vertexLayout == nullptr)
	{
		return;
	}

	Effekseer::Backend::PipelineStateParameter pipParam;

	// OpenGL doesn't require it
	pipParam.FrameBufferPtr = nullptr;
	pipParam.VertexLayoutPtr = vertexLayout;
	pipParam.ShaderPtr = shader;
	pipParam.IsDepthTestEnabled = isDepthTestEnabled;
	pipParam.IsDepthWriteEnabled = false;
	pipParam.IsBlendEnabled = true;
	pipParam.IsMSAAEnabled = true;
	pipParam.Topology = Effekseer::Backend::TopologyType::Line;

	auto pip = graphicsDevice->CreatePipelineState(pipParam);

	uniformBufferVS_ = graphicsDevice->CreateUniformBuffer(sizeof(Effekseer::Matrix44) * 2, nullptr);
	if (pip == nullptr || uniformBufferVS_ == nullptr)
	{
		return;
	}

	vb_ = vb;
	ib_ = ib;
	graphicsDevice_ = graphicsDevice;
	pip_ = pip;
	shader_ = shader;
	vertexLayout_ = vertexLayout;
}

bool LineRenderer::GetIsValid() const
{
	return graphicsDevice_ != nullptr && vb_ != nullptr && ib_ != nullptr && uniformBufferVS_ != nullptr && shader_ != nullptr && pip_ != nullptr &&
		   vertexLayout_ != nullptr;
}

void LineRenderer::DrawLine(const Effekseer::Vector3D& p1, const Effekseer::Vector3D& p2, const Effekseer::Color& c)
{
	if (!GetIsValid())
	{
		return;
	}

	Vertex v0;
	v0.Pos = p1;
	v0.VColor = c;
	v0.UV = {};

	Vertex v1;
	v1.Pos = p2;
	v1.VColor = c;
	v1.UV = {};

	vertexies_.push_back(v0);
	vertexies_.push_back(v1);
}

void LineRenderer::Render(const Effekseer::Matrix44& cameraMatrix, const Effekseer::Matrix44& projectionMatrix)
{
	if (!GetIsValid() || vertexies_.empty())
	{
		return;
	}

	vb_->UpdateData(vertexies_.data(), sizeof(Vertex) * vertexies_.size(), 0);

	Effekseer::Matrix44 constantVSBuffer[2];
	constantVSBuffer[0] = cameraMatrix;
	constantVSBuffer[1] = projectionMatrix;

	const auto deviceName = graphicsDevice_->GetDeviceName();
	if (deviceName == "DirectX11" || deviceName == "DirectX12")
	{
	}
	else
	{
		constantVSBuffer[0].Transpose();
		constantVSBuffer[1].Transpose();
	}

	graphicsDevice_->UpdateUniformBuffer(uniformBufferVS_, sizeof(Effekseer::Matrix44) * 2, 0, &constantVSBuffer);

	Effekseer::Backend::DrawParameter drawParam;

	drawParam.VertexBufferPtr = vb_;
	drawParam.IndexBufferPtr = ib_;
	drawParam.PipelineStatePtr = pip_;
	drawParam.VertexStride = sizeof(Vertex);
	drawParam.VertexUniformBufferPtrs[0] = uniformBufferVS_;
	drawParam.PrimitiveCount = static_cast<int32_t>(vertexies_.size() / 2);
	drawParam.InstanceCount = 1;
	drawParam.IndexOffset = 0;

	graphicsDevice_->Draw(drawParam);
}

void LineRenderer::ClearCache()
{
	if (!GetIsValid())
	{
		return;
	}

	vertexies_.clear();
}

} // namespace ToolRuntime
} // namespace Effekseer
