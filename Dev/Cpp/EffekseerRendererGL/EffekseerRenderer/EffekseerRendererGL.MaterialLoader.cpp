#include "EffekseerRendererGL.MaterialLoader.h"
#include "../EffekseerRendererCommon/EffekseerRenderer.ShaderLoader.h"
#include "EffekseerRendererGL.ModelRenderer.h"
#include "EffekseerRendererGL.Shader.h"

#include <iostream>

namespace EffekseerRendererGL
{

static const char g_material_model_vs_src_pre[] =
	R"(
IN vec4 a_Position;
IN vec3 a_Normal;
IN vec3 a_Binormal;
IN vec3 a_Tangent;
IN vec2 a_TexCoord;
IN vec4 a_Color;
)"
#if defined(MODEL_SOFTWARE_INSTANCING)
	R"(
IN float a_InstanceID;
IN vec4 a_UVOffset;
IN vec4 a_ModelColor;
)"
#endif
	R"(

OUT lowp vec4 v_VColor;
OUT mediump vec2 v_UV1;
OUT mediump vec2 v_UV2;
OUT mediump vec3 v_WorldP;
OUT mediump vec3 v_WorldN;
OUT mediump vec3 v_WorldT;
OUT mediump vec3 v_WorldB;

)"
#if defined(MODEL_SOFTWARE_INSTANCING)
	R"(
uniform mat4 ModelMatrix[20];
uniform vec4 UVOffset[20];
uniform vec4 ModelColor[20];
)"
#else
	R"(
uniform mat4 ModelMatrix;
uniform vec4 UVOffset;
uniform vec4 ModelColor;
)"
#endif
	R"(
uniform mat4 ProjectionMatrix;
uniform vec4 mUVInversed;
uniform vec4 predefined_uniform;


)";

static const char g_material_model_vs_src_suf1[] =
	R"(

void main()
{
)"
#if defined(MODEL_SOFTWARE_INSTANCING)
	R"(
	mat4 modelMatrix = ModelMatrix[int(a_InstanceID)];
	vec4 uvOffset = a_UVOffset;
	vec4 modelColor = a_ModelColor;
)"
#else
	R"(
	mat4 modelMatrix = ModelMatrix;
	vec4 uvOffset = UVOffset;
	vec4 modelColor = ModelColor * a_Color;
)"
#endif
	R"(
	mat3 modelMatRot = mat3(modelMatrix);
	vec3 worldPos = (modelMatrix * a_Position).xyz;
	vec3 worldNormal = normalize(modelMatRot * a_Normal);
	vec3 worldBinormal = normalize(modelMatRot * a_Binormal);
	vec3 worldTangent = normalize(modelMatRot * a_Tangent);

	// UV
	vec2 uv1 = a_TexCoord.xy * uvOffset.zw + uvOffset.xy;
	vec2 uv2 = uv1;
)";

static const char g_material_model_vs_src_suf2[] =
	R"(
	worldPos = worldPos + worldPositionOffset;

	v_WorldP = worldPos;
	v_WorldN = worldNormal;
	v_WorldB = worldBinormal;
	v_WorldT = worldTangent;
	v_UV1 = uv1;
	v_UV2 = uv2;
	v_VColor = a_Color;
	gl_Position = ProjectionMatrix * vec4(worldPos, 1.0);
}
)";

static const char g_material_sprite_vs_src_pre_simple[] =
	R"(
IN vec4 atPosition;
IN vec4 atColor;
IN vec4 atTexCoord;
)"

	R"(
OUT lowp vec4 v_VColor;
OUT mediump vec2 v_UV1;
OUT mediump vec2 v_UV2;
OUT mediump vec3 v_WorldP;
OUT mediump vec3 v_WorldN;
OUT mediump vec3 v_WorldT;
OUT mediump vec3 v_WorldB;
)"

	R"(
uniform mat4 uMatCamera;
uniform mat4 uMatProjection;
uniform vec4 mUVInversed;
uniform vec4 predefined_uniform;

)";

static const char g_material_sprite_vs_src_pre[] =
	R"(
