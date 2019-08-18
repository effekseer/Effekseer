#include "EffekseerRendererDX11.MaterialLoader.h"
#include "../EffekseerRendererCommon/EffekseerRenderer.ShaderLoader.h"
#include "EffekseerRendererDX11.ModelRenderer.h"
#include "EffekseerRendererDX11.Shader.h"
#include <d3dcompiler.h>
#include <iostream>

namespace EffekseerRendererDX11
{

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

static char* material_sprite_vs_pre_simple = R"(

struct VS_Input
{
	float3 Pos		: POSITION0;
	float4 Color		: NORMAL0;
	float2 UV		: TEXCOORD0;
};

struct VS_Output
{
	float4 Position		: SV_POSITION;
	float4 VColor		: COLOR;
	float2 UV1		: TEXCOORD0;
	float2 UV2		: TEXCOORD1;
	float3 WorldP	: TEXCOORD2;
	float3 WorldN : TEXCOORD3;
	float3 WorldT : TEXCOORD4;
	float3 WorldB : TEXCOORD5;
};

float4x4 mCamera		: register(c0);
float4x4 mProj			: register(c4);
float4 mUVInversed		: register(c8);
float4 predefined_uniform : register(c9);

)";

static char* material_sprite_vs_pre = R"(

struct VS_Input
{
	float3 Pos		: POSITION0;
	float4 Color		: NORMAL0;
	float4 Normal		: NORMAL1;
	float4 Tangent		: NORMAL2;
	float2 UV1		: TEXCOORD0;
	float2 UV2		: TEXCOORD1;
};

struct VS_Output
{
	float4 Position		: SV_POSITION;
	float4 VColor		: COLOR;
	float2 UV1		: TEXCOORD0;
	float2 UV2		: TEXCOORD1;
	float3 WorldP	: TEXCOORD2;
	float3 WorldN : TEXCOORD3;
	float3 WorldT : TEXCOORD4;
	float3 WorldB : TEXCOORD5;
};

float4x4 mCamera		: register(c0);
float4x4 mProj			: register(c4);
float4 mUVInversed		: register(c8);
float4 predefined_uniform : register(c9);

)";

static char* material_sprite_vs_suf1_simple = R"(


VS_Output main( const VS_Input Input )
{
	VS_Output Output = (VS_Output)0;
	float4 worldPos = { Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0 };
	float3 worldNormal = float3(0.0, 0.0, 0.0);
	float3 worldBinormal = float3(0.0, 0.0, 0.0);
	float3 worldTangent = float3(0.0, 0.0, 0.0);

	// UV
	float uv1 = Input.UV1;
	float uv2 = Input.UV2;
	uv1.y = mUVInversed.x + mUVInversed.y * uv1.y;
	uv2.y = mUVInversed.x + mUVInversed.y * uv2.y;

	// NBT
	Output.WorldN = worldNormal;
	Output.WorldB = worldBinormal;
	Output.WorldT = worldTangent;
)";

static char* material_sprite_vs_suf1 = R"(

VS_Output main( const VS_Input Input )
{
	VS_Output Output = (VS_Output)0;
	float3 worldPos = Input.Pos;
	float3 worldNormal = Input.Normal;
	float3 worldBinormal = cross(Input.Normal, Input.Tangent);
	float3 worldTangent = Input.Tangent;

	// UV
	float2 uv1 = Input.UV1;
	float2 uv2 = Input.UV1;
	uv1.y = mUVInversed.x + mUVInversed.y * uv1.y;
	uv2.y = mUVInversed.x + mUVInversed.y * uv2.y;

	// NBT
	Output.WorldN = worldNormal;
	Output.WorldB = worldBinormal;
	Output.WorldT = worldTangent;
)";

static char* material_sprite_vs_suf2 = R"(

	worldPos = worldPos + worldPositionOffset;

	float4 cameraPos = mul(mCamera, float4(worldPos, 1.0));
	cameraPos = cameraPos / cameraPos.w;
	Output.Position = mul(mProj, cameraPos);

	Output.WorldP = worldPos;
	Output.VColor = Input.Color;
	Output.UV1 = uv1;
	Output.UV2 = uv2;

	return Output;
}

)";

