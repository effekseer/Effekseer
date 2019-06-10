#pragma once

#include "../EffekseerRendererCommon/EffekseerRenderer.ShaderBase.h"
#include <vector>

namespace EffekseerRendererArea
{
class Shader
{
private:
	std::vector<uint8_t> vertexConstantBuffer;
	std::vector<uint8_t> pixelConstantBuffer;

public:
	Shader();

	virtual ~Shader();

	void* GetVertexConstantBuffer() { return vertexConstantBuffer.data(); }

	void* GetPixelConstantBuffer() { return pixelConstantBuffer.data(); }

	void SetConstantBuffer()
	{
		// TODO
	}
};
} // namespace EffekseerRendererArea
