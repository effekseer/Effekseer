
#include "efkMat.Graphics.h"

#include <OpenGLExtensions.h>

#include <Common/StringHelper.h>
#include <EffekseerRendererCommon/TextureLoader.h>
#include <IO/IO.h>
#include <algorithm>
#include <fstream>
#include <iostream>

#include "../EffekseerMaterialCompiler/OpenGL/EffekseerMaterialCompilerGL.h"
#include <EffekseerRendererCommon/EffekseerRenderer.CommonUtils.h>
#include <EffekseerRendererGL.h>
#include <EffekseerRendererGL/EffekseerRendererGL.MaterialLoader.h>
#include <EffekseerRendererGL/EffekseerRendererGL.Shader.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include "../ThirdParty/tiny_obj_loader.h"

namespace EffekseerMaterial
{

void StoreData(Effekseer::Backend::GraphicsDeviceRef gd, Effekseer::Backend::UniformBufferRef vertexUniformBuffer, Effekseer::Backend::UniformBufferRef pixelUniformBuffer, const Effekseer::Backend::UniformLayoutElement& elm, std::array<float, 4> data)
{
	if (elm.Stage == Effekseer::Backend::ShaderStageType::Vertex)
	{
		if (elm.Type == Effekseer::Backend::UniformBufferLayoutElementType::Vector4)
		{
			gd->UpdateUniformBuffer(vertexUniformBuffer, sizeof(float) * 4, elm.Offset, data.data());
		}
	}

	if (elm.Stage == Effekseer::Backend::ShaderStageType::Pixel)
	{
		if (elm.Type == Effekseer::Backend::UniformBufferLayoutElementType::Vector4)
		{
			gd->UpdateUniformBuffer(pixelUniformBuffer, sizeof(float) * 4, elm.Offset, data.data());
		}
	}
};

void StoreData(Effekseer::Backend::GraphicsDeviceRef gd, Effekseer::Backend::UniformBufferRef vertexUniformBuffer, Effekseer::Backend::UniformBufferRef pixelUniformBuffer, const Effekseer::Backend::UniformLayoutElement& elm, Effekseer::Matrix44 data)
{
	if (elm.Stage == Effekseer::Backend::ShaderStageType::Vertex)
	{
		if (elm.Type == Effekseer::Backend::UniformBufferLayoutElementType::Matrix44)
		{
			gd->UpdateUniformBuffer(vertexUniformBuffer, sizeof(float) * 16, elm.Offset, &data.Values);
		}
	}

	if (elm.Stage == Effekseer::Backend::ShaderStageType::Pixel)
	{
		if (elm.Type == Effekseer::Backend::UniformBufferLayoutElementType::Matrix44)
		{
			gd->UpdateUniformBuffer(pixelUniformBuffer, sizeof(float) * 16, elm.Offset, &data.Values);
		}
	}
};

Effekseer::Gradient ToEffekseerGradient(const Gradient& gradient)
{
	Effekseer::Gradient ret;
	ret.ColorCount = gradient.ColorCount;
	for (size_t i = 0; i < ret.Colors.size(); i++)
	{
		ret.Colors[i].Color = gradient.Colors[i].Color;
		ret.Colors[i].Intensity = gradient.Colors[i].Intensity;
		ret.Colors[i].Position = gradient.Colors[i].Position;
	}

	ret.AlphaCount = gradient.AlphaCount;
	for (size_t i = 0; i < ret.Alphas.size(); i++)
	{
		ret.Alphas[i].Alpha = gradient.Alphas[i].Alpha;
		ret.Alphas[i].Position = gradient.Colors[i].Position;
	}

	return ret;
}

bool Graphics::Initialize(int32_t width, int32_t height)
{
	graphicsDevice_ = EffekseerRendererGL::CreateGraphicsDevice(EffekseerRendererGL::OpenGLDeviceType::OpenGL3, true);
	if (graphicsDevice_ == nullptr)
	{
		return false;
	}

	return true;
}

bool Texture::Validate()
{
	if (path_ == "")
		return false;

	Invalidate();

	auto path16 = Effekseer::Tool::StringHelper::ConvertUtf8ToUtf16(path_);

	auto file = Effekseer::IO::GetInstance()->LoadFile(path16.c_str());
	if (file == nullptr)
	{
		return false;
	}

	auto textureLoader = EffekseerRenderer::CreateTextureLoader(graphics_->GetGraphicsDevice());
	auto texture = textureLoader->Load(file->GetData(), file->GetSize(), Effekseer::TextureType::Color, false);
	if (texture != nullptr)
	{
		texture_ = texture->GetBackend();
	}
	return texture_ != nullptr;
}

void Texture::Invalidate()
{
	if (path_ == "")
		return;
	texture_.Reset();
}

uint64_t Texture::GetInternal()
{
	if (texture_ == nullptr)
		return 0;

	auto prop = EffekseerRendererGL::GetTextureProperty(texture_);
	auto ret = prop.Buffer;

	// Temp
	glBindTexture(GL_TEXTURE_2D, ret);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	return ret;
}

std::shared_ptr<Texture> Texture::Load(std::shared_ptr<Graphics> graphics, const char* path)
{
	auto obj = std::make_shared<Texture>();

	obj->graphics_ = graphics;
	obj->path_ = path;

	if (obj->Validate())
	{
		return obj;
	}

	return nullptr;
}

std::shared_ptr<Texture> Texture::Load(std::shared_ptr<Graphics> graphics, Vector2 size, const void* initialData)
{
	auto obj = std::make_shared<Texture>();

	Effekseer::Backend::TextureParameter texParam;
	texParam.Size[0] = size.X;
	texParam.Size[1] = size.Y;

	Effekseer::CustomVector<uint8_t> data;
	data.resize(size.X * size.Y * 4);

	if (initialData != nullptr)
	{
		memcpy(data.data(), initialData, data.size());
	}

	obj->texture_ = graphics->GetGraphicsDevice()->CreateTexture(texParam, data);

	if (obj->texture_ == nullptr)
	{
		return nullptr;
	}

	return obj;
}

std::map<std::string, std::shared_ptr<Texture>> TextureCache::textures_;

std::shared_ptr<Texture> TextureCache::Load(std::shared_ptr<Graphics> graphics, const char* path)
{
	std::string key = path;

	if (textures_.count(key) > 0)
	{
		return textures_[key];
	}

	auto texture = Texture::Load(graphics, path);
	if (texture != nullptr)
	{
		textures_[key] = texture;
	}

	return texture;
}

void TextureCache::NotifyFileChanged(const char* path)
{
	auto it = textures_.find(path);
	if (it != textures_.end())
	{
		it->second->Invalidate();
		it->second->Validate();
	}
}

std::shared_ptr<Mesh> Mesh::Load(std::shared_ptr<Graphics> graphics, const char* path)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string warn;
	std::string err;

	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path);

	if (!warn.empty())
	{
		std::cout << warn << std::endl;
	}

	if (!err.empty())
	{
		std::cerr << err << std::endl;
		return nullptr;
	}

	if (!ret)
	{
		return nullptr;
	}

	std::vector<Vertex> vertexes;
	std::vector<uint16_t> indexes;

	std::unordered_map<std::tuple<Vector3, Vector3>, std::vector<int32_t>> pairVertexes;

	// only single object
	for (size_t s = 0; s < std::min(static_cast<size_t>(1), shapes.size()); s++)
	{
		size_t index_offset = 0;

		vertexes.resize(shapes[s].mesh.num_face_vertices.size() * 3);
		indexes.resize(shapes[s].mesh.num_face_vertices.size() * 3);

		for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
		{
			for (size_t v = 0; v < shapes[s].mesh.num_face_vertices[f]; v++)
			{
				indexes[index_offset + v] = index_offset + v;

				tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

				vertexes[index_offset + v].Pos.X = attrib.vertices[3 * idx.vertex_index + 0];
				vertexes[index_offset + v].Pos.Y = attrib.vertices[3 * idx.vertex_index + 1];
				vertexes[index_offset + v].Pos.Z = attrib.vertices[3 * idx.vertex_index + 2];

				Vector3 normal;
				normal.X = attrib.normals[3 * idx.normal_index + 0];
				normal.Y = attrib.normals[3 * idx.normal_index + 1];
				normal.Z = attrib.normals[3 * idx.normal_index + 2];
				vertexes[index_offset + v].Normal = normal;

				vertexes[index_offset + v].UV1.X = attrib.texcoords[2 * idx.texcoord_index + 0];
				vertexes[index_offset + v].UV1.Y = 1.0f - attrib.texcoords[2 * idx.texcoord_index + 1];
				vertexes[index_offset + v].Color[0] = attrib.colors[3 * idx.vertex_index + 0] * 255;
				vertexes[index_offset + v].Color[1] = attrib.colors[3 * idx.vertex_index + 1] * 255;
				vertexes[index_offset + v].Color[2] = attrib.colors[3 * idx.vertex_index + 2] * 255;
				vertexes[index_offset + v].Color[3] = 255;

				auto key = std::tuple<Vector3, Vector3>(vertexes[index_offset + v].Pos, normal);
				pairVertexes[key].emplace_back(static_cast<int32_t>(index_offset + v));
			}
			index_offset += shapes[s].mesh.num_face_vertices[f];
		}
	}

	// calc tangent
	std::vector<Vector3> tangents;
	tangents.resize(vertexes.size());
	for (size_t i = 0; i < vertexes.size() / 3; i++)
	{
		auto& v0 = vertexes[i * 3 + 0];
		auto& v1 = vertexes[i * 3 + 1];
		auto& v2 = vertexes[i * 3 + 2];

		Vector3 binormal;
		Vector3 tangent;

		CalcTangentSpace(v0, v1, v2, binormal, tangent);

		tangents[i * 3 + 0] = tangent;
		tangents[i * 3 + 1] = tangent;
		tangents[i * 3 + 2] = tangent;
	}

	for (auto& kv : pairVertexes)
	{
		Vector3 tangentsum = Vector3(0, 0, 0);

		for (auto i : kv.second)
		{
			tangentsum = tangentsum + tangents[i];
		}

		tangentsum = tangentsum / static_cast<float>(kv.second.size());
		tangentsum.Normalize();

		for (auto i : kv.second)
		{
			vertexes[i].Tangent = tangentsum;
			vertexes[i].Binormal = Vector3::Cross(vertexes[i].Normal, vertexes[i].Tangent);
			vertexes[i].Binormal.Normalize();
		}
	}

	auto vb = graphics->GetGraphicsDevice()->CreateVertexBuffer(sizeof(Vertex) * vertexes.size(), vertexes.data(), false);
	auto ib = graphics->GetGraphicsDevice()->CreateIndexBuffer(indexes.size(), indexes.data(), Effekseer::Backend::IndexBufferStrideType::Stride2);

	auto mesh = std::make_shared<Mesh>();
	mesh->vertexBuffer_ = vb;
	mesh->indexBuffer_ = ib;
	mesh->indexCount_ = indexes.size();

	return mesh;
}

