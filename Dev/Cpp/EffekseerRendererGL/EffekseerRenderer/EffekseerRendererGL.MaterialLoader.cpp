#include "EffekseerRendererGL.MaterialLoader.h"
#include "EffekseerRendererGL.ModelRenderer.h"
#include "EffekseerRendererGL.Shader.h"

#include <iostream>

#include "../EffekseerMaterialCompiler/OpenGL/EffekseerMaterialCompilerGL.h"
#include "Effekseer/Material/Effekseer.CompiledMaterial.h"
#include "EffekseerRendererGL.DeviceObjectCollection.h"

#undef min

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
		auto parameterGenerator = EffekseerRenderer::MaterialShaderParameterGenerator(material, false, st, 1);

		auto shader = Shader::Create(deviceType_,
									 deviceObjectCollection_,
									 (const char*)binary->GetVertexShaderData(shaderTypes[st]),
									 binary->GetVertexShaderSize(shaderTypes[st]),
									 (const char*)binary->GetPixelShaderData(shaderTypes[st]),
									 binary->GetPixelShaderSize(shaderTypes[st]),
									 "CustomMaterial",
									 true);

		if (shader == nullptr)
		{
			std::cout << "Vertex shader error" << std::endl;
			std::cout << (const char*)binary->GetVertexShaderData(shaderTypesModel[st]) << std::endl;

			std::cout << "Pixel shader error" << std::endl;
			std::cout << (const char*)binary->GetPixelShaderData(shaderTypesModel[st]) << std::endl;

			return nullptr;
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
			EffekseerRendererGL::ShaderAttribInfo sprite_attribs[8] = {
				{"atPosition", GL_FLOAT, 3, 0, false},
				{"atColor", GL_UNSIGNED_BYTE, 4, 12, true},
				{"atNormal", GL_UNSIGNED_BYTE, 4, 16, true},
				{"atTangent", GL_UNSIGNED_BYTE, 4, 20, true},
				{"atTexCoord", GL_FLOAT, 2, 24, false},
				{"atTexCoord2", GL_FLOAT, 2, 32, false},
				{"", GL_FLOAT, 0, 0, false},
				{"", GL_FLOAT, 0, 0, false},
			};

			int32_t offset = 40;
			int count = 6;
			const char* customData1Name = "atCustomData1";
			const char* customData2Name = "atCustomData2";

			if (material.GetCustomData1Count() > 0)
			{
				sprite_attribs[count].name = customData1Name;
				sprite_attribs[count].count = material.GetCustomData1Count();
				sprite_attribs[count].offset = offset;
				count++;
				offset += sizeof(float) * material.GetCustomData1Count();
			}

			if (material.GetCustomData2Count() > 0)
			{
				sprite_attribs[count].name = customData2Name;
				sprite_attribs[count].count = material.GetCustomData2Count();
				sprite_attribs[count].offset = offset;
				count++;
				offset += sizeof(float) * material.GetCustomData2Count();
			}

			shader->GetAttribIdList(count, sprite_attribs);
			shader->SetVertexSize(sizeof(EffekseerRenderer::DynamicVertex) +
								  sizeof(float) * (material.GetCustomData1Count() + material.GetCustomData2Count()));
		}

		shader->AddVertexConstantLayout(
			CONSTANT_TYPE_MATRIX44, shader->GetUniformId("uMatCamera"), parameterGenerator.VertexCameraMatrixOffset);

		shader->AddVertexConstantLayout(
			CONSTANT_TYPE_MATRIX44, shader->GetUniformId("uMatProjection"), parameterGenerator.VertexProjectionMatrixOffset);

		shader->AddVertexConstantLayout(
			CONSTANT_TYPE_VECTOR4, shader->GetUniformId("mUVInversed"), parameterGenerator.VertexInversedFlagOffset);

		shader->AddVertexConstantLayout(
			CONSTANT_TYPE_VECTOR4, shader->GetUniformId("predefined_uniform"), parameterGenerator.VertexPredefinedOffset);

		shader->AddVertexConstantLayout(
			CONSTANT_TYPE_VECTOR4, shader->GetUniformId("cameraPosition"), parameterGenerator.VertexCameraPositionOffset);

		for (int32_t ui = 0; ui < material.GetUniformCount(); ui++)
		{
			shader->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4,
											shader->GetUniformId(material.GetUniformName(ui)),
											parameterGenerator.VertexUserUniformOffset + sizeof(float) * 4 * ui);
		}

		shader->SetVertexConstantBufferSize(parameterGenerator.VertexShaderUniformBufferSize);

		shader->AddPixelConstantLayout(
			CONSTANT_TYPE_VECTOR4, shader->GetUniformId("mUVInversedBack"), parameterGenerator.PixelInversedFlagOffset);

		shader->AddPixelConstantLayout(
			CONSTANT_TYPE_VECTOR4, shader->GetUniformId("predefined_uniform"), parameterGenerator.PixelPredefinedOffset);

		shader->AddPixelConstantLayout(
			CONSTANT_TYPE_VECTOR4, shader->GetUniformId("cameraPosition"), parameterGenerator.PixelCameraPositionOffset);

		// shiding model
		if (material.GetShadingModel() == ::Effekseer::ShadingModelType::Lit)
		{
			shader->AddPixelConstantLayout(
				CONSTANT_TYPE_VECTOR4, shader->GetUniformId("lightDirection"), parameterGenerator.PixelLightDirectionOffset);
			shader->AddPixelConstantLayout(
				CONSTANT_TYPE_VECTOR4, shader->GetUniformId("lightColor"), parameterGenerator.PixelLightColorOffset);
			shader->AddPixelConstantLayout(
				CONSTANT_TYPE_VECTOR4, shader->GetUniformId("lightAmbientColor"), parameterGenerator.PixelLightAmbientColorOffset);
		}
		else if (material.GetShadingModel() == ::Effekseer::ShadingModelType::Unlit)
		{
		}

		if (material.GetHasRefraction() && st == 1)
		{
			shader->AddPixelConstantLayout(
				CONSTANT_TYPE_MATRIX44, shader->GetUniformId("cameraMat"), parameterGenerator.PixelCameraMatrixOffset);
		}

		for (int32_t ui = 0; ui < material.GetUniformCount(); ui++)
		{
			shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4,
										   shader->GetUniformId(material.GetUniformName(ui)),
										   parameterGenerator.PixelUserUniformOffset + sizeof(float) * 4 * ui);
		}

		shader->SetPixelConstantBufferSize(parameterGenerator.PixelShaderUniformBufferSize);

		int32_t lastIndex = -1;
		for (int32_t ti = 0; ti < material.GetTextureCount(); ti++)
		{
			shader->SetTextureSlot(material.GetTextureIndex(ti), shader->GetUniformId(material.GetTextureName(ti)));
			lastIndex = Effekseer::Max(lastIndex, material.GetTextureIndex(ti));
		}

		lastIndex += 1;
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
		auto parameterGenerator = EffekseerRenderer::MaterialShaderParameterGenerator(material, true, st, 1);

		auto shader = Shader::Create(deviceType_,
									 deviceObjectCollection_,
									 (const char*)binary->GetVertexShaderData(shaderTypesModel[st]),
									 binary->GetVertexShaderSize(shaderTypesModel[st]),
									 (const char*)binary->GetPixelShaderData(shaderTypesModel[st]),
									 binary->GetPixelShaderSize(shaderTypesModel[st]),
									 "CustomMaterial",
									 true);

		if (shader == nullptr)
		{
			std::cout << "Vertex shader error" << std::endl;
			std::cout << (const char*)binary->GetVertexShaderData(shaderTypesModel[st]) << std::endl;

			std::cout << "Pixel shader error" << std::endl;
			std::cout << (const char*)binary->GetPixelShaderData(shaderTypesModel[st]) << std::endl;

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

		shader->AddVertexConstantLayout(
			CONSTANT_TYPE_MATRIX44, shader->GetUniformId("ProjectionMatrix"), parameterGenerator.VertexProjectionMatrixOffset);

		shader->AddVertexConstantLayout(
			CONSTANT_TYPE_MATRIX44, shader->GetUniformId("ModelMatrix"), parameterGenerator.VertexModelMatrixOffset);

		shader->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("UVOffset"), parameterGenerator.VertexModelUVOffset);

		shader->AddVertexConstantLayout(
			CONSTANT_TYPE_VECTOR4, shader->GetUniformId("ModelColor"), parameterGenerator.VertexModelColorOffset);

		shader->AddVertexConstantLayout(
			CONSTANT_TYPE_VECTOR4, shader->GetUniformId("mUVInversed"), parameterGenerator.VertexInversedFlagOffset);

		shader->AddVertexConstantLayout(
			CONSTANT_TYPE_VECTOR4, shader->GetUniformId("predefined_uniform"), parameterGenerator.VertexPredefinedOffset);

		shader->AddVertexConstantLayout(
			CONSTANT_TYPE_VECTOR4, shader->GetUniformId("cameraPosition"), parameterGenerator.VertexCameraPositionOffset);

		if (material.GetCustomData1Count() > 0)
		{
			shader->AddVertexConstantLayout(
				CONSTANT_TYPE_VECTOR4, shader->GetUniformId("customData1"), parameterGenerator.VertexModelCustomData1Offset);
		}

		if (material.GetCustomData2Count() > 0)
		{
			shader->AddVertexConstantLayout(
				CONSTANT_TYPE_VECTOR4, shader->GetUniformId("customData2"), parameterGenerator.VertexModelCustomData2Offset);
		}

		for (int32_t ui = 0; ui < material.GetUniformCount(); ui++)
		{
			shader->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4,
											shader->GetUniformId(material.GetUniformName(ui)),
											parameterGenerator.VertexUserUniformOffset + sizeof(float) * 4 * ui);
		}

		shader->SetVertexConstantBufferSize(parameterGenerator.VertexShaderUniformBufferSize);

		shader->AddPixelConstantLayout(
			CONSTANT_TYPE_VECTOR4, shader->GetUniformId("mUVInversedBack"), parameterGenerator.PixelInversedFlagOffset);

		shader->AddPixelConstantLayout(
			CONSTANT_TYPE_VECTOR4, shader->GetUniformId("predefined_uniform"), parameterGenerator.PixelPredefinedOffset);

		shader->AddPixelConstantLayout(
			CONSTANT_TYPE_VECTOR4, shader->GetUniformId("cameraPosition"), parameterGenerator.PixelCameraPositionOffset);

		// shiding model
		if (material.GetShadingModel() == ::Effekseer::ShadingModelType::Lit)
		{
			shader->AddPixelConstantLayout(
				CONSTANT_TYPE_VECTOR4, shader->GetUniformId("lightDirection"), parameterGenerator.PixelLightDirectionOffset);
			shader->AddPixelConstantLayout(
				CONSTANT_TYPE_VECTOR4, shader->GetUniformId("lightColor"), parameterGenerator.PixelLightColorOffset);
			shader->AddPixelConstantLayout(
				CONSTANT_TYPE_VECTOR4, shader->GetUniformId("lightAmbientColor"), parameterGenerator.PixelLightAmbientColorOffset);
		}
		else if (material.GetShadingModel() == ::Effekseer::ShadingModelType::Unlit)
		{
		}

		if (material.GetHasRefraction() && st == 1)
		{
			shader->AddPixelConstantLayout(
				CONSTANT_TYPE_MATRIX44, shader->GetUniformId("cameraMat"), parameterGenerator.PixelCameraMatrixOffset);
		}

		for (int32_t ui = 0; ui < material.GetUniformCount(); ui++)
		{
			shader->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4,
										   shader->GetUniformId(material.GetUniformName(ui)),
										   parameterGenerator.PixelUserUniformOffset + sizeof(float) * 4 * ui);
		}

		shader->SetPixelConstantBufferSize(parameterGenerator.PixelShaderUniformBufferSize);

		int32_t lastIndex = -1;
		for (int32_t ti = 0; ti < material.GetTextureCount(); ti++)
		{
			shader->SetTextureSlot(material.GetTextureIndex(ti), shader->GetUniformId(material.GetTextureName(ti)));
			lastIndex = Effekseer::Max(lastIndex, material.GetTextureIndex(ti));
		}

		lastIndex += 1;
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

	materialData->CustomData1 = material.GetCustomData1Count();
	materialData->CustomData2 = material.GetCustomData2Count();
	materialData->TextureCount = std::min(material.GetTextureCount(), Effekseer::UserTextureSlotMax);
	materialData->UniformCount = material.GetUniformCount();
	materialData->ShadingModel = material.GetShadingModel();

	for (int32_t i = 0; i < materialData->TextureCount; i++)
	{
		materialData->TextureWrapTypes.at(i) = material.GetTextureWrap(i);
	}

	return materialData;
}