static char* model_vs_pre = R"(

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
	float4 Position		: SV_POSITION;
	float4 VColor		: COLOR;
	float2 UV1		: TEXCOORD0;
	float2 UV2		: TEXCOORD1;
	float3 WorldP	: TEXCOORD2;
	float3 WorldN : TEXCOORD3;
	float3 WorldT : TEXCOORD4;
	float3 WorldB : TEXCOORD5;
};

float4x4 mCameraProj		: register( c0 );
float4x4 mModel[40]		: register( c4 );
float4	fUV[40]			: register( c164 );
float4	fModelColor[40]		: register( c204 );

float4 mUVInversed		: register(c244);
float4 predefined_uniform : register(c245);


)";

static char* model_vs_suf1 = R"(


VS_Output main( const VS_Input Input )
{
	float4x4 matModel = mModel[Input.Index.x];
	float4 uv = fUV[Input.Index.x];
	float4 modelColor = fModelColor[Input.Index.x] * Input.Color;

	VS_Output Output = (VS_Output)0;
	float4 localPosition = { Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0 }; 

	float3x3 matRotModel = (float3x3)matModel;

	float3 worldPos = mul( matModel, localPosition ).xyz;
	float3 worldNormal = normalize( mul( matRotModel, Input.Normal ) );
	float3 worldBinormal = normalize( mul( matRotModel, Input.Binormal ) );
	float3 worldTangent = normalize( mul( matRotModel, Input.Tangent ) );

	float2 uv1;
	uv1.x = Input.UV.x * uv.z + uv.x;
	uv1.y = Input.UV.y * uv.w + uv.y;
	uv1.y = mUVInversed.x + mUVInversed.y * uv1.y;
	float uv2 = uv1;

)";

static char* model_vs_suf2 = R"(

	worldPos = worldPos + worldPositionOffset;

	Output.Position = mul( mCameraProj,  float4(worldPos, 1.0) );

	Output.WorldP = worldPos;
	Output.WorldN = worldNormal;
	Output.WorldB = worldBinormal;
	Output.WorldT = worldTangent;

	Output.VColor = modelColor;
	Output.UV1 = uv1;
	Output.UV2 = uv2;
	return Output;
}


)";

static char* g_material_ps_pre = R"(

struct PS_Input
{
	float4 Position		: SV_POSITION;
	float4 VColor		: COLOR;
	float2 UV1		: TEXCOORD0;
	float2 UV2		: TEXCOORD1;
	float3 WorldP	: TEXCOORD2;
	float3 WorldN : TEXCOORD3;
	float3 WorldT : TEXCOORD4;
	float3 WorldB : TEXCOORD5;
};

float4 mUVInversed		: register(c0);
float4 predefined_uniform : register(c1);

)";

static char* g_material_ps_suf1 = R"(

float calcD_GGX(float roughness, float dotNH)
{
	float alpha = roughness*roughness;
	float alphaSqr = alpha*alpha;
	float pi = 3.14159;
	float denom = dotNH * dotNH *(alphaSqr-1.0) + 1.0;
	return (alpha / denom) * (alpha / denom) / pi;
}

float calcF(float F0, float dotLH)
{
	float dotLH5 = pow(1.0f-dotLH,5);
	return F0 + (1.0-F0)*(dotLH5);
}

float calcG_Schlick(float roughness, float dotNV, float dotNL)
{
	// UE4
	float k = (roughness + 1.0) * (roughness + 1.0) / 8.0;
	// float k = roughness * roughness / 2.0;

	float gV = dotNV*(1.0 - k) + k;
	float gL = dotNL*(1.0 - k) + k;

	return 1.0 / (gV * gL);
}

float calcLightingGGX(float3 N, float3 V, float3 L, float roughness, float F0)
{
	float3 H = normalize(V+L);

	float dotNL = saturate( dot(N,L) );
	float dotLH = saturate( dot(L,H) );
	float dotNH = saturate( dot(N,H) ) - 0.001;
	float dotNV = saturate( dot(N,V) ) + 0.001;

	float D = calcD_GGX(roughness, dotNH);
	float F = calcF(F0, dotLH);
	float G = calcG_Schlick(roughness, dotNV, dotNL);

	return dotNL * D * F * G / 4.0;
}

