
#ifndef __EFFEKSEERRENDERER_DX11_RENDERER_IMPLEMENTED_H__
#define __EFFEKSEERRENDERER_DX11_RENDERER_IMPLEMENTED_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererDX11.Base.h"
#include "EffekseerRendererDX11.Renderer.h"
#include "GraphicsDevice.h"
#include <EffekseerRendererCommon/EffekseerRenderer.RenderStateBase.h>
#include <EffekseerRendererCommon/EffekseerRenderer.StandardRenderer.h>

#ifdef _WIN32
#include <xmmintrin.h>
#endif

namespace EffekseerRendererDX11
{

using Vertex = EffekseerRenderer::SimpleVertex;

class OriginalState : public ::Effekseer::SIMD::AlignedAllocationPolicy<16>
{
private:
	std::array<ID3D11SamplerState*, Effekseer::TextureSlotMax> samplers_;

	ID3D11BlendState* blendState_;
	std::array<float, Effekseer::TextureSlotMax> blendFactor_;
	UINT blendSampleMask_;

	ID3D11DepthStencilState* depthStencilState_;
	UINT depthStencilState_Ref;

	ID3D11RasterizerState* rasterizerState_;

	ID3D11Buffer* vertexConstantBuffer_;
	ID3D11Buffer* pixelConstantBuffer_;

	ID3D11VertexShader* vs_;
	ID3D11PixelShader* ps_;

	ID3D11InputLayout* layout_;
	D3D11_PRIMITIVE_TOPOLOGY topology_;

	std::array<ID3D11ShaderResourceView*, Effekseer::TextureSlotMax> psSrvs_;

	ID3D11Buffer* vertexBuffer_;
	UINT vbStrides_;
	UINT vbOffset_;

	ID3D11Buffer* indexBuffer_;
	DXGI_FORMAT ibFormat_;
	UINT ibOffset_;

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
	ID3D11Device* device_ = nullptr;
	ID3D11DeviceContext* context_ = nullptr;

	Effekseer::Backend::IndexBufferRef current_index_buffer_;
	Effekseer::Backend::IndexBufferRef index_buffer_;
	Effekseer::Backend::IndexBufferRef index_buffer_for_wireframe_;
	int32_t square_max_count_ = 0;
	Shader* current_shader_ = nullptr;

	EffekseerRenderer::StandardRenderer<RendererImplemented, Shader>* standard_renderer_ = nullptr;

	::Effekseer::CoordinateSystem coordinate_system_ = ::Effekseer::CoordinateSystem::RH;

	::EffekseerRenderer::RenderStateBase* render_state_ = nullptr;

	std::set<DeviceObject*> device_objects_;

	OriginalState* state_ = nullptr;

	bool restoration_of_states_ = true;

	::Effekseer::Backend::TextureRef background_dx11_;

	D3D11_COMPARISON_FUNC depth_func_ = D3D11_COMPARISON_NEVER;

	EffekseerRenderer::DistortingCallback* distorting_callback_ = nullptr;

	Backend::GraphicsDeviceRef graphics_device_ = nullptr;

public:
	RendererImplemented(int32_t squareMaxCount);

	~RendererImplemented();

	void OnLostDevice();
	void OnResetDevice();

	bool Initialize(Backend::GraphicsDeviceRef graphicsDevice, D3D11_COMPARISON_FUNC depthFunc, bool isMSAAEnabled);

	void SetRestorationOfStatesFlag(bool flag);

	bool BeginRendering();

	bool EndRendering();

	ID3D11Device* GetDevice() override;

	ID3D11DeviceContext* GetContext() override;

	Effekseer::Backend::IndexBufferRef GetIndexBuffer();

	int32_t GetSquareMaxCount() const;

	::EffekseerRenderer::RenderStateBase* GetRenderState();

	::Effekseer::SpriteRendererRef CreateSpriteRenderer();

	::Effekseer::RibbonRendererRef CreateRibbonRenderer();

	::Effekseer::RingRendererRef CreateRingRenderer();

	::Effekseer::ModelRendererRef CreateModelRenderer();

	::Effekseer::TrackRendererRef CreateTrackRenderer();

	::Effekseer::GpuTimerRef CreateGpuTimer() override;

	::Effekseer::GpuParticleSystemRef CreateGpuParticleSystem(const Effekseer::GpuParticleSystem::Settings& settings = {}) override;

	::Effekseer::GpuParticleFactoryRef CreateGpuParticleFactory() override;

	::Effekseer::TextureLoaderRef CreateTextureLoader(::Effekseer::FileInterfaceRef fileInterface = nullptr);

	::Effekseer::ModelLoaderRef CreateModelLoader(::Effekseer::FileInterfaceRef fileInterface = nullptr);

	::Effekseer::MaterialLoaderRef CreateMaterialLoader(::Effekseer::FileInterfaceRef fileInterface = nullptr) override;

	void SetBackground(ID3D11ShaderResourceView* background) override;

	EffekseerRenderer::DistortingCallback* GetDistortingCallback() override;

	void SetDistortingCallback(EffekseerRenderer::DistortingCallback* callback) override;

	EffekseerRenderer::StandardRenderer<RendererImplemented, Shader>* GetStandardRenderer()
	{
		return standard_renderer_;
	}

	void SetVertexBuffer(ID3D11Buffer* vertexBuffer, int32_t size);

	void SetVertexBuffer(const Effekseer::Backend::VertexBufferRef& vertexBuffer, int32_t size);
	void SetIndexBuffer(const Effekseer::Backend::IndexBufferRef& indexBuffer);

	void SetLayout(Shader* shader);
	void DrawSprites(int32_t spriteCount, int32_t vertexOffset);
	void DrawPolygon(int32_t vertexCount, int32_t indexCount);
	void DrawPolygonInstanced(int32_t vertexCount, int32_t indexCount, int32_t instanceCount);

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