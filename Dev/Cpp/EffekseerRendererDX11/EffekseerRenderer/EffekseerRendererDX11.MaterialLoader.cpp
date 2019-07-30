#include "EffekseerRendererDX11.MaterialLoader.h"
#include "../EffekseerRendererCommon/EffekseerRenderer.ShaderLoader.h"
#include "EffekseerRendererDX11.Shader.h"
#include "EffekseerRendererDX11.ModelRenderer.h"
#include <d3dcompiler.h>
#include <iostream>

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

	UINT flag = D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR;
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

	UINT flag = D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR;
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

static char* sprite_ps_pre = R"(

struct PS_Input
{
	float4 Pos		: SV_POSITION;
	float4 Color		: COLOR;
	float2 UV		: TEXCOORD0;
};


float4 predefined_uniform : register(c0);

)";

static char* sprite_ps_suf = R"(

float4 main( const PS_Input Input ) : SV_Target
{
	float4 Output =  Calculate(Input);

	if(Output.a == 0.0f) discard;

	return Output;
}

)";

static char* model_vs = R"(

float4x4 mCameraProj		: register( c0 );
float4x4 mModel[40]		: register( c4 );
float4	fUV[40]			: register( c164 );
float4	fModelColor[40]		: register( c204 );

float4 mUVInversed		: register(c247);


struct VS_Input
{
	float3 Pos		: POSITION0;
	float3 Normal		: NORMAL0;
	float3 Binormal		: NORMAL1;
	float3 Tangent		: NORMAL2;
	float2 UV		: TEXCOORD0;
	float4 Color		: NORMAL3;
	uint4 Index		: BLENDINDICES0;

};

struct VS_Output
{
	float4 Pos		: SV_POSITION;
	float2 UV		: TEXCOORD0;
	half3 Normal		: TEXCOORD1;
	half3 Binormal		: TEXCOORD2;
	half3 Tangent		: TEXCOORD3;
	float4 Color		: COLOR;
};

VS_Output main( const VS_Input Input )
{
	float4x4 matModel = mModel[Input.Index.x];
	float4 uv = fUV[Input.Index.x];
	float4 modelColor = fModelColor[Input.Index.x] * Input.Color;

	VS_Output Output = (VS_Output)0;
	float4 localPosition = { Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0 }; 
	localPosition = mul( matModel, localPosition );
	Output.Pos = mul( mCameraProj, localPosition );

	Output.UV.x = Input.UV.x * uv.z + uv.x;
	Output.UV.y = Input.UV.y * uv.w + uv.y;

	float3x3 lightMat = (float3x3)matModel;

	float4 localNormal = { 0.0, 0.0, 0.0, 1.0 };
	localNormal.xyz = normalize( mul( lightMat, Input.Normal ) );
	
	float4 localBinormal = { 0.0, 0.0, 0.0, 1.0 };
	localBinormal.xyz = normalize( mul( lightMat, Input.Binormal ) );

	float4 localTangent = { 0.0, 0.0, 0.0, 1.0 };
	localTangent.xyz = normalize( mul( lightMat, Input.Tangent ) );
	Output.Normal = localNormal.xyz;
	Output.Binormal = localBinormal.xyz;
	Output.Tangent = localTangent.xyz;

	Output.Color.r = 1.0;
	Output.Color.g = 1.0;
	Output.Color.b = 1.0;
	Output.Color.a = 1.0;
	Output.Color = Output.Color * modelColor;

	Output.UV.y = mUVInversed.x + mUVInversed.y * Output.UV.y;

	return Output;
}


)";

static char* model_ps_pre = R"(

struct PS_Input
{
	float4 Pos		: SV_POSITION;
	float2 UV		: TEXCOORD0;
	half3 Normal	: TEXCOORD1;
	half3 Binormal	: TEXCOORD2;
	half3 Tangent	: TEXCOORD3;
	float4 Color	: COLOR;
};

float4	fLightDirection		: register( c0 );
float4	fLightColor		: register( c1 );
float4	fLightAmbient		: register( c2 );



)";

static char* model_ps_suf = R"(

float4 main( const PS_Input Input ) : SV_Target
{
	float4 Output =  Calculate(Input);

	if(Output.a == 0.0f) discard;

	return Output;
}

)";

class ShaderLoader : public EffekseerRenderer::ShaderLoader
{
public:
	ShaderLoader() = default;
	virtual ~ShaderLoader() = default;

