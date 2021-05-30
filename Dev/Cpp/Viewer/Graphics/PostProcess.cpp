#include "PostProcess.h"

namespace Effekseer
{
namespace Tool
{

PostProcess::PostProcess(Effekseer::Backend::GraphicsDeviceRef graphicsDevice, Effekseer::Backend::ShaderRef shader, size_t uniformBufferVSSize, size_t uniformBufferPSSize)
	: shader_(shader)
{
	std::array<Vertex, 4> vdata;
	vdata[0] = Vertex{-1.0f, 1.0f, 0.0f, 0.0f};
	vdata[1] = Vertex{-1.0f, -1.0f, 0.0f, 1.0f};
	vdata[2] = Vertex{1.0f, 1.0f, 1.0f, 0.0f};
	vdata[3] = Vertex{1.0f, -1.0f, 1.0f, 1.0f};
	vb_ = graphicsDevice->CreateVertexBuffer(sizeof(Vertex), vdata.data(), false);

	std::array<int32_t, 6> idata;
}

void PostProcess::Render(Backend::TextureRef dst, Backend::TextureRef src)
{
}

} // namespace Tool
} // namespace Effekseer