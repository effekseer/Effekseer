#include "EffekseerRendererGL.MaterialLoader.h"
#include "../EffekseerRendererCommon/EffekseerRenderer.ShaderLoader.h"
#include "EffekseerRendererGL.ModelRenderer.h"
#include "EffekseerRendererGL.Shader.h"

#include <iostream>

namespace EffekseerRendererGL
{

static const char g_model_vs_src[] =
	R"(
IN vec4 a_Position;
IN vec4 a_Normal;
IN vec4 a_Binormal;
IN vec4 a_Tangent;
IN vec4 a_TexCoord;
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
OUT mediump vec4 v_Normal;
OUT mediump vec4 v_Binormal;
OUT mediump vec4 v_Tangent;
OUT mediump vec4 v_TexCoord;
OUT lowp vec4 v_Color;
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
uniform vec4 LightDirection;
uniform vec4 LightColor;
uniform vec4 LightAmbient;
uniform vec4 mUVInversed;

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
	vec4 localPosition = modelMatrix * a_Position;
	gl_Position = ProjectionMatrix * localPosition;

	v_TexCoord.xy = a_TexCoord.xy * uvOffset.zw + uvOffset.xy;
	v_Normal = vec4(normalize( (modelMatrix * (a_Normal + a_Position) - localPosition).xyz ), 1.0);
	v_Binormal = vec4(normalize( (modelMatrix * (a_Binormal + a_Position) - localPosition).xyz ), 1.0);
	v_Tangent = vec4(normalize( (modelMatrix * (a_Tangent + a_Position) - localPosition).xyz ), 1.0);
	v_Color = modelColor;
	v_TexCoord.y = mUVInversed.x + mUVInversed.y * v_TexCoord.y;
}
)";

static const char g_model_fs_src_pre[] =
	R"(

IN mediump vec4 v_Normal;
IN mediump vec4 v_Binormal;
IN mediump vec4 v_Tangent;
IN mediump vec4 v_TexCoord;
IN lowp vec4 v_Color;

uniform vec4 LightDirection;
uniform vec4 LightColor;
uniform vec4 LightAmbient;
uniform vec4 predefined_uniform;


)";

static const char g_model_fs_src_suf[] =
	R"(

void main()
{
	FRAGCOLOR = Calculate();
}

)";

static const char g_material_sprite_vs_src[] =
	R"(
IN vec4 atPosition;
IN vec4 atColor;
IN vec4 atTexCoord;
)"

	R"(
OUT vec4 vaColor;
OUT vec4 vaTexCoord;
OUT vec4 vaPos;
OUT vec4 vaPosR;
OUT vec4 vaPosU;
)"

	R"(
uniform mat4 uMatCamera;
uniform mat4 uMatProjection;
uniform vec4 mUVInversed;

void main() {
	vec4 cameraPos = uMatCamera * atPosition;
	cameraPos = cameraPos / cameraPos.w;

	gl_Position = uMatProjection * cameraPos;

	vaPos = gl_Position;

	vec4 cameraPosU = cameraPos + vec4(0.0, 1.0, 0.0, 0.0);
	vec4 cameraPosR = cameraPos + vec4(1.0, 0.0, 0.0, 0.0);

	vaPosR = uMatProjection * cameraPosR;
	vaPosU = uMatProjection * cameraPosU;
	
	vaPos = vaPos / vaPos.w;
	vaPosR = vaPosR / vaPosR.w;
	vaPosU = vaPosU / vaPosU.w;

	vaColor = atColor;
	vaTexCoord = atTexCoord;

	vaTexCoord.y = mUVInversed.x + mUVInversed.y * vaTexCoord.y;
}

)";

static const char g_sprite_fs_src_pre[] =
	R"(
IN lowp vec4 vaColor;
IN mediump vec4 vaTexCoord;

uniform vec4 predefined_uniform;

)";

static const char g_sprite_fs_src_suf[] =
	R"(

void main()
{
	FRAGCOLOR = Calculate();
}

)";

class ShaderLoader : public EffekseerRenderer::ShaderLoader
{
public:
	ShaderLoader() = default;
	virtual ~ShaderLoader() = default;

