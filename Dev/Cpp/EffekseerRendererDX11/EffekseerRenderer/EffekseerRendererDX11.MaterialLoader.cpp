#include "EffekseerRendererDX11.MaterialLoader.h"
#include "../EffekseerRendererCommon/EffekseerRenderer.ShaderLoader.h"
#include "EffekseerRendererDX11.Shader.h"

#include <d3dcompiler.h>

#ifdef _DEBUG
#include <iostream>
#endif

namespace EffekseerRendererDX11
{

namespace MaterialVS
{
static
#include "Shader/EffekseerRenderer.Standard_VS.h"
}; // namespace MaterialVS

static ID3DBlob* CompileVertexShader(const char* vertexShaderText,
									 const char* vertexShaderFileName,
									 const std::vector<D3D_SHADER_MACRO>& macro,
									 std::string& log)
{
	ID3DBlob* shader = nullptr;
	ID3DBlob* error = nullptr;

	UINT flag = D3D10_SHADER_PACK_MATRIX_ROW_MAJOR;
#if !_DEBUG
	flag = flag | D3D10_SHADER_OPTIMIZATION_LEVEL3;
#endif

	HRESULT hr;

	hr = D3DCompile(vertexShaderText,
					strlen(vertexShaderText),
					vertexShaderFileName,
					macro.size() > 0 ? (D3D_SHADER_MACRO*)&macro[0] : NULL,
					NULL,
					"main",
					"vs_4_0",
					flag,
					0,
					&shader,
					&error);

	if (FAILED(hr))
	{
		if (hr == E_OUTOFMEMORY)
		{
			log += "Out of memory\n";
		}
		else
		{
			log += "Unknown error\n";
		}

		if (error != NULL)
		{
			log += (const char*)error->GetBufferPointer();
			error->Release();
		}

		return nullptr;
	}

	return shader;
}

static ID3DBlob* CompilePixelShader(const char* vertexShaderText,
									const char* vertexShaderFileName,
									const std::vector<D3D_SHADER_MACRO>& macro,
									std::string& log)
{
	ID3DBlob* shader = nullptr;
	ID3DBlob* error = nullptr;

	UINT flag = D3D10_SHADER_PACK_MATRIX_ROW_MAJOR;
#if !_DEBUG
	flag = flag | D3D10_SHADER_OPTIMIZATION_LEVEL3;
#endif

	HRESULT hr;

	hr = D3DCompile(vertexShaderText,
					strlen(vertexShaderText),
					vertexShaderFileName,
					macro.size() > 0 ? (D3D_SHADER_MACRO*)&macro[0] : NULL,
					NULL,
					"main",
					"ps_4_0",
					flag,
					0,
					&shader,
					&error);

	if (FAILED(hr))
	{
		if (hr == E_OUTOFMEMORY)
		{
			log += "Out of memory\n";
		}
		else
		{
			log += "Unknown error\n";
		}

		if (error != NULL)
		{
			log += (const char*)error->GetBufferPointer();
			error->Release();
		}

		return nullptr;
	}

	return shader;
}

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

	// compile
	std::string log;
	std::vector<D3D_SHADER_MACRO> macros;

	// auto vs = CompileVertexShader(shaderCode.c_str(), "VS", macros,  log);
	auto ps = CompilePixelShader(shaderCode.c_str(), "PS", macros, log);

	if (ps == nullptr)
		return nullptr;


	auto shader = Shader::Create(static_cast<RendererImplemented*>(renderer_),
								 MaterialVS::g_VS,
								 sizeof(MaterialVS::g_VS),
								 (uint8_t*)ps->GetBufferPointer(),
								 ps->GetBufferSize(),
								 "MaterialStandardRenderer",
								 decl,
								 ARRAYSIZE(decl));

	ES_SAFE_RELEASE(ps);

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