#include "EffekseerRendererDX11.MaterialLoader.h"
#include "../EffekseerRendererCommon/EffekseerRenderer.ShaderLoader.h"
#include "EffekseerRendererDX11.Shader.h"

#ifdef _DEBUG
#include <iostream>
#endif

namespace EffekseerRendererDX11
{

namespace MaterialVS
{
static
#include "Shader/EffekseerRenderer.Standard_VS.h"
};


MaterialLoader::MaterialLoader(Renderer* renderer, ::Effekseer::FileInterface* fileInterface) : fileInterface_(fileInterface)
{
	if (fileInterface == nullptr)
	{
		fileInterface_ = &defaultFileInterface_;
	}

	renderer_ = renderer;
	ES_SAFE_ADDREF(renderer_);
}

MaterialLoader ::~MaterialLoader() { ES_SAFE_RELEASE(renderer_); }

::Effekseer::MaterialData* MaterialLoader::Load(const EFK_CHAR* path)
{
	std::unique_ptr<Effekseer::FileReader> reader(fileInterface_->OpenRead(path));

	if (reader.get() != nullptr)
	{
		size_t size = reader->GetLength();
		char* data = new char[size];
		reader->Read(data, size);

		auto material = Load(data, (int32_t)size);

		delete[] data;

		return material;
	}

	return nullptr;
}

::Effekseer::MaterialData* MaterialLoader::Load(const void* data, int32_t size)
{
	EffekseerRenderer::ShaderLoader loader;
	if (!loader.Load((const uint8_t*)data, size))
	{
		return nullptr;
	}

	auto shaderCode = loader.GenerateShader();
#ifdef _DEBUG
	// std::cout << shaderCode << std::endl;
#endif

	auto materialData = new ::Effekseer::MaterialData();
	
	// Pos(3) Color(1) UV(2)
	D3D11_INPUT_ELEMENT_DESC decl[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, sizeof(float) * 3, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(float) * 4, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	// TODO write compiler!
	assert(0);
	auto shader = Shader::Create(static_cast<RendererImplemented*>(renderer_),
							  MaterialVS::g_VS,
								 sizeof(MaterialVS::g_VS),
								 MaterialVS::g_VS,
								 sizeof(MaterialVS::g_VS),
							  "MaterialStandardRenderer",
							  decl,
							  ARRAYSIZE(decl));
	if (shader == nullptr)
		return false;

	int32_t vertexUniformSize = sizeof(Effekseer::Matrix44) * 2 + sizeof(float) * 4;

	int32_t pixelUniformSize = sizeof(float) * 4;

	pixelUniformSize += loader.Uniforms.size() * 4 * sizeof(float);

	shader->SetVertexConstantBufferSize(vertexUniformSize);

	shader->SetPixelConstantBufferSize(pixelUniformSize);

	materialData->TextureCount = loader.Textures.size();
	materialData->UniformCount = loader.Uniforms.size();
	materialData->UserPtr = shader;
	return materialData;
}

void MaterialLoader::Unload(::Effekseer::MaterialData* data)
{

	if (data == nullptr)
		return;
	auto shader = reinterpret_cast<Shader*>(data->UserPtr);
	ES_SAFE_DELETE(shader);
	ES_SAFE_DELETE(data);
}

} // namespace EffekseerRendererDX11