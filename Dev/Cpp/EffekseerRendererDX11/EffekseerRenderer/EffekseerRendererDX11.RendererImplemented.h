
#ifndef __EFFEKSEERRENDERER_DX11_RENDERER_IMPLEMENTED_H__
#define __EFFEKSEERRENDERER_DX11_RENDERER_IMPLEMENTED_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "../../EffekseerRendererCommon/EffekseerRenderer.RenderStateBase.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.StandardRenderer.h"
#include "EffekseerRendererDX11.Base.h"
#include "EffekseerRendererDX11.Renderer.h"
#include "GraphicsDevice.h"

#ifdef _WIN32
#include <xmmintrin.h>
#endif

namespace EffekseerRendererDX11
{

using Vertex = EffekseerRenderer::SimpleVertex;

class OriginalState : public ::Effekseer::SIMD::AlignedAllocationPolicy<16>
{
private:
	std::array<ID3D11SamplerState*, Effekseer::TextureSlotMax> m_samplers;

	ID3D11BlendState* m_blendState;
	std::array<float, Effekseer::TextureSlotMax> m_blendFactor;
	UINT m_blendSampleMask;

	ID3D11DepthStencilState* m_depthStencilState;
	UINT m_depthStencilStateRef;

	ID3D11RasterizerState* m_pRasterizerState;

	ID3D11Buffer* m_vertexConstantBuffer;
	ID3D11Buffer* m_pixelConstantBuffer;

	ID3D11VertexShader* m_pVS;
	ID3D11PixelShader* m_pPS;

	ID3D11InputLayout* m_layout;
	D3D11_PRIMITIVE_TOPOLOGY m_topology;

	std::array<ID3D11ShaderResourceView*, Effekseer::TextureSlotMax> m_psSRVs;

	ID3D11Buffer* m_pVB;
	UINT m_vbStrides;
	UINT m_vbOffset;

	ID3D11Buffer* m_pIB;
	DXGI_FORMAT m_ibFormat;
	UINT m_ibOffset;

public:
	OriginalState();
	~OriginalState();
	void SaveState(ID3D11Device* device, ID3D11DeviceContext* context);
	void LoadState(ID3D11Device* device, ID3D11DeviceContext* context);
	void ReleaseState();
};

class RendererImplemented;
using RendererImplementedRef = ::Effekseer::RefPtr<RendererImplemented>;

class RendererImplemented : public Renderer, public ::Effekseer::ReferenceObject, public ::Effekseer::SIMD::AlignedAllocationPolicy<16>
{
	friend class DeviceObject;

private:
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_context;

	VertexBuffer* m_vertexBuffer;
	IndexBuffer* m_indexBuffer;
	IndexBuffer* m_indexBufferForWireframe = nullptr;
	int32_t m_squareMaxCount;

	Shader* shader_unlit_ = nullptr;
	Shader* shader_distortion_ = nullptr;
	Shader* shader_lit_ = nullptr;
	Shader* shader_ad_unlit_ = nullptr;
	Shader* shader_ad_distortion_ = nullptr;
	Shader* shader_ad_lit_ = nullptr;

	Shader* currentShader = nullptr;

	EffekseerRenderer::StandardRenderer<RendererImplemented, Shader>* m_standardRenderer;

	::Effekseer::CoordinateSystem m_coordinateSystem;

	::EffekseerRenderer::RenderStateBase* m_renderState;

	std::set<DeviceObject*> m_deviceObjects;

	OriginalState* m_state;

	bool m_restorationOfStates;

	::Effekseer::Backend::TextureRef m_backgroundDX11;

	D3D11_COMPARISON_FUNC m_depthFunc;

	EffekseerRenderer::DistortingCallback* m_distortingCallback;

	Backend::GraphicsDeviceRef graphicsDevice_ = nullptr;

public:
	RendererImplemented(int32_t squareMaxCount);

	~RendererImplemented();

	void OnLostDevice();
	void OnResetDevice();

	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* context, D3D11_COMPARISON_FUNC depthFunc, bool isMSAAEnabled);

	void SetRestorationOfStatesFlag(bool flag);

	bool BeginRendering();

	bool EndRendering();

	ID3D11Device* GetDevice() override;

	ID3D11DeviceContext* GetContext() override;

	VertexBuffer* GetVertexBuffer();

	IndexBuffer* GetIndexBuffer();

	int32_t GetSquareMaxCount() const;

	::EffekseerRenderer::RenderStateBase* GetRenderState();

	::Effekseer::SpriteRendererRef CreateSpriteRenderer();

	::Effekseer::RibbonRendererRef CreateRibbonRenderer();

	::Effekseer::RingRendererRef CreateRingRenderer();

	::Effekseer::ModelRendererRef CreateModelRenderer();

	::Effekseer::TrackRendererRef CreateTrackRenderer();

	::Effekseer::TextureLoaderRef CreateTextureLoader(::Effekseer::FileInterface* fileInterface = nullptr);

	::Effekseer::ModelLoaderRef CreateModelLoader(::Effekseer::FileInterface* fileInterface = nullptr);

	::Effekseer::MaterialLoaderRef CreateMaterialLoader(::Effekseer::FileInterface* fileInterface = nullptr) override;

	void SetBackground(ID3D11ShaderResourceView* background) override;

	EffekseerRenderer::DistortingCallback* GetDistortingCallback() override;

	void SetDistortingCallback(EffekseerRenderer::DistortingCallback* callback) override;

	EffekseerRenderer::StandardRenderer<RendererImplemented, Shader>* GetStandardRenderer()
	{
		return m_standardRenderer;
	}

	void SetVertexBuffer(VertexBuffer* vertexBuffer, int32_t size);
	void SetVertexBuffer(ID3D11Buffer* vertexBuffer, int32_t size);
	void SetIndexBuffer(IndexBuffer* indexBuffer);
	void SetIndexBuffer(ID3D11Buffer* indexBuffer);

	void SetVertexBuffer(const Effekseer::Backend::VertexBufferRef& vertexBuffer, int32_t size);
	void SetIndexBuffer(const Effekseer::Backend::IndexBufferRef& indexBuffer);

	void SetLayout(Shader* shader);
	void DrawSprites(int32_t spriteCount, int32_t vertexOffset);
	void DrawPolygon(int32_t vertexCount, int32_t indexCount);
	void DrawPolygonInstanced(int32_t vertexCount, int32_t indexCount, int32_t instanceCount);

	Shader* GetShader(::EffekseerRenderer::RendererShaderType type) const;
	void BeginShader(Shader* shader);
	void EndShader(Shader* shader);

	void SetVertexBufferToShader(const void* data, int32_t size, int32_t dstOffset);

	void SetPixelBufferToShader(const void* data, int32_t size, int32_t dstOffset);

	void SetTextures(Shader* shader, ::Effekseer::Backend::TextureRef* textures, int32_t count);

	void ResetRenderState();

	Effekseer::Backend::GraphicsDeviceRef GetGraphicsDevice() const override;

	void ResetStateForDefferedContext() override;

	virtual int GetRef()
	{
		return ::Effekseer::ReferenceObject::GetRef();
	}
	virtual int AddRef()
	{
		return ::Effekseer::ReferenceObject::AddRef();
	}
	virtual int Release()
	{
		return ::Effekseer::ReferenceObject::Release();
	}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerRendererDX11
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEERRENDERER_DX11_RENDERER_IMPLEMENTED_H__