float3 calcDirectionalLightDiffuseColor(float3 diffuseColor, float3 normal, float3 lightDir, float ao)
{
	float3 color = float3(0.0,0.0,0.0);

	float NoL = dot(normal,lightDir);
	color.xyz = lightColor.xyz * max(NoL,0.0) * ao / 3.14;
	color.xyz = color.xyz * diffuseColor.xyz;
	return color;
}


float4 main( const PS_Input Input ) : SV_Target
{
	float2 uv1 = Input.UV1;
	float2 uv2 = Input.UV2;
	float3 worldPos = Input.WorldP;
	float3 worldNormal = Input.WorldN;
	float3 worldBinormal = Input.WorldB;
	float3 worldTangent = Input.WorldT;
)";

static char* g_material_ps_suf2_unlit = R"(


	float4 Output = float4(emissive, opacity);

	if(opacityMask <= 0.0f) discard;

	return Output;
}

)";

static char* g_material_ps_suf2_lit = R"(
	float3 viewDir = normalize(cameraPosition.xyz - worldPos);
	float3 diffuse = calcDirectionalLightDiffuseColor(baseColor, pixelNormalDir, lightDirection.xyz, ambientOcclusion);
	float3 specular = lightColor.xyz * calcLightingGGX(worldNormal, viewDir, lightDirection.xyz, roughness, 0.9);

	float4 Output =  float4(metallic * specular + (1.0 - metallic) * diffuse, opacity);

	if(opacityMask <= 0.0f) discard;

	// TODO refraction
	/*
	float airRefraction = 1.0;
	float2 distortUV = 	pixelNormalDir.xy * (refraction - airRefraction);
	float2 currentUV = hoge + distortUV;
	Output += tex(background, hoge) * (1.0 - opacity);
	*/

	return Output;
}

)";

class ShaderLoader : public EffekseerRenderer::ShaderLoader
{
public:
	ShaderLoader() = default;
	virtual ~ShaderLoader() = default;

