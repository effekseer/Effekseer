#include "EffekseerRendererArea.Shader.h"

namespace EffekseerRendererArea
{
Shader::Shader()
{
	vertexConstantBuffer.resize(sizeof(::Effekseer::Matrix44) * 2);
	pixelConstantBuffer.resize(sizeof(float));
}

Shader::~Shader() {}

} // namespace EffekseerRendererArea
