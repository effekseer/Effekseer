
#include "efkMat.Graphics.h"

#ifdef __APPLE__
#include <OpenGL/gl.h>
#elif defined(_WIN32)
#include <GL/glew.h>
#else
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#endif

#include <Common/StringHelper.h>
#include <IO/IO.h>
#include <algorithm>
#include <fstream>
#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION
#include "../ThirdParty/tiny_obj_loader.h"

namespace EffekseerMaterial
{

bool Graphics::Initialize(int32_t width, int32_t height)
{
	manager = ar::Manager::Create(ar::GraphicsDeviceType::OpenGL);

	ar::ManagerInitializationParameter param;
	param.WindowWidth = width;
	param.WindowHeight = height;

	if (manager->Initialize(param) != ar::ErrorCode::OK)
	{
		ar::SafeDelete(manager);
		return false;
	}

	compiler = ar::Compiler::Create(manager);

	return true;
}

Graphics::Graphics() {}

Graphics::~Graphics()
{
	ar::SafeDelete(compiler);
	ar::SafeDelete(manager);
}

Texture::Texture() {}

Texture::~Texture() { ar::SafeDelete(texture_); }

bool Texture::Validate()
{
	if (path_ == "")
		return false;

	Invalidate();

	auto path16 = Effekseer::utf8_to_utf16(path_);

	auto file = Effekseer::IO::GetInstance()->LoadFile(path16.c_str());
	if (file == nullptr)
	{
		return false;
	}

	texture_ = ar::Texture2D::Create(graphics_->GetManager());
	if (texture_ == nullptr)
	{
		return false;
	}

	if (!texture_->Initialize(graphics_->GetManager(), file->GetData(), file->GetSize(), false, false))
	{
		Invalidate();
		return false;
	}

	return true;
}

void Texture::Invalidate() {
	if (path_ == "")
		return;

	ar::SafeDelete(texture_); 
}

uint64_t Texture::GetInternal()
{
	if (texture_ == nullptr)
		return 0;
	auto ret = texture_->GetInternalObjects()[0];

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

	obj->texture_ = ar::Texture2D::Create(graphics->GetManager());
	if (obj->texture_ == nullptr)
	{
		return nullptr;
	}

	if (!obj->texture_->Initialize(graphics->GetManager(), size.X, size.Y, ar::TextureFormat::R8G8B8A8_UNORM, (void*)initialData, false))
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

Mesh::Mesh() {}

Mesh ::~Mesh()
{
	ar::SafeDelete(vb);
	ar::SafeDelete(ib);
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
				vertexes[index_offset + v].Normal = Vertex::CreatePacked(normal);

				vertexes[index_offset + v].UV1.X = attrib.texcoords[2 * idx.texcoord_index + 0];
				vertexes[index_offset + v].UV1.Y = 1.0f - attrib.texcoords[2 * idx.texcoord_index + 1];
				vertexes[index_offset + v].UV2.X = attrib.texcoords[2 * idx.texcoord_index + 0];
				vertexes[index_offset + v].UV2.Y = 1.0f - attrib.texcoords[2 * idx.texcoord_index + 1];
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

		for (auto i : kv.second)
		{
			vertexes[i].Tangent = Vertex::CreatePacked(tangentsum);
		}
	}

	auto vb = ar::VertexBuffer::Create(graphics->GetManager());
	vb->Initialize(graphics->GetManager(), sizeof(Vertex), vertexes.size(), false);

	auto ib = ar::IndexBuffer::Create(graphics->GetManager());
	ib->Initialize(graphics->GetManager(), indexes.size(), false);

	if (!vb->Write(vertexes.data(), vertexes.size() * sizeof(Vertex)))
	{
		assert(0);
	}

	if (!ib->Write(indexes.data(), indexes.size() * sizeof(int16_t)))
	{
		assert(0);
	}

	auto mesh = std::make_shared<Mesh>();

	mesh->vb = vb;
	mesh->ib = ib;
	mesh->indexCount = indexes.size();

	return mesh;
}

Preview::Preview() {}

Preview::~Preview()
{
	ar::SafeDelete(vb);
	ar::SafeDelete(ib);
	ar::SafeDelete(shader);
	ar::SafeDelete(renderTarget_);
	ar::SafeDelete(depthTarget_);
	ar::SafeDelete(context);
	ar::SafeDelete(constantBuffer);
}

bool Preview::Initialize(std::shared_ptr<Graphics> graphics)
{
	graphics_ = graphics;
	vb = ar::VertexBuffer::Create(graphics->GetManager());
	vb->Initialize(graphics->GetManager(), sizeof(Vertex), 4, false);

	ib = ar::IndexBuffer::Create(graphics->GetManager());
	ib->Initialize(graphics->GetManager(), 6, false);

	Vertex vb_[4];
	uint16_t ib_[6];

	vb_[0].Pos.X = -1.0;
	vb_[0].Pos.Y = +1.0;
	vb_[0].Pos.Z = 0.5;
	vb_[1].Pos.X = +1.0;
	vb_[1].Pos.Y = +1.0;
	vb_[1].Pos.Z = 0.5;
	vb_[2].Pos.X = +1.0;
	vb_[2].Pos.Y = -1.0;
	vb_[2].Pos.Z = 0.5;
	vb_[3].Pos.X = -1.0;
	vb_[3].Pos.Y = -1.0;
	vb_[3].Pos.Z = 0.5;

	vb_[0].UV1.X = 0.0;
	vb_[0].UV1.Y = 0.0;
	vb_[1].UV1.X = 1.0;
	vb_[1].UV1.Y = 0.0;
	vb_[2].UV1.X = 1.0;
	vb_[2].UV1.Y = 1.0;
	vb_[3].UV1.X = 0.0;
	vb_[3].UV1.Y = 1.0;

	vb_[0].Color[0] = 255;
	vb_[0].Color[1] = 255;
	vb_[0].Color[2] = 255;
	vb_[0].Color[3] = 255;

	vb_[1].Color[0] = 255;
	vb_[1].Color[1] = 255;
	vb_[1].Color[2] = 255;
	vb_[1].Color[3] = 255;

	vb_[2].Color[0] = 255;
	vb_[2].Color[1] = 255;
	vb_[2].Color[2] = 255;
	vb_[2].Color[3] = 255;

	vb_[3].Color[0] = 255;
	vb_[3].Color[1] = 255;
	vb_[3].Color[2] = 255;
	vb_[3].Color[3] = 255;

	vb->Write(vb_, sizeof(Vertex) * 4);

	ib_[0] = 0;
	ib_[1] = 1;
	ib_[2] = 2;
	ib_[3] = 0;
	ib_[4] = 2;
	ib_[5] = 3;

	ib->Write(ib_, sizeof(uint16_t) * 6);

	renderTarget_ = ar::RenderTexture2D::Create(graphics->GetManager());
	renderTarget_->Initialize(graphics->GetManager(), TextureSize, TextureSize, ar::TextureFormat::R8G8B8A8_UNORM);

	depthTarget_ = ar::DepthTexture::Create(graphics->GetManager());
	depthTarget_->Initialize(graphics->GetManager(), TextureSize, TextureSize);

	context = ar::Context::Create(graphics_->GetManager());
	context->Initialize(graphics_->GetManager());

	mesh_ = Mesh::Load(graphics_, "resources/meshes/sphere.obj");

	std::array<uint8_t, 4> blackPixels;
	std::array<uint8_t, 4> whitePixels;

	blackPixels.fill(0);
	whitePixels.fill(255);

	black_ = Texture::Load(graphics_, {1, 1}, blackPixels.data());
	white_ = Texture::Load(graphics_, {1, 1}, whitePixels.data());

	return true;
}

static const char g_header_vs_gl3_src[] = ""
										  "#define lowp\n"
										  "#define mediump\n"
										  "#define highp\n"
										  "#define IN in\n"
										  "#define CENTROID centroid\n"
										  "#define TEX2D textureLod\n"
										  "#define OUT out\n"
										  "uniform vec4 customData1;\n"	 // HACK
										  "uniform vec4 customData2;\n"; // HACK

static const char g_header_fs_gl3_src[] = ""
										  "#define lowp\n"
										  "#define mediump\n"
										  "#define highp\n"
										  "#define IN in\n"
										  "#define CENTROID centroid\n"
										  "#define TEX2D texture\n"
										  "layout (location = 0) out vec4 FRAGCOLOR;\n"
										  "uniform vec4 customData1;\n"	 // HACK
										  "uniform vec4 customData2;\n"; // HACK

bool Preview::CompileShader(std::string& vs,
							std::string& ps,
							std::vector<std::shared_ptr<TextureWithSampler>> textures,
							std::vector<std::shared_ptr<TextExporterUniform>>& uniforms)
{
	VS = vs;
	PS = ps;

	// std::cout << "Compile Shader" << std::endl;
	// std::cout << PS << std::endl;

	ar::SafeDelete(shader);
	ar::SafeDelete(constantBuffer);

	ar::ShaderCompilerParameter param;
	param.VertexShaderTexts.push_back(g_header_vs_gl3_src);
	param.VertexShaderTexts.push_back(vs);
	param.PixelShaderTexts.push_back(g_header_fs_gl3_src);
	param.PixelShaderTexts.push_back(ps);
	param.OpenGLVersion = ar::OpenGLVersionType::OpenGL33;
	ar::ShaderCompilerResult result;

	if (!graphics_->GetCompiler()->Compile(result, param))
	{
		std::cout << "Compile Error : " << std::endl;
		std::cout << result.ErrorMessage << std::endl;
		return false;
	}

	shader = ar::Shader::Create(graphics_->GetManager());
	std::vector<ar::VertexLayout> layout;
	layout.resize(6);
	layout[0].Name = "atPosition";
	layout[0].LayoutFormat = ar::VertexLayoutFormat::R32G32B32_FLOAT;
	layout[1].Name = "atTexCoord";
	layout[1].LayoutFormat = ar::VertexLayoutFormat::R32G32_FLOAT;
	layout[2].Name = "atTexCoord2";
	layout[2].LayoutFormat = ar::VertexLayoutFormat::R32G32_FLOAT;
	layout[3].Name = "atNormal";
	layout[3].LayoutFormat = ar::VertexLayoutFormat::R8G8B8A8_UNORM;
	layout[4].Name = "atTangent";
	layout[4].LayoutFormat = ar::VertexLayoutFormat::R8G8B8A8_UNORM;
	layout[5].Name = "atColor";
	layout[5].LayoutFormat = ar::VertexLayoutFormat::R8G8B8A8_UNORM;

	shader->Initialize(graphics_->GetManager(), result, layout, false);

	constantBuffer = ar::ConstantBuffer::Create(graphics_->GetManager());
	constantBuffer->Initialize(graphics_->GetManager(), shader->GetPixelConstantBufferSize());

	for (auto layout : shader->GetPixelConstantLayouts())
	{
		for (auto uni : uniforms)
		{
			if (uni->UniformName == layout.first)
			{
				constantBuffer->SetData(uni->DefaultConstants.data(), layout.second.GetSize(), layout.second.Offset);
				break;
			}
		}
	}

	textures_ = textures;

	return true;
}

bool Preview::UpdateUniforms(std::vector<std::shared_ptr<TextureWithSampler>> textures,
							 std::vector<std::shared_ptr<TextExporterUniform>>& uniforms)
{
	std::cout << "Update Uniforms" << std::endl;

	if (shader == nullptr)
		return false;

	for (auto layout : shader->GetPixelConstantLayouts())
	{
		for (auto uni : uniforms)
		{
			if (uni->UniformName == layout.first)
			{
				constantBuffer->SetData(uni->DefaultConstants.data(), layout.second.GetSize(), layout.second.Offset);
				break;
			}
		}
	}

	textures_ = textures;

	return true;
}

bool Preview::UpdateConstantValues(float time, std::array<float, 4> customData1, std::array<float, 4> customData2)
{
	if (shader == nullptr)
		return false;

	Matrix44 matProj;

	if (ModelType == PreviewModelType::Screen)
	{
		matProj.OrthographicRH(2.0f, 2.0f, 0.1f, 10.0f);	
	}
	else
	{
		matProj.SetPerspectiveFovRH_OpenGL(30.0f / 180.0f * 3.14f, 1.0, 0.1f, 10.0f);
	}

	for (auto layout : shader->GetPixelConstantLayouts())
	{
		if (layout.first == "uMatProjection")
		{
			constantBuffer->SetData(matProj.Values, layout.second.GetSize(), layout.second.Offset);
		}

		if (layout.first == "uMatCamera")
		{
			Matrix44 mat;
			mat.SetLookAtRH(Vector3(0, 0, 2), Vector3(0, 0, 0), Vector3(0, 1, 0));

			constantBuffer->SetData(mat.Values, layout.second.GetSize(), layout.second.Offset);
		}

		if (layout.first == "mUVInversed")
		{
			float values[4];
			values[0] = 1.0f;
			values[1] = -1.0f;
			values[2] = 1.0f;
			values[3] = -1.0f;

			constantBuffer->SetData(values, layout.second.GetSize(), layout.second.Offset);
		}

		if (layout.first == "mUVInversedBack")
		{
			float values[4];
			values[0] = 1.0f;
			values[1] = -1.0f;
			values[2] = 1.0f;
			values[3] = -1.0f;

			constantBuffer->SetData(values, layout.second.GetSize(), layout.second.Offset);
		}

		if (layout.first == "cameraPosition")
		{
			float values[4];
			values[0] = 0.0f;
			values[1] = 0.0f;
			values[2] = 2.0f;

			constantBuffer->SetData(values, layout.second.GetSize(), layout.second.Offset);
		}

		if (layout.first == "reconstructionParam1")
		{			
			float values[4];
			values[0] = 0.0f;
			values[1] = 1.0f;
			values[2] = 0.0f;
			values[3] = 1.0f;

			constantBuffer->SetData(values, layout.second.GetSize(), layout.second.Offset);
		}

		if (layout.first == "reconstructionParam2")
		{
			float values[4];
			values[0] = matProj.Values[2][2];
			values[1] = matProj.Values[3][2];
			values[2] = matProj.Values[2][3];
			values[3] = matProj.Values[3][3];

			constantBuffer->SetData(values, layout.second.GetSize(), layout.second.Offset);
		}

		if (layout.first == "lightDirection")
		{
			float values[4];
			values[0] = 1.0f / sqrt(3.0f);
			values[1] = 1.0f / sqrt(3.0f);
			values[2] = 1.0f / sqrt(3.0f);

			constantBuffer->SetData(values, layout.second.GetSize(), layout.second.Offset);
		}

		if (layout.first == "lightColor")
		{
			float values[4];
			values[0] = 1.0f;
			values[1] = 1.0f;
			values[2] = 1.0f;

			constantBuffer->SetData(values, layout.second.GetSize(), layout.second.Offset);
		}

		if (layout.first == "lightAmbientColor")
		{
			float values[4];
			values[0] = 0.0f;
			values[1] = 0.0f;
			values[2] = 0.0f;

			constantBuffer->SetData(values, layout.second.GetSize(), layout.second.Offset);
		}
	}

	for (auto layout : shader->GetPixelConstantLayouts())
	{
		if (layout.first == "predefined_uniform")
		{
			float values[4];
			values[0] = time;
			constantBuffer->SetData(&time, layout.second.GetSize(), layout.second.Offset);
		}

		if (layout.first == "customData1")
		{
			float values[4];
			constantBuffer->SetData(customData1.data(), layout.second.GetSize(), layout.second.Offset);
		}

		if (layout.first == "customData2")
		{
			float values[4];
			constantBuffer->SetData(customData2.data(), layout.second.GetSize(), layout.second.Offset);
		}
	}

	return true;
}

void Preview::Render()
{
	ar::SceneParameter sceneParam;
	sceneParam.RenderTargets[0] = renderTarget_;
	sceneParam.DepthTarget = depthTarget_;

	graphics_->GetManager()->BeginScene(sceneParam);
	graphics_->GetManager()->BeginRendering();

	ar::Color color;
	color.R = 0;
	color.G = 0;
	color.B = 0;
	color.A = 0;
	graphics_->GetManager()->Clear(true, true, color);

	context->Begin();

	if (shader != nullptr)
	{
		if (mesh_ != nullptr && ModelType != PreviewModelType::Screen)
		{
			ar::DrawParameter drawParam;
			drawParam.AlphaBlend = ar::AlphaBlendMode::Blend;
			drawParam.Culling = ar::CullingType::Double;
			drawParam.VertexBufferPtr = mesh_->GetVertexBuffer();
			drawParam.IndexBufferPtr = mesh_->GetIndexBuffer();
			drawParam.IndexCount = mesh_->GetIndexCount();
			drawParam.IndexOffset = 0;
			drawParam.ShaderPtr = shader;
			drawParam.IsDepthTest = false;
			drawParam.IsDepthWrite = false;
			drawParam.VertexConstantBufferPtr = this->constantBuffer;
			drawParam.PixelConstantBufferPtr = this->constantBuffer;

			auto pixelLayouts = shader->GetPixelTextureLayouts();

			for (int i = 0; i < textures_.size(); i++)
			{
				if (pixelLayouts.count(textures_[i]->Name) > 0)
				{
					auto ind = pixelLayouts[textures_[i]->Name].Index;

					drawParam.PixelShaderTextures[ind] =
						(textures_[i]->TexturePtr != nullptr) ? textures_[i]->TexturePtr->GetTexture() : nullptr;
					drawParam.PixelShaderTextureWraps[ind] =
						textures_[i]->SamplerType == TextureSamplerType::Repeat ? ar::TextureWrapType::Repeat : ar::TextureWrapType::Clamp;
				}
			}

			if (pixelLayouts.count("efk_depth") > 0)
			{
				drawParam.PixelShaderTextures[pixelLayouts["efk_depth"].Index] = white_->GetTexture();
			}

			context->Draw(drawParam);
		}
		else
		{
			ar::DrawParameter drawParam;
			drawParam.AlphaBlend = ar::AlphaBlendMode::Blend;
			drawParam.Culling = ar::CullingType::Double;
			drawParam.VertexBufferPtr = vb;
			drawParam.IndexBufferPtr = ib;
			drawParam.IndexCount = 6;
			drawParam.IndexOffset = 0;
			drawParam.ShaderPtr = shader;
			drawParam.IsDepthTest = false;
			drawParam.IsDepthWrite = false;
			drawParam.VertexConstantBufferPtr = this->constantBuffer;
			drawParam.PixelConstantBufferPtr = this->constantBuffer;

			auto pixelLayouts = shader->GetPixelTextureLayouts();

			for (int i = 0; i < textures_.size(); i++)
			{
				if (pixelLayouts.count(textures_[i]->Name) > 0)
				{
					auto ind = pixelLayouts[textures_[i]->Name].Index;

					drawParam.PixelShaderTextures[ind] =
						(textures_[i]->TexturePtr != nullptr) ? textures_[i]->TexturePtr->GetTexture() : nullptr;
					drawParam.PixelShaderTextureWraps[ind] =
						textures_[i]->SamplerType == TextureSamplerType::Repeat ? ar::TextureWrapType::Repeat : ar::TextureWrapType::Clamp;
				}
			}

			if (pixelLayouts.count("efk_depth") > 0)
			{
				drawParam.PixelShaderTextures[pixelLayouts["efk_depth"].Index] = white_->GetTexture();
			}

			context->Draw(drawParam);
		}
	}

	context->End();

	graphics_->GetManager()->EndRendering();
	graphics_->GetManager()->EndScene();
}

uint64_t Preview::GetInternal()
{
	if (renderTarget_ == nullptr)
		return 0;
	auto ret = renderTarget_->GetInternalObjects()[0];

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
