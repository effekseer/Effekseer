#include "EffekseerRendererGL.MaterialLoader.h"
#include "EffekseerRendererGL.ModelRenderer.h"
#include "EffekseerRendererGL.Shader.h"

#include <iostream>

#include "../EffekseerMaterialCompiler/OpenGL/EffekseerMaterialCompilerGL.h"
#include "Effekseer/Material/Effekseer.CompiledMaterial.h"

namespace EffekseerRendererGL
{

::Effekseer::MaterialData* MaterialLoader::LoadAcutually(::Effekseer::Material& material, ::Effekseer::CompiledMaterialBinary* binary)
{
	auto materialData = new ::Effekseer::MaterialData();
	materialData->IsSimpleVertex = material.GetIsSimpleVertex();
	materialData->IsRefractionRequired = material.GetHasRefraction();

	std::array<Effekseer::MaterialShaderType, 2> shaderTypes;
	std::array<Effekseer::MaterialShaderType, 2> shaderTypesModel;

	shaderTypes[0] = Effekseer::MaterialShaderType::Standard;
	shaderTypes[1] = Effekseer::MaterialShaderType::Refraction;
	shaderTypesModel[0] = Effekseer::MaterialShaderType::Model;
	shaderTypesModel[1] = Effekseer::MaterialShaderType::RefractionModel;
	int32_t shaderTypeCount = 1;

	if (material.GetHasRefraction())
	{
		shaderTypeCount = 2;
	}

	for (int32_t st = 0; st < shaderTypeCount; st++)
	{
		auto shader = Shader::Create(renderer_,
									 (const char*)binary->GetVertexShaderData(shaderTypes[st]),
									 binary->GetVertexShaderSize(shaderTypes[st]),
									 (const char*)binary->GetPixelShaderData(shaderTypes[st]),
									 binary->GetPixelShaderSize(shaderTypes[st]),
									 "CustomMaterial");

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

		for (int32_t ui = 0; ui < material.GetUniformCount(); ui++)
		{
			shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId(material.GetUniformName(ui)), vsOffset);
			vsOffset += sizeof(float) * 4;
		}

		shader->SetVertexConstantBufferSize(vsOffset);

		int32_t psOffset = 0;

		shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("mUVInversedBack"), psOffset);
		psOffset += sizeof(float) * 4;

		shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("predefined_uniform"), psOffset);
		psOffset += sizeof(float) * 4;

		// shiding model
		if (material.GetShadingModel() == ::Effekseer::ShadingModelType::Lit)
		{
			shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("cameraPosition"), psOffset);
			psOffset += sizeof(float) * 4;
			shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("lightDirection"), psOffset);
			psOffset += sizeof(float) * 4;
			shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("lightColor"), psOffset);
			psOffset += sizeof(float) * 4;
			shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("lightAmbientColor"), psOffset);
			psOffset += sizeof(float) * 4;
		}
		else if (material.GetShadingModel() == ::Effekseer::ShadingModelType::Unlit)
		{
		}

		if (material.GetHasRefraction() && st == 1)
		{
			shader->AddPixelConstantLayout(CONSTANT_TYPE_MATRIX44, shader->GetUniformId("cameraMat"), psOffset);
			psOffset += sizeof(float) * 16;
		}

		for (int32_t ui = 0; ui < material.GetUniformCount(); ui++)
		{
			shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId(material.GetUniformName(ui)), psOffset);
			psOffset += sizeof(float) * 4;
		}

		shader->SetPixelConstantBufferSize(psOffset);

		int32_t lastIndex = 0;
		for (int32_t ti = 0; ti < material.GetTextureCount(); ti++)
		{
			shader->SetTextureSlot(material.GetTextureIndex(ti), shader->GetUniformId(material.GetTextureName(ti)));
			lastIndex = Effekseer::Max(lastIndex, material.GetTextureIndex(ti));
		}

		lastIndex++;
		shader->SetTextureSlot(lastIndex, shader->GetUniformId("background"));

		materialData->TextureCount = material.GetTextureCount();
		materialData->UniformCount = material.GetUniformCount();

		if (st == 0)
		{
			materialData->UserPtr = shader;
		}
		else
		{
			materialData->RefractionUserPtr = shader;
		}
	}

	for (int32_t st = 0; st < shaderTypeCount; st++)
	{
		auto shader = Shader::Create(renderer_,
									 (const char*)binary->GetVertexShaderData(shaderTypesModel[st]),
									 binary->GetVertexShaderSize(shaderTypesModel[st]),
									 (const char*)binary->GetPixelShaderData(shaderTypesModel[st]),
									 binary->GetPixelShaderSize(shaderTypesModel[st]),
									 "CustomMaterial");

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

		for (int32_t ui = 0; ui < material.GetUniformCount(); ui++)
		{
			shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId(material.GetUniformName(ui)), vsOffset);
			vsOffset += sizeof(float) * 4;
		}

		shader->SetVertexConstantBufferSize(vsOffset);

		int32_t psOffset = 0;

		shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("mUVInversedBack"), psOffset);
		psOffset += sizeof(float) * 4;

		shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("predefined_uniform"), psOffset);
		psOffset += sizeof(float) * 4;

		// shiding model
		if (material.GetShadingModel() == ::Effekseer::ShadingModelType::Lit)
		{
			shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("cameraPosition"), psOffset);
			psOffset += sizeof(float) * 4;
			shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("lightDirection"), psOffset);
			psOffset += sizeof(float) * 4;
			shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("lightColor"), psOffset);
			psOffset += sizeof(float) * 4;
			shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("lightAmbientColor"), psOffset);
			psOffset += sizeof(float) * 4;
		}
		else if (material.GetShadingModel() == ::Effekseer::ShadingModelType::Unlit)
		{
		}

		if (material.GetHasRefraction() && st == 1)
		{
			shader->AddPixelConstantLayout(CONSTANT_TYPE_MATRIX44, shader->GetUniformId("cameraMat"), psOffset);
			psOffset += sizeof(float) * 16;
		}

		for (int32_t ui = 0; ui < material.GetUniformCount(); ui++)
		{
			shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId(material.GetUniformName(ui)), psOffset);
			psOffset += sizeof(float) * 4;
		}

		shader->SetPixelConstantBufferSize(psOffset);

		int32_t lastIndex = 0;
		for (int32_t ti = 0; ti < material.GetTextureCount(); ti++)
		{
			shader->SetTextureSlot(material.GetTextureIndex(ti), shader->GetUniformId(material.GetTextureName(ti)));
			lastIndex = Effekseer::Max(lastIndex, material.GetTextureIndex(ti));
		}

		lastIndex++;
		shader->SetTextureSlot(lastIndex, shader->GetUniformId("background"));

		if (st == 0)
		{
			materialData->ModelUserPtr = shader;
		}
		else
		{
			materialData->RefractionModelUserPtr = shader;
		}
	}

	materialData->TextureCount = material.GetTextureCount();
	materialData->UniformCount = material.GetUniformCount();
	materialData->ShadingModel = material.GetShadingModel();

	return materialData;
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
	// code file
	{
		auto binaryPath = std::u16string(path) + u"d";
		std::unique_ptr<Effekseer::FileReader> reader(fileInterface_->OpenRead(binaryPath.c_str()));

		if (reader.get() != nullptr)
		{
			size_t size = reader->GetLength();
			std::vector<char> data;
			data.resize(size);
			reader->Read(data.data(), size);

			auto material = Load(data.data(), (int32_t)size, ::Effekseer::MaterialFileType::Compiled);

			if (material != nullptr)
			{
				return material;
			}
		}
	}

	// code file
	{
		std::unique_ptr<Effekseer::FileReader> reader(fileInterface_->OpenRead(path));

		if (reader.get() != nullptr)
		{
			size_t size = reader->GetLength();
			std::vector<char> data;
			data.resize(size);
			reader->Read(data.data(), size);

			auto material = Load(data.data(), (int32_t)size, ::Effekseer::MaterialFileType::Code);

			return material;
		}
	}

	return nullptr;
}

::Effekseer::MaterialData* MaterialLoader::Load(const void* data, int32_t size, Effekseer::MaterialFileType fileType)
{
	if (fileType == Effekseer::MaterialFileType::Compiled)
	{
		auto compiled = Effekseer::CompiledMaterial();
		if (!compiled.Load(static_cast<const uint8_t*>(data), size))
		{
			return nullptr;
		}

		if (!compiled.GetHasValue(::Effekseer::CompiledMaterialPlatformType::OpenGL))
		{
			return nullptr;
		}

		// compiled
		Effekseer::Material material;
		material.Load((const uint8_t*)compiled.GetOriginalData().data(), compiled.GetOriginalData().size());
		auto binary = compiled.GetBinary(::Effekseer::CompiledMaterialPlatformType::OpenGL);

		return LoadAcutually(material, binary);
	}
	else
	{
		Effekseer::Material material;
		material.Load((const uint8_t*)data, size);
		auto compiler = ::Effekseer::CreateUniqueReference(new Effekseer::MaterialCompilerGL());
		auto binary = ::Effekseer::CreateUniqueReference(compiler->Compile(&material));

		return LoadAcutually(material, binary.get());
	}
}

void MaterialLoader::Unload(::Effekseer::MaterialData* data)
{
	if (data == nullptr)
		return;
	auto shader = reinterpret_cast<Shader*>(data->UserPtr);
	auto modelShader = reinterpret_cast<Shader*>(data->ModelUserPtr);
	auto refractionShader = reinterpret_cast<Shader*>(data->RefractionUserPtr);
	auto refractionModelShader = reinterpret_cast<Shader*>(data->RefractionModelUserPtr);

	ES_SAFE_DELETE(shader);
	ES_SAFE_DELETE(modelShader);
	ES_SAFE_DELETE(refractionShader);
	ES_SAFE_DELETE(refractionModelShader);
	ES_SAFE_DELETE(data);
}

} // namespace EffekseerRendererGL