	virtual std::string GenerateShader(ShaderType shaderType) override
	{
		std::ostringstream maincode;

		if (shaderType == ShaderType::Standard)
		{
			maincode << sprite_ps_pre;
		}
		else if (shaderType == ShaderType::Model)
		{
			maincode << model_ps_pre;
		}

		auto cind = 1;

		for (size_t i = 0; i < Uniforms.size(); i++)
		{
			auto& uniform = Uniforms[i];
			maincode << "float4 " << uniform.Name << " : register(c" << cind << ");" << std::endl;
			cind++;
		}

		for (size_t i = 0; i < Textures.size(); i++)
		{
			auto& texture = Textures[i];
			maincode << "Texture2D " << texture.Name << "_texture : register(t" << i << ");" << std::endl;
			maincode << "SamplerState " << texture.Name << "_sampler : register(s" << i << ");" << std::endl;
		}

		auto baseCode = GenericCode;
		baseCode = Replace(baseCode, "$F1$", "float");
		baseCode = Replace(baseCode, "$F2$", "float2");
		baseCode = Replace(baseCode, "$F3$", "float3");
		baseCode = Replace(baseCode, "$F4$", "float4");
		baseCode = Replace(baseCode, "$TIME$", "predefined_uniform.x");
		baseCode = Replace(baseCode, "$UV$", "input.UV");
		baseCode = Replace(baseCode, "$INPUT$", "const PS_Input input");

		// replace textures
		for (size_t i = 0; i < Textures.size(); i++)
		{
			auto& texture = Textures[i];
			std::string keyP = "$TEX_P" + std::to_string(texture.Index) + "$";
			std::string keyS = "$TEX_S" + std::to_string(texture.Index) + "$";

			baseCode = Replace(baseCode, keyP, texture.Name + "_texture.Sample(" + texture.Name + "_sampler,");
			baseCode = Replace(baseCode, keyS, ")");
		}

		maincode << baseCode;

		if (shaderType == ShaderType::Standard)
		{
			maincode << sprite_ps_suf;
		}
		else if (shaderType == ShaderType::Model)
		{
			maincode << model_ps_suf;
		}

		return maincode.str();
	}
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
	ShaderLoader loader;
	if (!loader.Load((const uint8_t*)data, size))
	{
		return nullptr;
	}

	auto materialData = new ::Effekseer::MaterialData();

	{
		auto shaderCode = loader.GenerateShader(EffekseerRenderer::ShaderLoader::ShaderType::Standard);

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
		{
			std::cout << shaderCode << std::endl;
			std::cout << log << std::endl;
			return nullptr;
		}

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
		shader->SetVertexRegisterCount(8 + 1);

		shader->SetPixelConstantBufferSize(pixelUniformSize);
		shader->SetPixelRegisterCount(pixelUniformSize / (sizeof(float) * 4));

		materialData->TextureCount = loader.Textures.size();
		materialData->UniformCount = loader.Uniforms.size();
		materialData->UserPtr = shader;
	}

	{
		auto shaderCode = loader.GenerateShader(EffekseerRenderer::ShaderLoader::ShaderType::Model);

		// Pos(3) Color(1) UV(2)
		D3D11_INPUT_ELEMENT_DESC decl[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(float) * 3, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 1, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(float) * 6, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 2, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(float) * 9, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(float) * 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 3, DXGI_FORMAT_R8G8B8A8_UNORM, 0, sizeof(float) * 14, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"BLENDINDICES", 0, DXGI_FORMAT_R8G8B8A8_UINT, 0, sizeof(float) * 15, D3D11_INPUT_PER_VERTEX_DATA, 0},
		};

		// compile
		std::string log;
		std::vector<D3D_SHADER_MACRO> macros;

		auto vs = CompileVertexShader(model_vs, "VS", macros,  log);
		auto ps = CompilePixelShader(shaderCode.c_str(), "PS", macros, log);

		if (vs == nullptr)
		{
			std::cout << model_vs << std::endl;
			std::cout << log << std::endl;
			return nullptr;
		}

		if (ps == nullptr)
		{
			std::cout << shaderCode << std::endl;
			std::cout << log << std::endl;
			return nullptr;
		}

		auto shader = Shader::Create(static_cast<RendererImplemented*>(renderer_),
									 (uint8_t*)vs->GetBufferPointer(),
									 vs->GetBufferSize(),
									 (uint8_t*)ps->GetBufferPointer(),
									 ps->GetBufferSize(),
									 "MaterialStandardRenderer",
									 decl,
									 ARRAYSIZE(decl));
		ES_SAFE_RELEASE(vs);
		ES_SAFE_RELEASE(ps);

		if (shader == nullptr)
			return false;

		int32_t vertexUniformSize = sizeof(::EffekseerRenderer::ModelRendererVertexConstantBuffer<40>);

		int32_t pixelUniformSize = sizeof(::EffekseerRenderer::ModelRendererPixelConstantBuffer) + sizeof(float) * 4;

		pixelUniformSize += loader.Uniforms.size() * 4 * sizeof(float);

		shader->SetVertexConstantBufferSize(vertexUniformSize);
		shader->SetVertexRegisterCount(vertexUniformSize / 16);

		shader->SetPixelConstantBufferSize(pixelUniformSize);
		shader->SetPixelRegisterCount(pixelUniformSize / (sizeof(float) * 4));

		materialData->TextureCount = loader.Textures.size();
		materialData->UniformCount = loader.Uniforms.size();
		materialData->ModelUserPtr = shader;
	}

	return materialData;
}

void MaterialLoader::Unload(::Effekseer::MaterialData* data)
{
	if (data == nullptr)
		return;
	auto shader = reinterpret_cast<Shader*>(data->UserPtr);
	auto modelShader = reinterpret_cast<Shader*>(data->ModelUserPtr);

	ES_SAFE_DELETE(shader);
	ES_SAFE_DELETE(modelShader);
	ES_SAFE_DELETE(data);
}

} // namespace EffekseerRendererDX11