IN vec4 atPosition;
IN vec4 atColor;
IN vec3 atNormal;
IN vec3 atTangent;
IN vec2 atTexCoord;
IN vec2 atTexCoord2;
)"

	R"(
OUT lowp vec4 v_VColor;
OUT mediump vec2 v_UV1;
OUT mediump vec2 v_UV2;
OUT mediump vec3 v_WorldP;
OUT mediump vec3 v_WorldN;
OUT mediump vec3 v_WorldT;
OUT mediump vec3 v_WorldB;
)"

	R"(
uniform mat4 uMatCamera;
uniform mat4 uMatProjection;
uniform vec4 mUVInversed;
uniform vec4 predefined_uniform;

)";

static const char g_material_sprite_vs_src_suf1_simple[] =

	R"(

void main() {
	vec3 worldPos = atPosition.xyz;

	// UV
	vec2 uv1 = atTexCoord.xy;
	uv1.y = mUVInversed.x + mUVInversed.y * uv1.y;
	vec2 uv2 = uv1;

	// NBT
	vec3 worldNormal = vec3(0.0, 0.0, 1.0);
	vec3 worldBinormal = vec3(0.0, 1.0, 1.0);
	vec3 worldTangent = vec3(1.0, 0.0, 1.0);
	v_WorldN = worldNormal;
	v_WorldB = worldBinormal;
	v_WorldT = worldTangent;

)";

static const char g_material_sprite_vs_src_suf1[] =

	R"(

void main() {
	vec3 worldPos = atPosition.xyz;

	// UV
	vec2 uv1 = atTexCoord.xy;
	uv1.y = mUVInversed.x + mUVInversed.y * uv1.y;
	vec2 uv2 = atTexCoord2.xy;
	uv2.y = mUVInversed.x + mUVInversed.y * uv2.y;

	// NBT
	vec3 worldNormal = atNormal;
	vec3 worldBinormal = cross(atNormal, atTangent);
	vec3 worldTangent = atTangent;
	v_WorldN = worldNormal;
	v_WorldB = worldBinormal;
	v_WorldT = worldTangent;
)";

static const char g_material_sprite_vs_src_suf2[] =

	R"(
	worldPos = worldPos + worldPositionOffset;

	vec4 cameraPos = uMatCamera * vec4(worldPos, 1.0);
	cameraPos = cameraPos / cameraPos.w;

	gl_Position = uMatProjection * cameraPos;

	v_WorldP = worldPos;
	v_VColor = atColor;

	v_UV1 = uv1;
	v_UV2 = uv2;
}

)";

static const char g_material_fs_src_pre[] =
	R"(

IN lowp vec4 v_VColor;
IN mediump vec2 v_UV1;
IN mediump vec2 v_UV2;
IN mediump vec3 v_WorldP;
IN mediump vec3 v_WorldN;
IN mediump vec3 v_WorldT;
IN mediump vec3 v_WorldB;

uniform vec4 mUVInversed;
uniform vec4 predefined_uniform;

)";

static const char g_material_fs_src_suf1[] =
	R"(

void main()
{
	vec2 uv1 = v_UV1;
	vec2 uv2 = v_UV2;
	vec3 worldPos = v_WorldP;
	vec3 worldNormal = v_WorldN;
	vec3 worldTangent = v_WorldT;
	vec3 worldBinormal = v_WorldB;
)";

static const char g_material_fs_src_suf2_lit[] =
	R"(

	FRAGCOLOR = vec4(emissive, opacity);
}

)";

static const char g_material_fs_src_suf2_unlit[] =
	R"(

	FRAGCOLOR = vec4(emissive, opacity);
}

)";

class ShaderLoader : public EffekseerRenderer::ShaderLoader
{
public:
	ShaderLoader() = default;
	virtual ~ShaderLoader() = default;