bool Preview::Initialize(std::shared_ptr<Graphics> graphics)
{
	std::array<Vertex, 4> vb;
	std::array<uint16_t, 6> ib;

	vb[0].Pos.X = -1.0;
	vb[0].Pos.Y = +1.0;
	vb[0].Pos.Z = 0.0;
	vb[1].Pos.X = +1.0;
	vb[1].Pos.Y = +1.0;
	vb[1].Pos.Z = 0.0;
	vb[2].Pos.X = +1.0;
	vb[2].Pos.Y = -1.0;
	vb[2].Pos.Z = 0.0;
	vb[3].Pos.X = -1.0;
	vb[3].Pos.Y = -1.0;
	vb[3].Pos.Z = 0.0;

	vb[0].UV1.X = 0.0;
	vb[0].UV1.Y = 0.0;
	vb[1].UV1.X = 1.0;
	vb[1].UV1.Y = 0.0;
	vb[2].UV1.X = 1.0;
	vb[2].UV1.Y = 1.0;
	vb[3].UV1.X = 0.0;
	vb[3].UV1.Y = 1.0;

	for (auto& v : vb)
	{
		v.Color.fill(255);
		v.Normal = Vector3(0, 0, 1);
		v.Tangent = Vector3(0, 1, 0);
		v.Binormal = Vector3(1, 0, 0);
	}

	ib[0] = 0;
	ib[1] = 1;
	ib[2] = 2;
	ib[3] = 0;
	ib[4] = 2;
	ib[5] = 3;

	graphics_ = graphics;

	screenVB_ = graphics->GetGraphicsDevice()->CreateVertexBuffer(sizeof(Vertex) * vb.size(), vb.data(), false);
	screenIB_ = graphics->GetGraphicsDevice()->CreateIndexBuffer(ib.size(), ib.data(), Effekseer::Backend::IndexBufferStrideType::Stride2);

	Effekseer::Backend::RenderTextureParameter renderTexParam;
	renderTexParam.Size[0] = TextureSize;
	renderTexParam.Size[1] = TextureSize;
	renderTarget_ = graphics_->GetGraphicsDevice()->CreateRenderTexture(renderTexParam);

	Effekseer::Backend::DepthTextureParameter depthTexParam;
	depthTexParam.Size[0] = TextureSize;
	depthTexParam.Size[1] = TextureSize;
	depthTarget_ = graphics_->GetGraphicsDevice()->CreateDepthTexture(depthTexParam);

	mesh_ = Mesh::Load(graphics_, "resources/meshes/sphere.obj");

	std::array<uint8_t, 4> blackPixels;
	std::array<uint8_t, 4> whitePixels;

	blackPixels.fill(0);
	whitePixels.fill(255);

	black_ = Texture::Load(graphics_, {1, 1}, blackPixels.data());
	white_ = Texture::Load(graphics_, {1, 1}, whitePixels.data());

	Effekseer::FixedSizeVector<Effekseer::Backend::TextureRef, Effekseer::Backend::RenderTargetMax> renderTargets;
	renderTargets.resize(1);
	renderTargets.at(0) = renderTarget_;
	renderPass_ = graphics->GetGraphicsDevice()->CreateRenderPass(renderTargets, depthTarget_);
	return true;
}

