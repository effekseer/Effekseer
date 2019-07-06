
#pragma once

#include "../../EffekseerRendererCommon/EffekseerRenderer.ModelRendererBase.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.RenderStateBase.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.StandardRenderer.h"
#include "EffekseerRendererArea.Renderer.h"

#include <map>
#include <memory>

namespace EffekseerRendererArea
{
class RendererImplemented;
class RenderState;
class VertexBuffer;
class IndexBuffer;
class Shader;

class TextureData
{
public:
	void* UserPtr = nullptr;
};

class TextureLoader : public ::Effekseer::TextureLoader
{
public:
	::Effekseer::TextureData* Load(const EFK_CHAR* path, ::Effekseer::TextureType textureType)
	{
		auto d_ = new ::Effekseer::TextureData();
		return d_;
	}

	void Unload(::Effekseer::TextureData* data)
	{
		if (data == nullptr)
			return;

		auto textureData = (TextureData*)data->UserPtr;
		delete textureData;
		delete data;
	}
};

class ModelLoader : public ::Effekseer::ModelLoader
{
private:
	::Effekseer::FileInterface* m_fileInterface;
	::Effekseer::DefaultFileInterface m_defaultFileInterface;

public:
	ModelLoader(::Effekseer::FileInterface* fileInterface) : m_fileInterface(fileInterface)
	{
		if (m_fileInterface == NULL)
		{
			m_fileInterface = &m_defaultFileInterface;
		}
	}
	virtual ~ModelLoader() {}

	void* Load(const EFK_CHAR* path) override
	{
		std::unique_ptr<Effekseer::FileReader> reader(m_fileInterface->OpenRead(path));

		if (reader.get() != NULL)
		{
			size_t size_model = reader->GetLength();
			char* data_model = new char[size_model];
			reader->Read(data_model, size_model);

			Effekseer::Model* model = (Effekseer::Model*)Load(data_model, (int32_t)size_model);

			delete[] data_model;

			return (void*)model;
		}

		return NULL;
	}

	void* Load(const void* data, int32_t size) override
	{
		Effekseer::Model* model = new Effekseer::Model((uint8_t*)data, size);
		return model;
	}

	virtual void Unload(void* data)
	{
		auto m = (Effekseer::Model*)data;
		if (m != nullptr)
		{
			delete m;
		}
	}
};

struct VertexDistortion
{
	::Effekseer::Vector3D Pos;
	::Effekseer::Color Col;
	float UV[2];
	::Effekseer::Vector3D Tangent;
	::Effekseer::Vector3D Binormal;

	void SetColor(const ::Effekseer::Color& color) { Col = color; }
};

typedef ::Effekseer::ModelRenderer::NodeParameter efkModelNodeParam;
typedef ::Effekseer::ModelRenderer::InstanceParameter efkModelInstanceParam;
typedef ::Effekseer::Vector3D efkVector3D;

class ModelRenderer : public ::EffekseerRenderer::ModelRendererBase
{
private:
	RendererImplemented* m_renderer;
	ModelRenderer(RendererImplemented* renderer);

public:
	virtual ~ModelRenderer();

	static ModelRenderer* Create(RendererImplemented* renderer);

public:
	void BeginRendering(const efkModelNodeParam& parameter, int32_t count, void* userData);

	void Rendering(const efkModelNodeParam& parameter, const efkModelInstanceParam& instanceParameter, void* userData) override;

	void EndRendering(const efkModelNodeParam& parameter, void* userData);
};

class RendererImplemented : public Renderer, public ::Effekseer::ReferenceObject
{
protected:
	::Effekseer::Vector3D m_lightDirection;
	::Effekseer::Color m_lightColor;
	::Effekseer::Color m_lightAmbient;
	int32_t m_squareMaxCount;

	::Effekseer::Matrix44 m_proj;
	::Effekseer::Matrix44 m_camera;
	::Effekseer::Matrix44 m_cameraProj;

	::Effekseer::Vector3D m_cameraPosition;
	::Effekseer::Vector3D m_cameraFrontDirection;

	VertexBuffer* m_vertexBuffer = nullptr;
	Shader* m_stanShader = nullptr;
	Shader* m_distShader = nullptr;
	RenderState* m_renderState = nullptr;

	bool m_distorting = false;
	Shader* currentShader = nullptr;

	void* m_textures[16];

	std::vector<Vertex> m_vertexes;
	std::vector<std::array<int32_t, 3>> m_indexes;
	std::vector<RenderedMaterial> m_materials;
	std::vector<int32_t> m_materialFaceCounts;

	EffekseerRenderer::StandardRenderer<RendererImplemented, Shader, Vertex, VertexDistortion>* m_standardRenderer = nullptr;

public:
	static RendererImplemented* Create();