	virtual EffekseerRenderer::ShaderData GenerateShader(ShaderType shaderType)
	{
		::EffekseerRenderer::ShaderData shaderData;

		for (int stage = 0; stage < 2; stage++)
		{
			std::ostringstream maincode;

			if (stage == 0)
			{
				if (shaderType == ShaderType::Standard)
				{
					if (IsSimpleVertex)
					{
						maincode << g_material_sprite_vs_src_pre_simple;
					}
					else
					{
						maincode << g_material_sprite_vs_src_pre;
					}
				}
				else if (shaderType == ShaderType::Model)
				{
					maincode << g_material_model_vs_src_pre;
				}
			}
			else
			{
				maincode << g_material_fs_src_pre;
			}

			for (size_t i = 0; i < Uniforms.size(); i++)
			{
				auto& uniform = Uniforms[i];
				if (uniform.Index == 0)
					maincode << "uniform float " << uniform.Name << ";" << std::endl;
				if (uniform.Index == 1)
					maincode << "uniform vec2 " << uniform.Name << ";" << std::endl;
				if (uniform.Index == 2)
					maincode << "uniform vec3 " << uniform.Name << ";" << std::endl;
				if (uniform.Index == 3)
					maincode << "uniform vec4 " << uniform.Name << ";" << std::endl;
			}

			for (size_t i = 0; i < Textures.size(); i++)
			{
				auto& texture = Textures[i];
				maincode << "uniform sampler2D " << texture.Name << ";" << std::endl;
			}

			if (ShadingModel == ::Effekseer::ShadingModelType::Lit)
			{
				maincode << "uniform vec4 "
						 << "lightDirection"
						 << ";" << std::endl;
				maincode << "uniform vec4 "
						 << "lightColor"
						 << ";" << std::endl;
				maincode << "uniform vec4 "
						 << "lightAmbientColor"
						 << ";" << std::endl;
				maincode << "uniform vec4 "
						 << "cameraPosition"
						 << ";" << std::endl;
			}
			else if (ShadingModel == ::Effekseer::ShadingModelType::Unlit)
			{
			}

			auto baseCode = GenericCode;
			baseCode = Replace(baseCode, "$F1$", "float");
			baseCode = Replace(baseCode, "$F2$", "vec2");
			baseCode = Replace(baseCode, "$F3$", "vec3");
			baseCode = Replace(baseCode, "$F4$", "vec4");
			baseCode = Replace(baseCode, "$TIME$", "predefined_uniform.x");
			baseCode = Replace(baseCode, "$UV$", "uv1.xy");
			baseCode = Replace(baseCode, "$MOD", "mod");
			baseCode = Replace(baseCode, "$SUFFIX", "");

			// replace textures
			for (size_t i = 0; i < Textures.size(); i++)
			{
				auto& texture = Textures[i];
				std::string keyP = "$TEX_P" + std::to_string(texture.Index) + "$";
				std::string keyS = "$TEX_S" + std::to_string(texture.Index) + "$";

				baseCode = Replace(baseCode, keyP, "TEX2D(" + texture.Name + ",");
				baseCode = Replace(baseCode, keyS, ")");
			}

			if (stage == 0)
			{
				if (shaderType == ShaderType::Standard)
				{
					if (IsSimpleVertex)
					{
						maincode << g_material_sprite_vs_src_suf1_simple;
					}
					else
					{
						maincode << g_material_sprite_vs_src_suf1;
					}
				}
				else if (shaderType == ShaderType::Model)
				{
					maincode << g_material_model_vs_src_suf1;
				}

				maincode << baseCode;

				if (shaderType == ShaderType::Standard)
				{
					maincode << g_material_sprite_vs_src_suf2;
				}
				else if (shaderType == ShaderType::Model)
				{
					maincode << g_material_model_vs_src_suf2;
				}

				shaderData.CodeVS = maincode.str();
			}
			else
			{
				maincode << g_material_fs_src_suf1;

				maincode << baseCode;

				if (ShadingModel == Effekseer::ShadingModelType::Lit)
				{
					maincode << g_material_fs_src_suf2_lit;
				}
				else if (ShadingModel == Effekseer::ShadingModelType::Unlit)
				{
					maincode << g_material_fs_src_suf2_unlit;
				}

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

		auto shader = Shader::Create(renderer_,
									 shaderCode.CodeVS.c_str(),
									 shaderCode.CodeVS.size(),
									 shaderCode.CodePS.c_str(),
									 shaderCode.CodePS.size(),
									 "CustomMaterial");

		bool isDebug = false;
#ifdef _DEBUG
		isDebug = true;
#endif

		if (shader == nullptr || isDebug)
		{
			std::cout << shaderCode.CodeVS << std::endl;
			std::cout << shaderCode.CodePS << std::endl;

			if (shader == nullptr)
			{
				return nullptr;
			}
		}

		if (materialData->IsSimpleVertex)
		{
			EffekseerRendererGL::ShaderAttribInfo sprite_attribs[3] = {
				{"atPosition", GL_FLOAT, 3, 0, false}, {"atColor", GL_UNSIGNED_BYTE, 4, 12, true}, {"atTexCoord", GL_FLOAT, 2, 16, false}};
			shader->GetAttribIdList(3, sprite_attribs);
			shader->SetVertexSize(sizeof(EffekseerRendererGL::Vertex));
		}
		else
		{
			EffekseerRendererGL::ShaderAttribInfo sprite_attribs[6] = {
				{"atPosition", GL_FLOAT, 3, 0, false},
				{"atColor", GL_UNSIGNED_BYTE, 4, 12, true},
				{"atNormal", GL_UNSIGNED_BYTE, 4, 16, true},
				{"atTangent", GL_UNSIGNED_BYTE, 4, 20, true},
				{"atTexCoord", GL_FLOAT, 2, 24, false},
				{"atTexCoord2", GL_FLOAT, 2, 32, false},
			};

			shader->GetAttribIdList(6, sprite_attribs);
			shader->SetVertexSize(sizeof(EffekseerRenderer::DynamicVertex));
		}

		int32_t vsOffset = 0;
		shader->AddVertexConstantLayout(CONSTANT_TYPE_MATRIX44, shader->GetUniformId("uMatCamera"), vsOffset);
		vsOffset += sizeof(Effekseer::Matrix44);

		shader->AddVertexConstantLayout(CONSTANT_TYPE_MATRIX44, shader->GetUniformId("uMatProjection"), vsOffset);
		vsOffset += sizeof(Effekseer::Matrix44);

		shader->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("mUVInversed"), vsOffset);
		vsOffset += sizeof(float) * 4;

		shader->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("predefined_uniform"), vsOffset);
		vsOffset += sizeof(float) * 4;

		for (auto uniform : loader.Uniforms)
		{
			shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId(uniform.Name.c_str()), vsOffset);
			vsOffset += sizeof(float) * 4;
		}

