#include "EffekseerRendererArea.Shader.h"

namespace EffekseerRendererArea
{
Shader::Shader()
{
	vertexConstantBuffer.resize(sizeof(::Effekseer::Matrix44) * 2 + sizeof(float) * 4);
	pixelConstantBuffer.resize(sizeof(float) * 4 + sizeof(float) * 4);
}

Shader::~Shader() {}

} // namespace EffekseerRendererArea