	RendererImplemented();
	virtual ~RendererImplemented();

	void OnLostDevice() override {}
	void OnResetDevice() override {}

	bool Initialize(int32_t squareMaxCount);

	void Destroy() override;

	void SetRestorationOfStatesFlag(bool flag) override;

	bool BeginRendering() override;

	bool EndRendering() override;

	const ::Effekseer::Vector3D& GetLightDirection() const override;

	void SetLightDirection(const ::Effekseer::Vector3D& direction) override;

	const ::Effekseer::Color& GetLightColor() const override;

	void SetLightColor(const ::Effekseer::Color& color) override;

	const ::Effekseer::Color& GetLightAmbientColor() const override;

	void SetLightAmbientColor(const ::Effekseer::Color& color) override;

	int32_t GetSquareMaxCount() const override;

	const ::Effekseer::Matrix44& GetProjectionMatrix() const override;

	void SetProjectionMatrix(const ::Effekseer::Matrix44& mat) override;

	const ::Effekseer::Matrix44& GetCameraMatrix() const override;

	void SetCameraMatrix(const ::Effekseer::Matrix44& mat) override;

	::Effekseer::Vector3D GetCameraFrontDirection() const override;

	::Effekseer::Vector3D GetCameraPosition() const override;

	void SetCameraParameter(const ::Effekseer::Vector3D& front, const ::Effekseer::Vector3D& position) override;

	::Effekseer::Matrix44& GetCameraProjectionMatrix() override;

	::Effekseer::SpriteRenderer* CreateSpriteRenderer() override;

	::Effekseer::RibbonRenderer* CreateRibbonRenderer() override;

	::Effekseer::RingRenderer* CreateRingRenderer() override;

	::Effekseer::ModelRenderer* CreateModelRenderer() override;

	::Effekseer::TrackRenderer* CreateTrackRenderer() override;

	::Effekseer::TextureLoader* CreateTextureLoader(::Effekseer::FileInterface* fileInterface = NULL) override;

	::Effekseer::ModelLoader* CreateModelLoader(::Effekseer::FileInterface* fileInterface = NULL) override;

	::Effekseer::MaterialLoader* CreateMaterialLoader(::Effekseer::FileInterface* fileInterface = nullptr) override { return nullptr; }

	void ResetRenderState() override;

	::EffekseerRenderer::DistortingCallback* GetDistortingCallback() override;

	void SetDistortingCallback(::EffekseerRenderer::DistortingCallback* callback) override;

	void SetRenderMode(Effekseer::RenderMode renderMode) {}

	Effekseer::RenderMode GetRenderMode() { return Effekseer::RenderMode::Normal; }

	Effekseer::TextureData* GetBackground();

	VertexBuffer* GetVertexBuffer();

	IndexBuffer* GetIndexBuffer();

	EffekseerRenderer::StandardRenderer<RendererImplemented, Shader, Vertex, VertexDistortion>* GetStandardRenderer();

	::EffekseerRenderer::RenderStateBase* GetRenderState();

	void SetVertexBuffer(VertexBuffer* vertexBuffer, int32_t size);
	void SetIndexBuffer(IndexBuffer* indexBuffer);

	void SetLayout(Shader* shader);
	void DrawSprites(int32_t spriteCount, int32_t vertexOffset);

	void DrawModel(void* model,
				   std::vector<Effekseer::Matrix44>& matrixes,
				   std::vector<Effekseer::RectF>& uvs,
				   std::vector<Effekseer::Color>& colors);

	Shader* GetShader(bool useTexture, bool useDistortion) const;
	void BeginShader(Shader* shader);
	void EndShader(Shader* shader);

	void SetVertexBufferToShader(const void* data, int32_t size, int32_t dstOffset);

	void SetPixelBufferToShader(const void* data, int32_t size, int32_t dstOffset);

	void SetTextures(Shader* shader, Effekseer::TextureData** textures, int32_t count);

	virtual int GetRef() { return ::Effekseer::ReferenceObject::GetRef(); }
	virtual int AddRef() { return ::Effekseer::ReferenceObject::AddRef(); }
	virtual int Release() { return ::Effekseer::ReferenceObject::Release(); }

	std::vector<Vertex>& GetCurrentVertexes() { return m_vertexes; }
	std::vector<std::array<int32_t, 3>>& GetCurrentIndexes() { return m_indexes; }
	std::vector<RenderedMaterial>& GetCurrentMaterials() { return m_materials; }
	std::vector<int32_t>& GetCurrentMaterialFaceCounts() { return m_materialFaceCounts; }
};

} // namespace EffekseerRendererArea
