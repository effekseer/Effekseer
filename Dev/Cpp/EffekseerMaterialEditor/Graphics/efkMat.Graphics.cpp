
#include "efkMat.Graphics.h"

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/glew.h>
#endif

#include <fstream>
#include <iostream>

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

Texture::~Texture() { ar::SafeDelete(texture); }

std::shared_ptr<Texture> Texture::Load(std::shared_ptr<Graphics> graphics, const char* path)
{
	std::vector<uint8_t> buffer;

	std::ifstream fin(path, std::ios::in | std::ios::binary);
	if (fin.bad())
		return nullptr;
	if (fin.fail())
		return nullptr;

	while (!fin.eof())
	{
		uint8_t b;
		fin.read((char*)&b, 1);
		buffer.push_back(b);
	}

	auto obj = std::make_shared<Texture>();

	obj->graphics_ = graphics;

	auto texture = ar::Texture2D::Create(graphics->GetManager());

	texture->Initialize(graphics->GetManager(), buffer.data(), buffer.size(), false, true);
	obj->texture = texture;

	return obj;
}

std::map<std::string, std::shared_ptr<Texture>> TextureCache::textures;

std::shared_ptr<Texture> TextureCache::Load(std::shared_ptr<Graphics> graphics, const char* path)
{
	std::string key = path;

	if (textures.count(key) > 0)
	{
		return textures[key];
	}

	auto texture = Texture::Load(graphics, path);
	if (texture != nullptr)
	{
		textures[key] = texture;
	}

	return texture;
}

Preview::Preview() {}

Preview::~Preview()
{
	ar::SafeDelete(vb);
	ar::SafeDelete(ib);
	ar::SafeDelete(shader);
	ar::SafeDelete(renderTexture);
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

	vb_[0].Pos[0] = -1.0;
	vb_[0].Pos[1] = +1.0;
	vb_[0].Pos[2] = 0.5;
	vb_[1].Pos[0] = +1.0;
	vb_[1].Pos[1] = +1.0;
	vb_[1].Pos[2] = 0.5;
	vb_[2].Pos[0] = +1.0;
	vb_[2].Pos[1] = -1.0;
	vb_[2].Pos[2] = 0.5;
	vb_[3].Pos[0] = -1.0;
	vb_[3].Pos[1] = -1.0;
	vb_[3].Pos[2] = 0.5;

	vb_[0].UV[0] = 0.0;
	vb_[0].UV[1] = 0.0;
	vb_[1].UV[0] = 1.0;
	vb_[1].UV[1] = 0.0;
	vb_[2].UV[0] = 1.0;
	vb_[2].UV[1] = 1.0;
	vb_[3].UV[0] = 0.0;
	vb_[3].UV[1] = 1.0;

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

	renderTexture = ar::RenderTexture2D::Create(graphics->GetManager());
	renderTexture->Initialize(graphics->GetManager(), TextureSize, TextureSize, ar::TextureFormat::R8G8B8A8_UNORM);

	context = ar::Context::Create(graphics_->GetManager());
	context->Initialize(graphics_->GetManager());

	return true;
}

bool Preview::CompileShader(std::string& vs,
							std::string& ps,
							std::vector<std::shared_ptr<Texture>> textures,
							std::vector<std::shared_ptr<TextExporterUniform>>& uniforms)
{
	std::cout << "Compile Shader" << std::endl;

	ar::SafeDelete(shader);
	ar::SafeDelete(constantBuffer);

	ar::ShaderCompilerParameter param;
	param.VertexShaderTexts.push_back(vs);
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
	layout.resize(3);
	layout[0].Name = "Pos";
	layout[0].LayoutFormat = ar::VertexLayoutFormat::R32G32B32_FLOAT;
	layout[1].Name = "UV";
	layout[1].LayoutFormat = ar::VertexLayoutFormat::R32G32_FLOAT;
	layout[2].Name = "Color";
	layout[2].LayoutFormat = ar::VertexLayoutFormat::R8G8B8A8_UNORM;

	shader->Initialize(graphics_->GetManager(), result, layout, false);

	constantBuffer = ar::ConstantBuffer::Create(graphics_->GetManager());
	constantBuffer->Initialize(graphics_->GetManager(), shader->GetPixelConstantBufferSize());

	for (auto layout : shader->GetPixelConstantLayouts())
	{
		for (auto uni : uniforms)
		{
			if (uni->Name == layout.first)
			{
				constantBuffer->SetData(uni->DefaultConstants.data(), layout.second.GetSize(), layout.second.Offset);
				break;
			}
		}
	}

	textures_ = textures;

	return true;
}

bool Preview::UpdateUniforms(std::vector<std::shared_ptr<Texture>> textures, std::vector<std::shared_ptr<TextExporterUniform>>& uniforms)
{
	std::cout << "Update Uniforms" << std::endl;

	if (shader == nullptr)
		return false;

	for (auto layout : shader->GetPixelConstantLayouts())
	{
		for (auto uni : uniforms)
		{
			if (uni->Name == layout.first)
			{
				constantBuffer->SetData(uni->DefaultConstants.data(), layout.second.GetSize(), layout.second.Offset);
				break;
			}
		}
	}

	textures_ = textures;

	return true;
}

bool Preview::UpdateTime(float time)
{
	if (shader == nullptr)
		return false;

	for (auto layout : shader->GetPixelConstantLayouts())
	{
		if (layout.first == "ps_time")
		{
			constantBuffer->SetData(&time, layout.second.GetSize(), layout.second.Offset);
		}
	}

	return true;
}

void Preview::Render()
{
	ar::SceneParameter sceneParam;
	sceneParam.RenderTargets[0] = renderTexture;

	graphics_->GetManager()->BeginScene(sceneParam);
	graphics_->GetManager()->BeginRendering();

	context->Begin();

	ar::Color color;
	color.R = 0;
	color.G = 0;
	color.B = 0;
	color.A = 0;
	graphics_->GetManager()->Clear(true, false, color);

	if (shader != nullptr)
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
		drawParam.PixelConstantBufferPtr = this->constantBuffer;

		for (int i = 0; i < textures_.size(); i++)
		{
			drawParam.PixelShaderTextures[i] = textures_[i] != nullptr ? textures_[i]->GetTexture() : nullptr;
			drawParam.PixelShaderTextureWraps[i] = ar::TextureWrapType::Repeat;
		}

		context->Draw(drawParam);
	}

	context->End();

	graphics_->GetManager()->EndRendering();
	graphics_->GetManager()->EndScene();
}

uint64_t Preview::GetInternal()
{
	if (renderTexture == nullptr)
		return 0;
	auto ret = renderTexture->GetInternalObjects()[0];

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
