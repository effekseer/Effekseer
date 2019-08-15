
#pragma once

#include <AltseedRHI.h>
#include <efkMat.TextExporter.h>

namespace EffekseerMaterial
{

struct Vertex
{
	float Pos[3];
	float UV[2];
	uint8_t Color[4];
};

class Graphics
{
private:
	ar::Manager* manager = nullptr;
	ar::Compiler* compiler = nullptr;

public:
	bool Initialize(int32_t width, int32_t height);
	Graphics();
	virtual ~Graphics();

	ar::Manager* GetManager() { return manager; }
	ar::Compiler* GetCompiler() { return compiler; }
};

class Texture
{
private:
	std::shared_ptr<Graphics> graphics_;
	ar::Texture2D* texture = nullptr;

public:
	Texture();
	virtual ~Texture();
	ar::Texture2D* GetTexture() { return texture; }
	static std::shared_ptr<Texture> Load(std::shared_ptr<Graphics> graphics, const char* path);
};

class TextureCache
{
private:
	static std::map<std::string, std::shared_ptr<Texture>> textures;

public:
	static std::shared_ptr<Texture> Load(std::shared_ptr<Graphics> graphics, const char* path);
};

class Preview
{
private:
	std::shared_ptr<Graphics> graphics_;
	ar::RenderTexture2D* renderTexture = nullptr;
	ar::Shader* shader = nullptr;
	ar::VertexBuffer* vb = nullptr;
	ar::IndexBuffer* ib = nullptr;
	ar::ConstantBuffer* constantBuffer = nullptr;
	ar::Context* context = nullptr;
	std::vector<std::shared_ptr<Texture>> textures_;

public:
	Preview();
	virtual ~Preview();
	bool Initialize(std::shared_ptr<Graphics> graphics);
	bool CompileShader(std::string& vs,
					   std::string& ps,
					   std::vector<std::shared_ptr<Texture>> textures,
					   std::vector<std::shared_ptr<TextExporterUniform>>& uniforms);
	bool UpdateUniforms(std::vector<std::shared_ptr<Texture>> textures, std::vector<std::shared_ptr<TextExporterUniform>>& uniforms);
	bool UpdateTime(float time);
	void Render();

	uint64_t GetInternal();

	static const int32_t TextureSize = 128;
};

} // namespace EffekseerMaterial