bool Preview::UpdateShader(const CompileResult& compileResult)
{
	auto gd = graphics_->GetGraphicsDevice().DownCast<EffekseerRendererGL::Backend::GraphicsDevice>();
	auto vl = EffekseerRenderer::GetMaterialModelVertexLayout(graphics_->GetGraphicsDevice()).DownCast<Effekseer::Backend::VertexLayout>();

	auto compiler = Effekseer::MakeRefPtr<Effekseer::MaterialCompilerGL>();
	auto binary = compiler->Compile(compileResult.materialFile.get());

	auto parameterGenerator = EffekseerRenderer::MaterialShaderParameterGenerator(*compileResult.materialFile, true, 0, 10);
	Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement> uniformLayoutElements;
	EffekseerRendererGL::StoreModelVertexUniform(*compileResult.materialFile, parameterGenerator, uniformLayoutElements, true);
	EffekseerRendererGL::StorePixelUniform(*compileResult.materialFile, parameterGenerator, uniformLayoutElements, 0);
	uniformLayout_ = Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(EffekseerRendererGL::StoreTextureLocations(*compileResult.materialFile), uniformLayoutElements);

	VS = reinterpret_cast<const char*>(binary->GetVertexShaderData(Effekseer::MaterialShaderType::Model));
	PS = reinterpret_cast<const char*>(binary->GetPixelShaderData(Effekseer::MaterialShaderType::Model));

	shader_ = gd->CreateShaderFromCodes(
					{{EffekseerRendererGL::GetVertexShaderHeader(gd->GetDeviceType())}, VS.c_str()},
					{{EffekseerRendererGL::GetFragmentShaderHeader(gd->GetDeviceType())}, PS.c_str()},
					uniformLayout_)
				  .DownCast<EffekseerRendererGL::Backend::Shader>();

	Effekseer::SafeRelease(binary);

	if (shader_ == nullptr)
	{
		return false;
	}

	vertexUniformBuffer_ = gd->CreateUniformBuffer(parameterGenerator.VertexShaderUniformBufferSize, nullptr);
	pixelUniformBuffer_ = gd->CreateUniformBuffer(parameterGenerator.PixelShaderUniformBufferSize, nullptr);

	Effekseer::Backend::PipelineStateParameter pipParam;

	// OpenGL doesn't require it
	pipParam.FrameBufferPtr = nullptr;
	pipParam.VertexLayoutPtr = vl;
	pipParam.ShaderPtr = shader_;
	pipParam.IsDepthTestEnabled = false;
	pipParam.IsDepthWriteEnabled = false;

	pipParam.IsBlendEnabled = true;
	pipParam.BlendSrcFunc = Effekseer::Backend::BlendFuncType::SrcAlpha;
	pipParam.BlendDstFunc = Effekseer::Backend::BlendFuncType::OneMinusSrcAlpha;
	pipParam.BlendSrcFuncAlpha = Effekseer::Backend::BlendFuncType::SrcAlpha;
	pipParam.BlendDstFuncAlpha = Effekseer::Backend::BlendFuncType::OneMinusSrcAlpha;
	pipParam.BlendEquationRGB = Effekseer::Backend::BlendEquationType::Add;
	pipParam.BlendEquationAlpha = Effekseer::Backend::BlendEquationType::Add;
	pipelineState_ = graphics_->GetGraphicsDevice()->CreatePipelineState(pipParam);

	for (const auto& elm : uniformLayout_->GetElements())
	{
		for (const auto& uni : compileResult.uniforms)
		{
			if (uni->UniformName == elm.Name.c_str())
			{
				StoreData(gd, vertexUniformBuffer_, pixelUniformBuffer_, elm, uni->DefaultConstants);
			}
		}

		for (const auto grad : compileResult.gradients)
		{
			auto data = EffekseerRenderer::ToUniform(ToEffekseerGradient(grad->Defaults));

			for (size_t i = 0; i < data.size(); i++)
			{
				if (grad->UniformName + "_" + std::to_string(i) == elm.Name.c_str())
				{
					StoreData(gd, vertexUniformBuffer_, pixelUniformBuffer_, elm, data[i]);
				}
			}
		}
	}

	textures_ = compileResult.textures;

	return true;
}

