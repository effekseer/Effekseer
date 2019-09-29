#include "EffekseerMaterialCompilerDX11.h"

#include <d3d11.h>
#include <d3dcompiler.h>
#include <iostream>

#pragma comment(lib, "d3dcompiler.lib")

namespace Effekseer
{
namespace DX11
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

		ES_SAFE_RELEASE(shader);
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

		ES_SAFE_RELEASE(shader);
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
	float2 ScreenUV : TEXCOORD6;
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
	float2 ScreenUV : TEXCOORD6;
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

	float3 pixelNormalDir = worldNormal;
)";

static char* material_sprite_vs_suf1 = R"(

VS_Output main( const VS_Input Input )
{
	VS_Output Output = (VS_Output)0;
	float3 worldPos = Input.Pos;
	float3 worldNormal = (Input.Normal - float3(0.5, 0.5, 0.5)) * 2.0;
	float3 worldTangent = (Input.Tangent - float3(0.5, 0.5, 0.5)) * 2.0;
	float3 worldBinormal = cross(worldNormal, worldTangent);

	// UV
	float2 uv1 = Input.UV1;
	float2 uv2 = Input.UV1;
	uv1.y = mUVInversed.x + mUVInversed.y * uv1.y;
	uv2.y = mUVInversed.x + mUVInversed.y * uv2.y;

	// NBT
	Output.WorldN = worldNormal;
	Output.WorldB = worldBinormal;
	Output.WorldT = worldTangent;

	float3 pixelNormalDir = worldNormal;
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
	Output.ScreenUV = Output.Position.xy / Output.Position.w;
	Output.ScreenUV.xy = float2(Output.ScreenUV.x + 1.0, 1.0 - Output.ScreenUV.y) * 0.5;

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
	float2 ScreenUV : TEXCOORD6;
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

	float3 pixelNormalDir = worldNormal;
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
	Output.ScreenUV = Output.Position.xy / Output.Position.w;
	Output.ScreenUV.xy = float2(Output.ScreenUV.x + 1.0, 1.0 - Output.ScreenUV.y) * 0.5;

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
	float2 ScreenUV : TEXCOORD6;
};

float4 mUVInversedBack	: register(c0);
float4 predefined_uniform : register(c1);

)";

static char* g_material_ps_suf1 = R"(

#ifdef __MATERIAL_LIT__

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

#endif

float4 main( const PS_Input Input ) : SV_Target
{
	float2 uv1 = Input.UV1;
	float2 uv2 = Input.UV2;
	float3 worldPos = Input.WorldP;
	float3 worldNormal = Input.WorldN;
	float3 worldBinormal = Input.WorldB;
	float3 worldTangent = Input.WorldT;

	float3 pixelNormalDir = worldNormal;
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

	return Output;
}

)";

static char* g_material_ps_suf2_refraction = R"(

	float airRefraction = 1.0;
	float3 dir = mul((float3x3)cameraMat, pixelNormalDir);
	dir.y = -dir.y;

	float2 distortUV = 	dir.xy * (refraction - airRefraction);

	distortUV += Input.ScreenUV;
	distortUV.y = mUVInversedBack.x + mUVInversedBack.y * distortUV.y;

	float4 bg = background_texture.Sample(background_sampler, distortUV);
	float4 Output = bg;

	if(opacityMask <= 0.0f) discard;

	return Output;
}

)";

std::string Replace(std::string target, std::string from_, std::string to_)
{
	std::string::size_type Pos(target.find(from_));

	while (Pos != std::string::npos)
	{
		target.replace(Pos, from_.length(), to_);
		Pos = target.find(from_, Pos + to_.length());
	}

	return target;
}

struct ShaderData
{
	std::string CodeVS;
	std::string CodePS;
};

