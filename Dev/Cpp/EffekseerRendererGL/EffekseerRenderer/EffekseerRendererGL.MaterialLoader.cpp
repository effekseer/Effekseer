#include "EffekseerRendererGL.MaterialLoader.h"
#include "EffekseerRendererGL.ModelRenderer.h"
#include "EffekseerRendererGL.Shader.h"

#include <iostream>

#include "../EffekseerMaterialCompiler/OpenGL/EffekseerMaterialCompilerGL.h"
#include "Effekseer/Material/Effekseer.CompiledMaterial.h"

#undef min

namespace EffekseerRendererGL
{

static const int GL_InstanceCount = 10;

void StoreVertexUniform(const ::Effekseer::MaterialFile& materialFile, const EffekseerRenderer::MaterialShaderParameterGenerator& generator, Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement>& uniformLayout)
{
	using namespace Effekseer::Backend;

	auto storeVector = [&](const char* name, int offset, int count = 1) {
		uniformLayout.emplace_back(UniformLayoutElement{ShaderStageType::Vertex, name, UniformBufferLayoutElementType::Vector4, count, offset});
	};

	auto storeMatrix = [&](const char* name, int offset, int count = 1) {
		uniformLayout.emplace_back(UniformLayoutElement{ShaderStageType::Vertex, name, UniformBufferLayoutElementType::Matrix44, count, offset});
	};

	storeMatrix("uMatCamera", generator.VertexCameraMatrixOffset);
	storeMatrix("uMatProjection", generator.VertexProjectionMatrixOffset);
	storeVector("mUVInversed", generator.VertexInversedFlagOffset);
	storeVector("predefined_uniform", generator.VertexPredefinedOffset);
	storeVector("cameraPosition", generator.VertexCameraPositionOffset);

	for (int32_t ui = 0; ui < materialFile.GetUniformCount(); ui++)
	{
		storeVector(materialFile.GetUniformName(ui), generator.VertexUserUniformOffset + sizeof(float) * 4 * ui);
	}
}

void StoreModelVertexUniform(const ::Effekseer::MaterialFile& materialFile, const EffekseerRenderer::MaterialShaderParameterGenerator& generator, Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement>& uniformLayout, bool instancing)
{
	using namespace Effekseer::Backend;

	auto storeVector = [&](const char* name, int offset, int count = 1) {
		uniformLayout.emplace_back(UniformLayoutElement{ShaderStageType::Vertex, name, UniformBufferLayoutElementType::Vector4, count, offset});
	};

	auto storeMatrix = [&](const char* name, int offset, int count = 1) {
		uniformLayout.emplace_back(UniformLayoutElement{ShaderStageType::Vertex, name, UniformBufferLayoutElementType::Matrix44, count, offset});
	};

	storeMatrix("ProjectionMatrix", generator.VertexProjectionMatrixOffset);

	if (instancing)
	{
		storeMatrix("ModelMatrix", generator.VertexModelMatrixOffset, GL_InstanceCount);

		storeVector("UVOffset", generator.VertexModelUVOffset, GL_InstanceCount);

		storeVector("ModelColor", generator.VertexModelColorOffset, GL_InstanceCount);
	}
	else
	{
		storeMatrix("ModelMatrix", generator.VertexModelMatrixOffset);

		storeVector("UVOffset", generator.VertexModelUVOffset);

		storeVector("ModelColor", generator.VertexModelColorOffset);
	}

	storeVector("mUVInversed", generator.VertexInversedFlagOffset);

	storeVector("predefined_uniform", generator.VertexPredefinedOffset);

	storeVector("cameraPosition", generator.VertexCameraPositionOffset);

	if (instancing)
	{
		if (materialFile.GetCustomData1Count() > 0)
		{
			storeVector("customData1s", generator.VertexModelCustomData1Offset, GL_InstanceCount);
		}

		if (materialFile.GetCustomData2Count() > 0)
		{
			storeVector("customData2s", generator.VertexModelCustomData2Offset, GL_InstanceCount);
		}
	}
	else
	{
		if (materialFile.GetCustomData1Count() > 0)
		{
			storeVector("customData1", generator.VertexModelCustomData1Offset);
		}

		if (materialFile.GetCustomData2Count() > 0)
		{
			storeVector("customData2", generator.VertexModelCustomData2Offset);
		}
	}

	for (int32_t ui = 0; ui < materialFile.GetUniformCount(); ui++)
	{
		storeVector(materialFile.GetUniformName(ui), generator.VertexUserUniformOffset + sizeof(float) * 4 * ui);
	}
}

void StorePixelUniform(const ::Effekseer::MaterialFile& materialFile, const EffekseerRenderer::MaterialShaderParameterGenerator& generator, Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement>& uniformLayout, int shaderType)
{
	using namespace Effekseer::Backend;

	auto storeVector = [&](const char* name, int offset, int count = 1) {
		uniformLayout.emplace_back(UniformLayoutElement{ShaderStageType::Pixel, name, UniformBufferLayoutElementType::Vector4, count, offset});
	};

	auto storeMatrix = [&](const char* name, int offset, int count = 1) {
		uniformLayout.emplace_back(UniformLayoutElement{ShaderStageType::Pixel, name, UniformBufferLayoutElementType::Matrix44, count, offset});
	};

	storeVector("mUVInversedBack", generator.PixelInversedFlagOffset);

	storeVector("predefined_uniform", generator.PixelPredefinedOffset);

	storeVector("cameraPosition", generator.PixelCameraPositionOffset);

	storeVector("reconstructionParam1", generator.PixelReconstructionParam1Offset);

	storeVector("reconstructionParam2", generator.PixelReconstructionParam2Offset);

	// shiding model
	if (materialFile.GetShadingModel() == ::Effekseer::ShadingModelType::Lit)
	{
		storeVector("lightDirection", generator.PixelLightDirectionOffset);
		storeVector("lightColor", generator.PixelLightColorOffset);
		storeVector("lightAmbientColor", generator.PixelLightAmbientColorOffset);
	}
	else if (materialFile.GetShadingModel() == ::Effekseer::ShadingModelType::Unlit)
	{
	}

	if (materialFile.GetHasRefraction() && shaderType == 1)
	{
		storeMatrix("cameraMat", generator.PixelCameraMatrixOffset);
	}

	for (int32_t ui = 0; ui < materialFile.GetUniformCount(); ui++)
	{
		storeVector(materialFile.GetUniformName(ui), generator.PixelUserUniformOffset + sizeof(float) * 4 * ui);
	}
}

Effekseer::CustomVector<Effekseer::CustomString<char>> StoreTextureLocations(const ::Effekseer::MaterialFile& materialFile)
{
	Effekseer::CustomVector<Effekseer::CustomString<char>> texLoc;

	int32_t maxInd = -1;
	for (int32_t ti = 0; ti < materialFile.GetTextureCount(); ti++)
	{
		maxInd = Effekseer::Max(maxInd, materialFile.GetTextureIndex(ti));
	}

	texLoc.resize(maxInd + 1);
	for (int32_t ti = 0; ti < materialFile.GetTextureCount(); ti++)
	{
		texLoc[materialFile.GetTextureIndex(ti)] = materialFile.GetTextureName(ti);
	}

	texLoc.emplace_back("efk_background");
	texLoc.emplace_back("efk_depth");
	return texLoc;
}

::Effekseer::MaterialRef MaterialLoader::LoadAcutually(::Effekseer::MaterialFile& materialFile, ::Effekseer::CompiledMaterialBinary* binary)
{
	auto deviceType = graphicsDevice_->GetDeviceType();

	auto instancing = deviceType == OpenGLDeviceType::OpenGL3 || deviceType == OpenGLDeviceType::OpenGLES3;

	auto material = ::Effekseer::MakeRefPtr<::Effekseer::Material>();
	material->IsSimpleVertex = materialFile.GetIsSimpleVertex();
	material->IsRefractionRequired = materialFile.GetHasRefraction();

	std::array<Effekseer::MaterialShaderType, 2> shaderTypes;
	std::array<Effekseer::MaterialShaderType, 2> shaderTypesModel;

	shaderTypes[0] = Effekseer::MaterialShaderType::Standard;
	shaderTypes[1] = Effekseer::MaterialShaderType::Refraction;
	shaderTypesModel[0] = Effekseer::MaterialShaderType::Model;
	shaderTypesModel[1] = Effekseer::MaterialShaderType::RefractionModel;
	int32_t shaderTypeCount = 1;

	if (materialFile.GetHasRefraction())
	{
		shaderTypeCount = 2;
	}

	for (int32_t st = 0; st < shaderTypeCount; st++)
	{
		auto parameterGenerator = EffekseerRenderer::MaterialShaderParameterGenerator(materialFile, false, st, 1);

		Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement> uniformLayoutElements;
		StoreVertexUniform(materialFile, parameterGenerator, uniformLayoutElements);
		StorePixelUniform(materialFile, parameterGenerator, uniformLayoutElements, st);
		auto uniformLayout = Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(StoreTextureLocations(materialFile), uniformLayoutElements);

		auto shader = Shader::CreateWithHeader(graphicsDevice_, {(const char*)binary->GetVertexShaderData(shaderTypes[st])}, {(const char*)binary->GetPixelShaderData(shaderTypes[st])}, uniformLayout, "CustomMaterial");

		if (shader == nullptr)
		{
			std::cout << "Vertex shader error" << std::endl;
			std::cout << (const char*)binary->GetVertexShaderData(shaderTypesModel[st]) << std::endl;

			std::cout << "Pixel shader error" << std::endl;
			std::cout << (const char*)binary->GetPixelShaderData(shaderTypesModel[st]) << std::endl;

			return nullptr;
		}

		if (material->IsSimpleVertex)
		{
			const Effekseer::Backend::VertexLayoutElement vlElem[3] = {
				{Effekseer::Backend::VertexLayoutFormat::R32G32B32_FLOAT, "atPosition", "POSITION", 0},
				{Effekseer::Backend::VertexLayoutFormat::R8G8B8A8_UNORM, "atColor", "NORMAL", 0},
				{Effekseer::Backend::VertexLayoutFormat::R32G32_FLOAT, "atTexCoord", "TEXCOORD", 0},
			};

			auto vl = graphicsDevice_->CreateVertexLayout(vlElem, 3).DownCast<Backend::VertexLayout>();
			shader->SetVertexLayout(vl);
		}
		else
		{
			Effekseer::Backend::VertexLayoutElement vlElem[8] = {
				{Effekseer::Backend::VertexLayoutFormat::R32G32B32_FLOAT, "atPosition", "POSITION", 0},
				{Effekseer::Backend::VertexLayoutFormat::R8G8B8A8_UNORM, "atColor", "NORMAL", 0},
				{Effekseer::Backend::VertexLayoutFormat::R8G8B8A8_UNORM, "atNormal", "NORMAL", 1},
				{Effekseer::Backend::VertexLayoutFormat::R8G8B8A8_UNORM, "atTangent", "NORMAL", 2},
				{Effekseer::Backend::VertexLayoutFormat::R32G32_FLOAT, "atTexCoord", "TEXCOORD", 0},
				{Effekseer::Backend::VertexLayoutFormat::R32G32_FLOAT, "atTexCoord2", "TEXCOORD", 1},
				{Effekseer::Backend::VertexLayoutFormat::R32G32_FLOAT, "", "TEXCOORD", 2},
				{Effekseer::Backend::VertexLayoutFormat::R32G32_FLOAT, "", "TEXCOORD", 3},
			};

			auto getFormat = [](int32_t i) -> Effekseer::Backend::VertexLayoutFormat {
				if (i == 1)
					return Effekseer::Backend::VertexLayoutFormat::R32_FLOAT;
				if (i == 2)
					return Effekseer::Backend::VertexLayoutFormat::R32G32_FLOAT;
				if (i == 3)
					return Effekseer::Backend::VertexLayoutFormat::R32G32B32_FLOAT;
				if (i == 4)
					return Effekseer::Backend::VertexLayoutFormat::R32G32B32A32_FLOAT;

				assert(0);
				return Effekseer::Backend::VertexLayoutFormat::R32_FLOAT;
			};

			int32_t offset = 40;
			int count = 6;
			int semanticIndex = 2;
			const char* customData1Name = "atCustomData1";
			const char* customData2Name = "atCustomData2";

			if (materialFile.GetCustomData1Count() > 0)
			{
				vlElem[count].Name = customData1Name;
				vlElem[count].Format = getFormat(materialFile.GetCustomData1Count());
				vlElem[count].SemanticIndex = semanticIndex;
				semanticIndex++;

				count++;
				offset += sizeof(float) * materialFile.GetCustomData1Count();
			}

			if (materialFile.GetCustomData2Count() > 0)
			{
				vlElem[count].Name = customData2Name;
				vlElem[count].Format = getFormat(materialFile.GetCustomData2Count());
				vlElem[count].SemanticIndex = semanticIndex;
				semanticIndex++;

				count++;
				offset += sizeof(float) * materialFile.GetCustomData2Count();
			}

			auto vl = graphicsDevice_->CreateVertexLayout(vlElem, count).DownCast<Backend::VertexLayout>();
			shader->SetVertexLayout(vl);
		}

		shader->SetVertexConstantBufferSize(parameterGenerator.VertexShaderUniformBufferSize);
		shader->SetPixelConstantBufferSize(parameterGenerator.PixelShaderUniformBufferSize);

		material->TextureCount = std::min(materialFile.GetTextureCount(), Effekseer::UserTextureSlotMax);
		material->UniformCount = materialFile.GetUniformCount();

		if (st == 0)
		{
			material->UserPtr = shader;
		}
		else
		{
			material->RefractionUserPtr = shader;
		}
	}

	for (int32_t st = 0; st < shaderTypeCount; st++)
	{
		auto parameterGenerator = EffekseerRenderer::MaterialShaderParameterGenerator(materialFile, true, st, instancing ? GL_InstanceCount : 1);
		Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement> uniformLayoutElements;
		StoreModelVertexUniform(materialFile, parameterGenerator, uniformLayoutElements, instancing);
		StorePixelUniform(materialFile, parameterGenerator, uniformLayoutElements, st);
		auto uniformLayout = Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(StoreTextureLocations(materialFile), uniformLayoutElements);

		auto shader = Shader::CreateWithHeader(graphicsDevice_, {(const char*)binary->GetVertexShaderData(shaderTypesModel[st])}, {(const char*)binary->GetPixelShaderData(shaderTypesModel[st])}, uniformLayout, "CustomMaterial");

		if (shader == nullptr)
		{
			std::cout << "Vertex shader error" << std::endl;
			std::cout << (const char*)binary->GetVertexShaderData(shaderTypesModel[st]) << std::endl;

			std::cout << "Pixel shader error" << std::endl;
			std::cout << (const char*)binary->GetPixelShaderData(shaderTypesModel[st]) << std::endl;

			return nullptr;
		}

		const Effekseer::Backend::VertexLayoutElement vlElem[6] = {
			{Effekseer::Backend::VertexLayoutFormat::R32G32B32_FLOAT, "a_Position", "POSITION", 0},
			{Effekseer::Backend::VertexLayoutFormat::R32G32B32_FLOAT, "a_Normal", "NORMAL", 1},
			{Effekseer::Backend::VertexLayoutFormat::R32G32B32_FLOAT, "a_Binormal", "NORMAL", 1},
			{Effekseer::Backend::VertexLayoutFormat::R32G32B32_FLOAT, "a_Tangent", "NORMAL", 2},
			{Effekseer::Backend::VertexLayoutFormat::R32G32_FLOAT, "a_TexCoord", "TEXCOORD", 0},
			{Effekseer::Backend::VertexLayoutFormat::R8G8B8A8_UNORM, "a_Color", "NORMAL", 3},
		};

		auto vl = graphicsDevice_->CreateVertexLayout(vlElem, 6).DownCast<Backend::VertexLayout>();
		shader->SetVertexLayout(vl);

		shader->SetVertexConstantBufferSize(parameterGenerator.VertexShaderUniformBufferSize);
		shader->SetPixelConstantBufferSize(parameterGenerator.PixelShaderUniformBufferSize);

		if (st == 0)
		{
			material->ModelUserPtr = shader;
		}
		else
		{
			material->RefractionModelUserPtr = shader;
		}
	}

	material->CustomData1 = materialFile.GetCustomData1Count();
	material->CustomData2 = materialFile.GetCustomData2Count();
	material->TextureCount = std::min(materialFile.GetTextureCount(), Effekseer::UserTextureSlotMax);
	material->UniformCount = materialFile.GetUniformCount();
	material->ShadingModel = materialFile.GetShadingModel();

	for (int32_t i = 0; i < material->TextureCount; i++)
	{
		material->TextureWrapTypes.at(i) = materialFile.GetTextureWrap(i);
	}

	return material;
}

MaterialLoader::MaterialLoader(Backend::GraphicsDeviceRef graphicsDevice, ::Effekseer::FileInterface* fileInterface, bool canLoadFromCache)
	: fileInterface_(fileInterface)
	, canLoadFromCache_(canLoadFromCache)
{
	if (fileInterface == nullptr)
	{
		fileInterface_ = &defaultFileInterface_;
	}

	graphicsDevice_ = graphicsDevice;
}

MaterialLoader ::~MaterialLoader()
{
}

::Effekseer::MaterialRef MaterialLoader::Load(const char16_t* path)
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

::Effekseer::MaterialRef MaterialLoader::Load(const void* data, int32_t size, Effekseer::MaterialFileType fileType)
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
		Effekseer::MaterialFile materialFile;
		if (!materialFile.Load((const uint8_t*)compiled.GetOriginalData().data(), static_cast<int32_t>(compiled.GetOriginalData().size())))
		{
			std::cout << "Error : Invalid material is loaded." << std::endl;
			return nullptr;
		}

		auto binary = compiled.GetBinary(::Effekseer::CompiledMaterialPlatformType::OpenGL);

		return LoadAcutually(materialFile, binary);
	}
	else
	{
		Effekseer::MaterialFile materialFile;
		if (!materialFile.Load((const uint8_t*)data, size))
		{
			std::cout << "Error : Invalid material is loaded." << std::endl;
			return nullptr;
		}

		auto compiler = ::Effekseer::CreateUniqueReference(new Effekseer::MaterialCompilerGL());
		auto binary = ::Effekseer::CreateUniqueReference(compiler->Compile(&materialFile));

		return LoadAcutually(materialFile, binary.get());
	}
}

void MaterialLoader::Unload(::Effekseer::MaterialRef data)
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

	data->UserPtr = nullptr;
	data->ModelUserPtr = nullptr;
	data->RefractionUserPtr = nullptr;
	data->RefractionModelUserPtr = nullptr;
}

} // namespace EffekseerRendererGL