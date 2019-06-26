#include "EffekseerRendererDX9.MaterialLoader.h"
#include "../EffekseerRendererCommon/EffekseerRenderer.ShaderLoader.h"
#include "EffekseerRendererDX9.Shader.h"

#ifdef _DEBUG
#include <iostream>
#endif

namespace EffekseerRendererDX9
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
	D3DVERTEXELEMENT9 decl[] = {{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
								{0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
								{0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
								D3DDECL_END()};

	// TODO write compiler!
	assert(0);
	auto shader = Shader::Create(static_cast<RendererImplemented*>(renderer_),
								 MaterialVS::g_vs20_VS,
								 sizeof(MaterialVS::g_vs20_VS),
								 MaterialVS::g_vs20_VS,
								 sizeof(MaterialVS::g_vs20_VS),
							  "MaterialRenderer",
							  decl);
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

} // namespace EffekseerRendererDX9