	virtual ::EffekseerRenderer::ShaderData GenerateShader(ShaderType shaderType) override
	{
		::EffekseerRenderer::ShaderData shaderData;

		auto cind = 0;

		for (int stage = 0; stage < 2; stage++)
		{
			std::ostringstream maincode;

			if (stage == 0)
			{
				if (shaderType == ShaderType::Standard)
				{
					if (IsSimpleVertex)
					{
						maincode << material_sprite_vs_pre_simple;
					}
					else
					{
						maincode << material_sprite_vs_pre;
					}
					cind = 10;
				}
				else if (shaderType == ShaderType::Model)
				{
					maincode << model_vs_pre;
					cind = 246;
				}
			}
			else
			{
				maincode << g_material_ps_pre;
				cind = 2;
			}

			if (ShadingModel == ::Effekseer::ShadingModelType::Lit)
			{
				maincode << "float4 "
						 << "cameraPosition"
						 << " : register(c" << cind << ");" << std::endl;
				cind++;
				maincode << "float4 "
						 << "lightDirection"
						 << " : register(c" << cind << ");" << std::endl;
				cind++;
				maincode << "float4 "
						 << "lightColor"
						 << " : register(c" << cind << ");" << std::endl;
				cind++;
				maincode << "float4 "
						 << "lightAmbientColor"
						 << " : register(c" << cind << ");" << std::endl;
				cind++;
			}
			else if (ShadingModel == ::Effekseer::ShadingModelType::Unlit)
			{
			}

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
			baseCode = Replace(baseCode, "$UV$", "uv1");
			baseCode = Replace(baseCode, "$MOD", "fmod");
			baseCode = Replace(baseCode, "$SUFFIX", "");

			// replace textures
			for (size_t i = 0; i < Textures.size(); i++)
			{
				auto& texture = Textures[i];
				std::string keyP = "$TEX_P" + std::to_string(texture.Index) + "$";
				std::string keyS = "$TEX_S" + std::to_string(texture.Index) + "$";

				baseCode = Replace(baseCode, keyP, texture.Name + "_texture.Sample(" + texture.Name + "_sampler,");
				baseCode = Replace(baseCode, keyS, ")");
			}

			if (stage == 0)
			{
				if (shaderType == ShaderType::Standard)
				{
					if (IsSimpleVertex)
					{
						maincode << material_sprite_vs_suf1_simple;
					}
					else
					{
						maincode << material_sprite_vs_suf1;
					}
				}
				else if (shaderType == ShaderType::Model)
				{
					maincode << model_vs_suf1;
				}
			}
			else
			{
				maincode << g_material_ps_suf1;
			}

			maincode << baseCode;

			if (stage == 0)
			{
				if (shaderType == ShaderType::Standard)
				{
					maincode << material_sprite_vs_suf2;
				}
				else if (shaderType == ShaderType::Model)
				{
					maincode << model_vs_suf2;
				}
			}
			else
			{
				if (ShadingModel == Effekseer::ShadingModelType::Lit)
				{
					maincode << g_material_ps_suf2_lit;
				}
				else if (ShadingModel == Effekseer::ShadingModelType::Unlit)
				{
					maincode << g_material_ps_suf2_unlit;
				}
				else
				{
					assert(0);
				}
			}

			if (stage == 0)
			{
				shaderData.CodeVS = maincode.str();
			}
			else
			{
				shaderData.CodePS = maincode.str();
			}
		}

		return shaderData;
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
	materialData->IsSimpleVertex = loader.IsSimpleVertex;

	{
		auto shaderCode = loader.GenerateShader(EffekseerRenderer::ShaderLoader::ShaderType::Standard);

		// compile
		std::string log;
		std::vector<D3D_SHADER_MACRO> macros;

		auto vs = CompileVertexShader(shaderCode.CodeVS.c_str(), "VS", macros, log);
		auto ps = CompilePixelShader(shaderCode.CodePS.c_str(), "PS", macros, log);

#ifdef _DEBUG
		std::cout << shaderCode.CodeVS << std::endl;
		std::cout << shaderCode.CodePS << std::endl;
#endif
		if (vs == nullptr)
		{
			std::cout << shaderCode.CodeVS << std::endl;
			std::cout << log << std::endl;
			return nullptr;
		}

		if (ps == nullptr)
		{
			std::cout << shaderCode.CodePS << std::endl;
			std::cout << log << std::endl;
			return nullptr;
		}

		Shader* shader = nullptr;

		if (materialData->IsSimpleVertex)
		{
			// Pos(3) Color(1) UV(2)
			D3D11_INPUT_ELEMENT_DESC decl[] = {
				{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"NORMAL", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, sizeof(float) * 3, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(float) * 4, D3D11_INPUT_PER_VERTEX_DATA, 0},
			};

			shader = Shader::Create(static_cast<RendererImplemented*>(renderer_),
									(uint8_t*)vs->GetBufferPointer(),
									vs->GetBufferSize(),
									(uint8_t*)ps->GetBufferPointer(),
									ps->GetBufferSize(),
									"MaterialStandardRenderer",
									decl,
									ARRAYSIZE(decl));
		}
		else
		{
			// Pos(3) Color(1) Normal(1) Tangent(1) UV(2) UV(2)
			D3D11_INPUT_ELEMENT_DESC decl[] = {
				{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"NORMAL", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, sizeof(float) * 3, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"NORMAL", 1, DXGI_FORMAT_R8G8B8A8_UNORM, 0, sizeof(float) * 4, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"NORMAL", 2, DXGI_FORMAT_R8G8B8A8_UNORM, 0, sizeof(float) * 5, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(float) * 6, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(float) * 8, D3D11_INPUT_PER_VERTEX_DATA, 0},

			};

			shader = Shader::Create(static_cast<RendererImplemented*>(renderer_),
									(uint8_t*)vs->GetBufferPointer(),
									vs->GetBufferSize(),
									(uint8_t*)ps->GetBufferPointer(),
									ps->GetBufferSize(),
									"MaterialStandardRenderer",
									decl,
									ARRAYSIZE(decl));
		}

		ES_SAFE_RELEASE(vs);
		ES_SAFE_RELEASE(ps);

		if (shader == nullptr)
			return false;

		// vs ps fixed
		int32_t vertexUniformSizeFixed = sizeof(Effekseer::Matrix44) * 2 + sizeof(float) * 4;
		int32_t pixelUniformSizeFixed = sizeof(float) * 4;

		// vs ps shadermodel (light)
		int32_t vertexUniformSizeModel = 0;
		int32_t pixelUniformSizeModel = 0;

		// int32_t vertexUniformSizeModel = 0;
		// int32_t pixelUniformSizeModel = sizeof(float) * 4 * 3;

		// vs ps material common (predefined)
		int32_t vertexUniformSizeCommon = sizeof(float) * 4;
		int32_t pixelUniformSizeCommon = sizeof(float) * 4;

		int32_t vertexUniformSize = vertexUniformSizeFixed + vertexUniformSizeCommon + vertexUniformSizeModel;
		int32_t pixelUniformSize = pixelUniformSizeFixed + pixelUniformSizeCommon + pixelUniformSizeModel;

		vertexUniformSize += loader.Uniforms.size() * 4 * sizeof(float);
		pixelUniformSize += loader.Uniforms.size() * 4 * sizeof(float);

		// shiding model
		if (loader.ShadingModel == ::Effekseer::ShadingModelType::Lit)
		{
			pixelUniformSize += sizeof(float) * 16;
		}
		else if (loader.ShadingModel == ::Effekseer::ShadingModelType::Unlit)
		{
		}

		shader->SetVertexConstantBufferSize(vertexUniformSize);
		shader->SetVertexRegisterCount(vertexUniformSize / (sizeof(float) * 4));

		shader->SetPixelConstantBufferSize(pixelUniformSize);
		shader->SetPixelRegisterCount(pixelUniformSize / (sizeof(float) * 4));

		materialData->TextureCount = loader.Textures.size();
		materialData->UniformCount = loader.Uniforms.size();
		materialData->UserPtr = shader;
	}

	{
		auto shaderCode = loader.GenerateShader(EffekseerRenderer::ShaderLoader::ShaderType::Model);

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

		auto vs = CompileVertexShader(shaderCode.CodeVS.c_str(), "VS", macros, log);
		auto ps = CompilePixelShader(shaderCode.CodePS.c_str(), "PS", macros, log);

		if (vs == nullptr)
		{
			std::cout << shaderCode.CodeVS.c_str() << std::endl;
			std::cout << log << std::endl;
			return nullptr;
		}

		if (ps == nullptr)
		{
			std::cout << shaderCode.CodePS << std::endl;
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

		// vs ps fixed
		int32_t vertexUniformSizeFixed =
			sizeof(Effekseer::Matrix44) + (sizeof(Effekseer::Matrix44) + sizeof(float) * 4 + sizeof(float) * 4) * 40 + sizeof(float) * 4;
		int32_t pixelUniformSizeFixed = sizeof(float) * 4;

		// vs ps shadermodel (light)
		int32_t vertexUniformSizeModel = 0;
		int32_t pixelUniformSizeModel = 0;

		// int32_t vertexUniformSizeModel = 0;
		// int32_t pixelUniformSizeModel = sizeof(float) * 4 * 3;

		// vs ps material common (predefined)
		int32_t vertexUniformSizeCommon = sizeof(float) * 4;
		int32_t pixelUniformSizeCommon = sizeof(float) * 4;

		int32_t vertexUniformSize = vertexUniformSizeFixed + vertexUniformSizeCommon + vertexUniformSizeModel;
		int32_t pixelUniformSize = pixelUniformSizeFixed + pixelUniformSizeCommon + pixelUniformSizeModel;

		// shiding model
		if (loader.ShadingModel == ::Effekseer::ShadingModelType::Lit)
		{
			pixelUniformSize += sizeof(float) * 16;
		}
		else if (loader.ShadingModel == ::Effekseer::ShadingModelType::Unlit)
		{
		}

		vertexUniformSize += loader.Uniforms.size() * 4 * sizeof(float);
		pixelUniformSize += loader.Uniforms.size() * 4 * sizeof(float);

		shader->SetVertexConstantBufferSize(vertexUniformSize);
		shader->SetVertexRegisterCount(vertexUniformSize / (sizeof(float) * 4));

		shader->SetPixelConstantBufferSize(pixelUniformSize);
		shader->SetPixelRegisterCount(pixelUniformSize / (sizeof(float) * 4));

		materialData->ModelUserPtr = shader;
	}

	materialData->TextureCount = loader.Textures.size();
	materialData->UniformCount = loader.Uniforms.size();
	materialData->ShadingModel = loader.ShadingModel;
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