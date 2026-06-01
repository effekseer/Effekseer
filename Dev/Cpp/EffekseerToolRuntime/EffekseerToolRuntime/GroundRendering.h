#pragma once

namespace Effekseer
{
namespace ToolRuntime
{

enum class GroundRenderPass
{
	Color,
	Depth,
};

enum class GroundShaderBackend
{
	DirectX9,
	DirectX11,
	DirectX12,
	OpenGL,
	Vulkan,
	Metal,
	WebGPU,
};

struct GroundShaderCode
{
	const char* Vertex = nullptr;
	const char* DepthVertex = nullptr;
	const char* Pixel = nullptr;
	const char* DepthPixel = nullptr;
	const char* VertexProfile = nullptr;
	const char* PixelProfile = nullptr;
};

const GroundShaderCode& GetGroundShaderCode(GroundShaderBackend backend);

} // namespace ToolRuntime
} // namespace Effekseer