MaterialLoader::MaterialLoader(OpenGLDeviceType deviceType,
							   Renderer* renderer,
							   DeviceObjectCollection* deviceObjectCollection,
							   ::Effekseer::FileInterface* fileInterface,
							   bool canLoadFromCache)
	: fileInterface_(fileInterface)
	, canLoadFromCache_(canLoadFromCache)
{
	if (fileInterface == nullptr)
	{
		fileInterface_ = &defaultFileInterface_;
	}

	deviceType_ = deviceType;

	renderer_ = renderer;
	ES_SAFE_ADDREF(renderer_);

	deviceObjectCollection_ = deviceObjectCollection;
	ES_SAFE_ADDREF(deviceObjectCollection_);
}

MaterialLoader ::~MaterialLoader()
{
	ES_SAFE_RELEASE(renderer_);
	ES_SAFE_RELEASE(deviceObjectCollection_);
}

::Effekseer::MaterialData* MaterialLoader::Load(const EFK_CHAR* path)
{
	// code file
	if (canLoadFromCache_)
	{
		auto binaryPath = std::u16string(path) + u"d";
		std::unique_ptr<Effekseer::FileReader> reader(fileInterface_->TryOpenRead(binaryPath.c_str()));

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
		material.Load((const uint8_t*)compiled.GetOriginalData().data(), static_cast<int32_t>(compiled.GetOriginalData().size()));
		auto binary = compiled.GetBinary(::Effekseer::CompiledMaterialPlatformType::OpenGL);

		return LoadAcutually(material, binary);
	}
	else
	{
		Effekseer::Material material;
		if (!material.Load((const uint8_t*)data, size))
		{
			std::cout << "Error : Invalid material is loaded." << std::endl;
		}
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