bool Preview::UpdateUniforms(std::vector<std::shared_ptr<TextureWithSampler>> textures,
							 std::vector<std::shared_ptr<TextExporterUniform>>& uniforms,
							 std::vector<std::shared_ptr<TextExporterGradient>>& gradients)
{
	std::cout << "Update Uniforms" << std::endl;

	if (shader_ == nullptr)
	{
		return false;
	}

	// TODO remove copy and paste
	auto gd = graphics_->GetGraphicsDevice().DownCast<EffekseerRendererGL::Backend::GraphicsDevice>();
	for (const auto& elm : uniformLayout_->GetElements())
	{
		for (const auto& uni : uniforms)
		{
			if (uni->UniformName == elm.Name.c_str())
			{
				StoreData(gd, vertexUniformBuffer_, pixelUniformBuffer_, elm, uni->DefaultConstants);
			}
		}

		for (const auto grad : gradients)
		{
			auto data = EffekseerRenderer::ToUniform(ToEffekseerGradient(grad->Defaults));

			for (size_t i = 0; i < data.size(); i++)
			{
				if (grad->UniformName + "_" + std::to_string(i) == elm.Name.c_str())
				{
					StoreData(gd, vertexUniformBuffer_, pixelUniformBuffer_, elm, data[i]);
				}
			}
		}
	}

	textures_ = textures;

	return true;
}