	virtual std::string GenerateShader(ShaderType shaderType)
	{
		std::ostringstream maincode;

		if (shaderType == ShaderType::Standard)
		{
			maincode << g_sprite_fs_src_pre;
		}
		else if (shaderType == ShaderType::Model)
		{
			maincode << g_model_fs_src_pre;
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

		auto baseCode = GenericCode;
		baseCode = Replace(baseCode, "$F1$", "float");
		baseCode = Replace(baseCode, "$F2$", "vec2");
		baseCode = Replace(baseCode, "$F3$", "vec3");
		baseCode = Replace(baseCode, "$F4$", "vec4");
		baseCode = Replace(baseCode, "$TIME$", "predefined_uniform.x");
		baseCode = Replace(baseCode, "$UV$", "vaTexCoord.xy");
		baseCode = Replace(baseCode, "$INPUT$", "");

		// replace textures
		for (size_t i = 0; i < Textures.size(); i++)
		{
			auto& texture = Textures[i];
			std::string keyP = "$TEX_P" + std::to_string(texture.Index) + "$";
			std::string keyS = "$TEX_S" + std::to_string(texture.Index) + "$";

			baseCode = Replace(baseCode, keyP, "TEX2D(" + texture.Name + ",");
			baseCode = Replace(baseCode, keyS, ")");
		}

		maincode << baseCode;

		if (shaderType == ShaderType::Standard)
		{
			maincode << g_sprite_fs_src_suf;
		}
		else if (shaderType == ShaderType::Model)
		{
			maincode << g_model_fs_src_suf;
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

		auto shader = Shader::Create(
			renderer_, g_material_sprite_vs_src, sizeof(g_material_sprite_vs_src), shaderCode.c_str(), shaderCode.size(), "CustomMaterial");

		if (shader == nullptr)
			return nullptr;

		EffekseerRendererGL::ShaderAttribInfo sprite_attribs[3] = {
			{"atPosition", GL_FLOAT, 3, 0, false}, {"atColor", GL_UNSIGNED_BYTE, 4, 12, true}, {"atTexCoord", GL_FLOAT, 2, 16, false}};
		shader->GetAttribIdList(3, sprite_attribs);
		shader->SetVertexSize(sizeof(EffekseerRendererGL::Vertex));

		int32_t vertexUniformSize = sizeof(Effekseer::Matrix44) * 2 + sizeof(float) * 4;

		int32_t pixelUniformSize = sizeof(float) * 4;

		pixelUniformSize += loader.Uniforms.size() * 4 * sizeof(float);

		shader->SetVertexConstantBufferSize(vertexUniformSize);

		shader->AddVertexConstantLayout(CONSTANT_TYPE_MATRIX44, shader->GetUniformId("uMatCamera"), 0);

		shader->AddVertexConstantLayout(CONSTANT_TYPE_MATRIX44, shader->GetUniformId("uMatProjection"), sizeof(Effekseer::Matrix44));

		shader->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("mUVInversed"), sizeof(Effekseer::Matrix44) * 2);

		shader->SetPixelConstantBufferSize(pixelUniformSize);

		int32_t index = 0;
		shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("predefined_uniform"), sizeof(float) * 4 * index);
		index++;

		for (auto uniform : loader.Uniforms)
		{
			shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId(uniform.Name.c_str()), sizeof(float) * 4 * index);
			index++;
		}

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

		auto shader =
			Shader::Create(renderer_, g_model_vs_src, sizeof(g_model_vs_src), shaderCode.c_str(), shaderCode.size(), "CustomMaterial");

		if (shader == nullptr)
		{
			std::cout << shaderCode << std::endl;
			return nullptr;
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

		int32_t vertexUniformSize = sizeof(::EffekseerRenderer::ModelRendererVertexConstantBuffer<1>);

		int32_t pixelUniformSize = sizeof(float) * 4 * 4;

		pixelUniformSize += loader.Uniforms.size() * 4 * sizeof(float);

		shader->SetVertexConstantBufferSize(vertexUniformSize);

		shader->AddVertexConstantLayout(CONSTANT_TYPE_MATRIX44, shader->GetUniformId("ProjectionMatrix"), 0);

		shader->AddVertexConstantLayout(CONSTANT_TYPE_MATRIX44, shader->GetUniformId("ModelMatrix"), sizeof(Effekseer::Matrix44));

		shader->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("UVOffset"), sizeof(Effekseer::Matrix44) * 2);

		shader->AddVertexConstantLayout(
			CONSTANT_TYPE_VECTOR4, shader->GetUniformId("ModelColor"), sizeof(Effekseer::Matrix44) * 2 + sizeof(float[4]) * 1);

		shader->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4,
										shader->GetUniformId("LightDirection"),
										sizeof(Effekseer::Matrix44) * 2 + sizeof(float[4]) * 2 + sizeof(float[4]) * 0);
		shader->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4,
										shader->GetUniformId("LightColor"),
										sizeof(Effekseer::Matrix44) * 2 + sizeof(float[4]) * 2 + sizeof(float[4]) * 1);
		shader->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4,
										shader->GetUniformId("LightAmbient"),
										sizeof(Effekseer::Matrix44) * 2 + sizeof(float[4]) * 2 + sizeof(float[4]) * 2);
		shader->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4,
										shader->GetUniformId("mUVInversed"),
										sizeof(Effekseer::Matrix44) * 2 + sizeof(float[4]) * 2 + sizeof(float[4]) * 3);

		shader->SetPixelConstantBufferSize(sizeof(::EffekseerRenderer::ModelRendererPixelConstantBuffer));
		shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("LightDirection"), sizeof(float[4]) * 0);
		shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("LightColor"), sizeof(float[4]) * 1);
		shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("LightAmbient"), sizeof(float[4]) * 2);

		shader->SetPixelConstantBufferSize(pixelUniformSize);

		int32_t index = 0;
		shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("predefined_uniform"), sizeof(float) * 4 * index);
		index++;

		for (auto uniform : loader.Uniforms)
		{
			shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId(uniform.Name.c_str()), sizeof(float) * 4 * index);
			index++;
		}

		for (auto texture : loader.Textures)
		{
			shader->SetTextureSlot(texture.Index, shader->GetUniformId(texture.Name.c_str()));
		}

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

} // namespace EffekseerRendererGL