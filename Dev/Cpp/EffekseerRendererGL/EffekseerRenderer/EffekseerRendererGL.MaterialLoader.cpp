#include "EffekseerRendererGL.MaterialLoader.h"
#include "../EffekseerRendererCommon/EffekseerRenderer.ShaderLoader.h"
#include "EffekseerRendererGL.Shader.h"

#ifdef _DEBUG
#include <iostream>
#endif

namespace EffekseerRendererGL
{

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
	//std::cout << shaderCode << std::endl;
#endif

	auto materialData = new ::Effekseer::MaterialData();

	auto shader = Shader::Create(
		renderer_, g_material_sprite_vs_src, sizeof(g_material_sprite_vs_src), shaderCode.c_str(), shaderCode.size(), "CustomMaterial");

	if (shader == nullptr)
		return nullptr;

	EffekseerRendererGL::ShaderAttribInfo sprite_attribs[3] = {
		{"atPosition", GL_FLOAT, 3, 0, false}, {"atColor", GL_UNSIGNED_BYTE, 4, 12, true}, {"atTexCoord", GL_FLOAT, 2, 16, false}};
	shader->GetAttribIdList(3, sprite_attribs);
	shader->SetVertexSize(sizeof(EffekseerRendererGL::Vertex));

	int32_t vertexUniformSize = sizeof(Effekseer::Matrix44) * 2 + sizeof(float) * 4;

	int32_t pixelUniformSize = sizeof(Effekseer::Matrix44) * 2 + sizeof(float) * 4;

	pixelUniformSize += loader.Uniforms.size() * 4 * sizeof(float);

	shader->SetVertexConstantBufferSize(vertexUniformSize);

	shader->AddVertexConstantLayout(CONSTANT_TYPE_MATRIX44, shader->GetUniformId("uMatCamera"), 0);

	shader->AddVertexConstantLayout(CONSTANT_TYPE_MATRIX44, shader->GetUniformId("uMatProjection"), sizeof(Effekseer::Matrix44));

	shader->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId("mUVInversed"), sizeof(Effekseer::Matrix44) * 2);

	shader->SetPixelConstantBufferSize(pixelUniformSize);

	int32_t index = 0;
	for (auto uniform : loader.Uniforms)
	{
		shader->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shader->GetUniformId(uniform.Name.c_str()), sizeof(float) * 4 * index);	
		index++;
	}

	for (auto texture : loader.Textures)
	{
		shader->SetTextureSlot(texture.Index, shader->GetUniformId(texture.Name.c_str()));
	
	}

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

} // namespace EffekseerRendererGL