bool Preview::UpdateConstantValues(float time, std::array<float, 4> customData1, std::array<float, 4> customData2)
{
	if (shader_ == nullptr)
	{
		return false;
	}

	Effekseer::Matrix44 matProj;

	if (ModelType == PreviewModelType::Screen)
	{
		matProj.OrthographicRH(2.0f, 2.0f, 0.1f, 10.0f);
	}
	else
	{
		matProj.PerspectiveFovRH_OpenGL(30.0f / 180.0f * 3.14f, 1.0, 0.1f, 10.0f);
	}

	Effekseer::Matrix44 matCamera;
	matCamera.LookAtRH(Effekseer::Vector3D(0, 0, 2), Effekseer::Vector3D(0, 0, 0), Effekseer::Vector3D(0, 1, 0));

	Effekseer::Matrix44 matCameraProj;

	Effekseer::Matrix44::Mul(matCameraProj, matCamera, matProj);

	auto gd = graphics_->GetGraphicsDevice().DownCast<EffekseerRendererGL::Backend::GraphicsDevice>();

	for (const auto& elm : uniformLayout_->GetElements())
	{
		if (elm.Name == "ProjectionMatrix")
		{
			StoreData(gd, vertexUniformBuffer_, pixelUniformBuffer_, elm, matCameraProj);
		}
		else if (elm.Name == "ModelMatrix")
		{
			Effekseer::Matrix44 matModel;
			matModel.Indentity();
			StoreData(gd, vertexUniformBuffer_, pixelUniformBuffer_, elm, matModel);
		}
		else if (elm.Name == "ModelColor")
		{
			std::array<float, 4> values;
			values.fill(1.0f);
			StoreData(gd, vertexUniformBuffer_, pixelUniformBuffer_, elm, values);
		}
		else if (elm.Name == "UVOffset")
		{
			std::array<float, 4> values;
			values[0] = 0.0f;
			values[1] = 0.0f;
			values[2] = 1.0f;
			values[3] = 1.0f;
			StoreData(gd, vertexUniformBuffer_, pixelUniformBuffer_, elm, values);
		}
		else if (elm.Name == "mUVInversed")
		{
			std::array<float, 4> values;
			values[0] = 1.0f;
			values[1] = -1.0f;
			values[2] = 1.0f;
			values[3] = -1.0f;
			StoreData(gd, vertexUniformBuffer_, pixelUniformBuffer_, elm, values);
		}
		else if (elm.Name == "mUVInversedBack")
		{
			std::array<float, 4> values;
			values[0] = 1.0f;
			values[1] = -1.0f;
			values[2] = 1.0f;
			values[3] = -1.0f;
			StoreData(gd, vertexUniformBuffer_, pixelUniformBuffer_, elm, values);
		}
		else if (elm.Name == "cameraPosition")
		{
			std::array<float, 4> values;
			values[0] = 0.0f;
			values[1] = 0.0f;
			values[2] = 2.0f;
			StoreData(gd, vertexUniformBuffer_, pixelUniformBuffer_, elm, values);
		}
		else if (elm.Name == "reconstructionParam1")
		{
			std::array<float, 4> values;
			values[0] = 0.0f;
			values[1] = 1.0f;
			values[2] = 0.0f;
			values[3] = 1.0f;
			StoreData(gd, vertexUniformBuffer_, pixelUniformBuffer_, elm, values);
		}
		else if (elm.Name == "reconstructionParam2")
		{
			std::array<float, 4> values;
			values[0] = matProj.Values[2][2];
			values[1] = matProj.Values[3][2];
			values[2] = matProj.Values[2][3];
			values[3] = matProj.Values[3][3];
			StoreData(gd, vertexUniformBuffer_, pixelUniformBuffer_, elm, values);
		}
		else if (elm.Name == "lightDirection")
		{
			std::array<float, 4> values;
			values[0] = 1.0f / sqrt(3.0f);
			values[1] = 1.0f / sqrt(3.0f);
			values[2] = 1.0f / sqrt(3.0f);
			values[3] = 1.0f;
			StoreData(gd, vertexUniformBuffer_, pixelUniformBuffer_, elm, values);
		}
		else if (elm.Name == "lightColor")
		{
			std::array<float, 4> values;
			values.fill(1);
			StoreData(gd, vertexUniformBuffer_, pixelUniformBuffer_, elm, values);
		}
		else if (elm.Name == "lightAmbientColor")
		{
			std::array<float, 4> values;
			values.fill(0);
			StoreData(gd, vertexUniformBuffer_, pixelUniformBuffer_, elm, values);
		}
		else if (elm.Name == "predefined_uniform")
		{
			std::array<float, 4> values;
			values[0] = time;
			values[1] = 1.0f;
			values[2] = 0.0f;
			values[3] = time;
			StoreData(gd, vertexUniformBuffer_, pixelUniformBuffer_, elm, values);
		}
		else if (elm.Name == "customData1s")
		{
			StoreData(gd, vertexUniformBuffer_, pixelUniformBuffer_, elm, customData1);
		}
		else if (elm.Name == "customData2s")
		{
			StoreData(gd, vertexUniformBuffer_, pixelUniformBuffer_, elm, customData2);
		}
	}

	return true;
}