		shader->SetVertexConstantBufferSize(vsOffset);

		int32_t psOffset = 0;

		shader->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("mUVInversed"), psOffset);
		psOffset += sizeof(float) * 4;

		shader->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("predefined_uniform"), psOffset);
		psOffset += sizeof(float) * 4;

		// shiding model
		if (loader.ShadingModel == ::Effekseer::ShadingModelType::Lit)
		{
			shader->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("cameraPosition"), psOffset);
			psOffset += sizeof(float) * 4;
			shader->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("lightDirection"), psOffset);
			psOffset += sizeof(float) * 4;
			shader->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("lightColor"), psOffset);
			psOffset += sizeof(float) * 4;
			shader->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("lightAmbientColor"), psOffset);
			psOffset += sizeof(float) * 4;
		}
		else if (loader.ShadingModel == ::Effekseer::ShadingModelType::Unlit)
		{
		}

		for (auto uniform : loader.Uniforms)
		{
			shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId(uniform.Name.c_str()), psOffset);
			psOffset += sizeof(float) * 4;
		}

		shader->SetPixelConstantBufferSize(psOffset);

		for (auto texture : loader.Textures)
		{
			shader->SetTextureSlot(texture.Index, shader->GetUniformId(texture.Name.c_str()));
		}

		materialData->TextureCount = loader.Textures.size();
		materialData->UniformCount = loader.Uniforms.size();
		materialData->UserPtr = shader;
	}

	{
		auto shaderCode = loader.GenerateShader(EffekseerRenderer::ShaderLoader::ShaderType::Model);

		auto shader = Shader::Create(renderer_,
									 shaderCode.CodeVS.c_str(),
									 shaderCode.CodeVS.size(),
									 shaderCode.CodePS.c_str(),
									 shaderCode.CodePS.size(),
									 "CustomMaterial");

		bool isDebug = false;
#ifdef _DEBUG
		isDebug = true;
#endif

		if (shader == nullptr || isDebug)
		{
			std::cout << shaderCode.CodeVS << std::endl;
			std::cout << shaderCode.CodePS << std::endl;

			if (shader == nullptr)
			{
				return nullptr;
			}
		}

		static ShaderAttribInfo g_model_attribs[ModelRenderer::NumAttribs] = {
			{"a_Position", GL_FLOAT, 3, 0, false},
			{"a_Normal", GL_FLOAT, 3, 12, false},
			{"a_Binormal", GL_FLOAT, 3, 24, false},
			{"a_Tangent", GL_FLOAT, 3, 36, false},
			{"a_TexCoord", GL_FLOAT, 2, 48, false},
			{"a_Color", GL_UNSIGNED_BYTE, 4, 56, true},
#if defined(MODEL_SOFTWARE_INSTANCING)
			{"a_InstanceID", GL_FLOAT, 1, 0, false},
			{"a_UVOffset", GL_FLOAT, 4, 0, false},
			{"a_ModelColor", GL_FLOAT, 4, 0, false},
#endif
		};

		shader->GetAttribIdList(ModelRenderer::NumAttribs, g_model_attribs);
		shader->SetVertexSize(sizeof(::Effekseer::Model::Vertex));

		int32_t vsOffset = 0;
		shader->AddVertexConstantLayout(CONSTANT_TYPE_MATRIX44, shader->GetUniformId("ProjectionMatrix"), 0);
		vsOffset += sizeof(Effekseer::Matrix44);

		shader->AddVertexConstantLayout(CONSTANT_TYPE_MATRIX44, shader->GetUniformId("ModelMatrix"), sizeof(Effekseer::Matrix44));
		vsOffset += sizeof(Effekseer::Matrix44);

		shader->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("UVOffset"), sizeof(Effekseer::Matrix44) * 2);
		vsOffset += sizeof(Effekseer::Matrix44);

		shader->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("ModelColor"), vsOffset);
		vsOffset += sizeof(float) * 4;

		shader->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("mUVInversed"), vsOffset);
		vsOffset += sizeof(float) * 4;

		shader->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("predefined_uniform"), vsOffset);
		vsOffset += sizeof(float) * 4;

		for (auto uniform : loader.Uniforms)
		{
			shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId(uniform.Name.c_str()), vsOffset);
			vsOffset += sizeof(float) * 4;
		}

		shader->SetVertexConstantBufferSize(vsOffset);

		int32_t psOffset = 0;

		shader->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("mUVInversed"), psOffset);
		psOffset += sizeof(float) * 4;

		shader->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("predefined_uniform"), psOffset);
		psOffset += sizeof(float) * 4;

		// shiding model
		if (loader.ShadingModel == ::Effekseer::ShadingModelType::Lit)
		{
			shader->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("cameraPosition"), psOffset);
			psOffset += sizeof(float) * 4;
			shader->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("lightDirection"), psOffset);
			psOffset += sizeof(float) * 4;
			shader->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("lightColor"), psOffset);
			psOffset += sizeof(float) * 4;
			shader->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("lightAmbientColor"), psOffset);
			psOffset += sizeof(float) * 4;
		}
		else if (loader.ShadingModel == ::Effekseer::ShadingModelType::Unlit)
		{
		}

		for (auto uniform : loader.Uniforms)
		{
			shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId(uniform.Name.c_str()), psOffset);
			psOffset += sizeof(float) * 4;
		}

		shader->SetPixelConstantBufferSize(psOffset);

		for (auto texture : loader.Textures)
		{
			shader->SetTextureSlot(texture.Index, shader->GetUniformId(texture.Name.c_str()));
		}

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

} // namespace EffekseerRendererGL