ShaderData GenerateShader(Material* material, MaterialShaderType shaderType)
{
	ShaderData shaderData;

	auto cind = 0;

	for (int stage = 0; stage < 2; stage++)
	{
		std::ostringstream maincode;

		if (stage == 0)
		{
			if (shaderType == MaterialShaderType::Standard || shaderType == MaterialShaderType::Refraction)
			{
				if (material->GetIsSimpleVertex())
				{
					maincode << material_sprite_vs_pre_simple;
				}
				else
				{
					maincode << material_sprite_vs_pre;
				}
				cind = 10;
			}
			else if (shaderType == MaterialShaderType::Model || shaderType == MaterialShaderType::RefractionModel)
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

		if (material->GetShadingModel() == ::Effekseer::ShadingModelType::Lit)
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

			maincode << "#define __MATERIAL_LIT__ 1" << std::endl;
		}
		else if (material->GetShadingModel() == ::Effekseer::ShadingModelType::Unlit)
		{
		}

		if (material->GetHasRefraction() && stage == 1 &&
			(shaderType == MaterialShaderType::Refraction || shaderType == MaterialShaderType::RefractionModel))
		{
			maincode << "float4x4 "
					 << "cameraMat"
					 << " : register(c" << cind << ");" << std::endl;
			cind += 4;
		}

		for (size_t i = 0; i < material->GetUniformCount(); i++)
		{
			maincode << "float4 " << material->GetUniformName(i) << " : register(c" << cind << ");" << std::endl;
			cind++;
		}

		for (size_t i = 0; i < material->GetTextureCount(); i++)
		{
			maincode << "Texture2D " << material->GetTextureName(i) << "_texture : register(t" << i << ");" << std::endl;
			maincode << "SamplerState " << material->GetTextureName(i) << "_sampler : register(s" << i << ");" << std::endl;
		}

		// background
		for (size_t i = material->GetTextureCount(); i < material->GetTextureCount() + 1; i++)
		{
			maincode << "Texture2D "
					 << "background"
					 << "_texture : register(t" << i << ");" << std::endl;
			maincode << "SamplerState "
					 << "background"
					 << "_sampler : register(s" << i << ");" << std::endl;
		}

		auto baseCode = std::string(material->GetGenericCode());
		baseCode = Replace(baseCode, "$F1$", "float");
		baseCode = Replace(baseCode, "$F2$", "float2");
		baseCode = Replace(baseCode, "$F3$", "float3");
		baseCode = Replace(baseCode, "$F4$", "float4");
		baseCode = Replace(baseCode, "$TIME$", "predefined_uniform.x");
		baseCode = Replace(baseCode, "$UV$", "uv");
		baseCode = Replace(baseCode, "$MOD", "fmod");
		baseCode = Replace(baseCode, "$SUFFIX", "");

		// replace textures
		for (size_t i = 0; i < material->GetTextureCount(); i++)
		{
			std::string keyP = "$TEX_P" + std::to_string(material->GetTextureIndex(i)) + "$";
			std::string keyS = "$TEX_S" + std::to_string(material->GetTextureIndex(i)) + "$";

			baseCode = Replace(
				baseCode, keyP, std::string(material->GetTextureName(i)) + "_texture.Sample(" + material->GetTextureName(i) + "_sampler,");
			baseCode = Replace(baseCode, keyS, ")");
		}

		if (stage == 0)
		{
			if (shaderType == MaterialShaderType::Standard || shaderType == MaterialShaderType::Refraction)
			{
				if (material->GetIsSimpleVertex())
				{
					maincode << material_sprite_vs_suf1_simple;
				}
				else
				{
					maincode << material_sprite_vs_suf1;
				}
			}
			else if (shaderType == MaterialShaderType::Model || shaderType == MaterialShaderType::RefractionModel)
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
			if (shaderType == MaterialShaderType::Standard || shaderType == MaterialShaderType::Refraction)
			{
				maincode << material_sprite_vs_suf2;
			}
			else if (shaderType == MaterialShaderType::Model || shaderType == MaterialShaderType::RefractionModel)
			{
				maincode << model_vs_suf2;
			}
		}
		else
		{
			if (shaderType == MaterialShaderType::Refraction || shaderType == MaterialShaderType::RefractionModel)
			{
				maincode << g_material_ps_suf2_refraction;
			}
			else
			{
				if (material->GetShadingModel() == Effekseer::ShadingModelType::Lit)
				{
					maincode << g_material_ps_suf2_lit;
				}
				else if (material->GetShadingModel() == Effekseer::ShadingModelType::Unlit)
				{
					maincode << g_material_ps_suf2_unlit;
				}
				else
				{
					assert(0);
				}
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

} // namespace DX11

} // namespace Effekseer

namespace Effekseer
{

class CompiledMaterialBinaryDX11 : public CompiledMaterialBinary, ReferenceObject
{
private:
	std::array<std::vector<uint8_t>, static_cast<int32_t>(MaterialShaderType::Max)> vertexShaders_;

	std::array<std::vector<uint8_t>, static_cast<int32_t>(MaterialShaderType::Max)> pixelShaders_;

public:
	CompiledMaterialBinaryDX11() {}

	virtual ~CompiledMaterialBinaryDX11() {}

	void SetVertexShaderData(MaterialShaderType type, const std::vector<uint8_t>& data)
	{
		vertexShaders_.at(static_cast<int>(type)) = data;
	}

	void SetPixelShaderData(MaterialShaderType type, const std::vector<uint8_t>& data) { pixelShaders_.at(static_cast<int>(type)) = data; }

	const uint8_t* GetVertexShaderData(MaterialShaderType type) const override { return vertexShaders_.at(static_cast<int>(type)).data(); }

	int32_t GetVertexShaderSize(MaterialShaderType type) const override { return vertexShaders_.at(static_cast<int>(type)).size(); }

	const uint8_t* GetPixelShaderData(MaterialShaderType type) const override { return pixelShaders_.at(static_cast<int>(type)).data(); }

	int32_t GetPixelShaderSize(MaterialShaderType type) const override { return pixelShaders_.at(static_cast<int>(type)).size(); }

	int AddRef() override { return ReferenceObject::AddRef(); }

	int Release() override { return ReferenceObject::Release(); }

	int GetRef() override { return ReferenceObject::GetRef(); }
};

CompiledMaterialBinary* MaterialCompilerDX11::Compile(Material* material)
{
	auto binary = new CompiledMaterialBinaryDX11();

	auto convertToVectorVS = [](const std::string& str) -> std::vector<uint8_t> {
		std::vector<uint8_t> ret;

		std::string log;
		std::vector<D3D_SHADER_MACRO> macros;

		auto blob = DX11::CompileVertexShader(str.c_str(), "VS", macros, log);

		if (blob != nullptr)
		{
			std::cout << str << std::endl;
			ret.resize(blob->GetBufferSize());
			memcpy(ret.data(), blob->GetBufferPointer(), ret.size());
			blob->Release();
		}
		else
		{
			std::cout << "VertexShader Compile Error" << std::endl;
			std::cout << log << std::endl;
			std::cout << str << std::endl;
		}

		return ret;
	};

	auto convertToVectorPS = [](const std::string& str) -> std::vector<uint8_t> {
		std::vector<uint8_t> ret;

		std::string log;
		std::vector<D3D_SHADER_MACRO> macros;

		auto blob = DX11::CompilePixelShader(str.c_str(), "PS", macros, log);

		if (blob != nullptr)
		{
			std::cout << str << std::endl;
			ret.resize(blob->GetBufferSize());
			memcpy(ret.data(), blob->GetBufferPointer(), ret.size());
			blob->Release();
		}
		else
		{
			std::cout << "PixelShader Compile Error" << std::endl;
			std::cout << log << std::endl;
			std::cout << str << std::endl;
		}

		return ret;
	};

	auto saveBinary = [&material, &binary, &convertToVectorVS, &convertToVectorPS](MaterialShaderType type) {
		auto shader = DX11::GenerateShader(material, type);
		binary->SetVertexShaderData(type, convertToVectorVS(shader.CodeVS));
		binary->SetPixelShaderData(type, convertToVectorPS(shader.CodePS));
	};

	if (material->GetHasRefraction())
	{
		saveBinary(MaterialShaderType::Refraction);
		saveBinary(MaterialShaderType::RefractionModel);
	}

	saveBinary(MaterialShaderType::Standard);
	saveBinary(MaterialShaderType::Model);

	return binary;
}

} // namespace Effekseer

#ifdef __SHARED_OBJECT__

extern "C"
{

#ifdef _WIN32
#define EFK_EXPORT __declspec(dllexport)
#else
#define EFK_EXPORT
#endif

	EFK_EXPORT Effekseer::MaterialCompiler* EFK_STDCALL CreateCompiler() { return new Effekseer::MaterialCompilerDX11(); }

}
#endif