void Preview::Render()
{
	auto gd = graphics_->GetGraphicsDevice();

	gd->BeginRenderPass(renderPass_, true, true, Effekseer::Color(0, 0, 0, 0));

	if (shader_ != nullptr)
	{
		Effekseer::Backend::DrawParameter drawParam;

		if (mesh_ != nullptr && ModelType != PreviewModelType::Screen)
		{
			drawParam.VertexBufferPtr = mesh_->GetVertexBuffer();
			drawParam.IndexBufferPtr = mesh_->GetIndexBuffer();
			drawParam.IndexOffset = 0;
			drawParam.InstanceCount = 1;
			drawParam.PrimitiveCount = mesh_->GetIndexCount() / 3;
			drawParam.PipelineStatePtr = pipelineState_;
			drawParam.VertexUniformBufferPtrs[0] = vertexUniformBuffer_;
			drawParam.PixelUniformBufferPtrs[0] = pixelUniformBuffer_;
		}
		else
		{
			drawParam.VertexBufferPtr = screenVB_;
			drawParam.IndexBufferPtr = screenIB_;
			drawParam.IndexOffset = 0;
			drawParam.InstanceCount = 1;
			drawParam.PrimitiveCount = 2;
			drawParam.PipelineStatePtr = pipelineState_;
			drawParam.VertexUniformBufferPtrs[0] = vertexUniformBuffer_;
			drawParam.PixelUniformBufferPtrs[0] = pixelUniformBuffer_;
		}

		for (int i = 0; i < textures_.size(); i++)
		{
			for (size_t j = 0; j < uniformLayout_->GetTextures().size(); j++)
			{
				if (textures_[i]->Name == uniformLayout_->GetTextures()[j].c_str())
				{
					Effekseer::Backend::TextureRef texture;
					if (textures_[i]->TexturePtr != nullptr)
					{
						texture = textures_[i]->TexturePtr->GetTexture();
					}
					else
					{
						texture = black_->GetTexture();
					}

					drawParam.SetTexture(j, texture, textures_[i]->SamplerType == TextureSamplerType::Repeat ? Effekseer::Backend::TextureWrapType::Repeat : Effekseer::Backend::TextureWrapType::Clamp, Effekseer::Backend::TextureSamplingType::Linear);
				}
			}
		}

		for (size_t j = 0; j < uniformLayout_->GetTextures().size(); j++)
		{
			if (uniformLayout_->GetTextures()[j] == "efk_depth")
			{
				drawParam.SetTexture(j, white_->GetTexture(), Effekseer::Backend::TextureWrapType::Clamp, Effekseer::Backend::TextureSamplingType::Linear);
			}
		}

		gd->Draw(drawParam);
	}

	gd->EndRenderPass();
}

uint64_t Preview::GetInternal()
{
	if (renderTarget_ == nullptr)
		return 0;

	auto prop = EffekseerRendererGL::GetTextureProperty(renderTarget_);
	auto ret = prop.Buffer;

	// Temp
	glBindTexture(GL_TEXTURE_2D, ret);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	return ret;
}

} // namespace EffekseerMaterial
