
#ifndef __EFFEKSEERRENDERER_DX11_GRAPHICS_DEVICE_H__
#define __EFFEKSEERRENDERER_DX11_GRAPHICS_DEVICE_H__

#include <Effekseer.h>
#include <EffekseerRendererCommon/EffekseerRenderer.CommonUtils.h>
#include <assert.h>
#include <d3d11.h>
#include <set>

namespace EffekseerRendererDX11
{
namespace Backend
{

using D3D11BufferPtr = std::unique_ptr<ID3D11Buffer, Effekseer::ReferenceDeleter<ID3D11Buffer>>;
using D3D11DevicePtr = std::unique_ptr<ID3D11Device, Effekseer::ReferenceDeleter<ID3D11Device>>;
using D3D11DeviceContextPtr = std::unique_ptr<ID3D11DeviceContext, Effekseer::ReferenceDeleter<ID3D11DeviceContext>>;
using D3D11ResourcePtr = std::unique_ptr<ID3D11Resource, Effekseer::ReferenceDeleter<ID3D11Resource>>;
using D3D11Texture2DPtr = std::unique_ptr<ID3D11Texture2D, Effekseer::ReferenceDeleter<ID3D11Texture2D>>;
using D3D11Texture3DPtr = std::unique_ptr<ID3D11Texture3D, Effekseer::ReferenceDeleter<ID3D11Texture3D>>;
using D3D11ShaderResourceViewPtr = std::unique_ptr<ID3D11ShaderResourceView, Effekseer::ReferenceDeleter<ID3D11ShaderResourceView>>;
using D3D11UnorderedAccessViewPtr = std::unique_ptr<ID3D11UnorderedAccessView, Effekseer::ReferenceDeleter<ID3D11UnorderedAccessView>>;
using D3D11RenderTargetViewPtr = std::unique_ptr<ID3D11RenderTargetView, Effekseer::ReferenceDeleter<ID3D11RenderTargetView>>;
using D3D11DepthStencilViewPtr = std::unique_ptr<ID3D11DepthStencilView, Effekseer::ReferenceDeleter<ID3D11DepthStencilView>>;
using D3D11InputLayoutPtr = std::unique_ptr<ID3D11InputLayout, Effekseer::ReferenceDeleter<ID3D11InputLayout>>;
using D3D11VertexShaderPtr = std::unique_ptr<ID3D11VertexShader, Effekseer::ReferenceDeleter<ID3D11VertexShader>>;
using D3D11PixelShaderPtr = std::unique_ptr<ID3D11PixelShader, Effekseer::ReferenceDeleter<ID3D11PixelShader>>;
using D3D11ComputeShaderPtr = std::unique_ptr<ID3D11ComputeShader, Effekseer::ReferenceDeleter<ID3D11ComputeShader>>;
using D3D11RasterizerStatePtr = std::unique_ptr<ID3D11RasterizerState, Effekseer::ReferenceDeleter<ID3D11RasterizerState>>;
using D3D11DepthStencilStatePtr = std::unique_ptr<ID3D11DepthStencilState, Effekseer::ReferenceDeleter<ID3D11DepthStencilState>>;
using D3D11BlendStatePtr = std::unique_ptr<ID3D11BlendState, Effekseer::ReferenceDeleter<ID3D11BlendState>>;
using D3D11SamplerStatePtr = std::unique_ptr<ID3D11SamplerState, Effekseer::ReferenceDeleter<ID3D11SamplerState>>;
using D3D11QueryPtr = std::unique_ptr<ID3D11Query, Effekseer::ReferenceDeleter<ID3D11Query>>;

class GraphicsDevice;
class VertexBuffer;
class IndexBuffer;
class UniformBuffer;
class Shader;
class ComputeShader;
class VertexLayout;
class FrameBuffer;
class Texture;
class RenderPass;
class PipelineState;
class UniformLayout;
class ComputeBuffer;

using GraphicsDeviceRef = Effekseer::RefPtr<GraphicsDevice>;
using VertexBufferRef = Effekseer::RefPtr<VertexBuffer>;
using IndexBufferRef = Effekseer::RefPtr<IndexBuffer>;
using UniformBufferRef = Effekseer::RefPtr<UniformBuffer>;
using ShaderRef = Effekseer::RefPtr<Shader>;
using ComputeShaderRef = Effekseer::RefPtr<ComputeShader>;
using VertexLayoutRef = Effekseer::RefPtr<VertexLayout>;
using FrameBufferRef = Effekseer::RefPtr<FrameBuffer>;
using TextureRef = Effekseer::RefPtr<Texture>;
using RenderPassRef = Effekseer::RefPtr<RenderPass>;
using PipelineStateRef = Effekseer::RefPtr<PipelineState>;
using UniformLayoutRef = Effekseer::RefPtr<UniformLayout>;
using ComputeBufferRef = Effekseer::RefPtr<ComputeBuffer>;

DXGI_FORMAT GetTextureFormatType(Effekseer::Backend::TextureFormatType format);

D3D11InputLayoutPtr CreateInputLayout(GraphicsDevice& graphicsDevice, VertexLayoutRef vertexLayout, const void* vertexBufferData, int32_t vertexBufferSize);

class DeviceObject
{
private:
public:
	virtual void OnLostDevice();

	virtual void OnResetDevice();
};

/**
	@brief	VertexBuffer of DirectX11
*/
class VertexBuffer
	: public DeviceObject,
	  public Effekseer::Backend::VertexBuffer
{
private:
	EffekseerRenderer::DirtiedBlock blocks_;

	GraphicsDevice* graphicsDevice_ = nullptr;
	D3D11BufferPtr buffer_ = nullptr;
	int32_t size_ = 0;
	bool isDynamic_ = false;

public:
	VertexBuffer(GraphicsDevice* graphicsDevice);

	~VertexBuffer() override;

	bool Allocate(const void* src, int32_t size, bool isDynamic);

	void Deallocate();

	void OnLostDevice() override;

	void OnResetDevice() override;

	bool Init(const void* src, int32_t size, bool isDynamic);

	void UpdateData(const void* src, int32_t size, int32_t offset);

	void MakeAllDirtied();

	ID3D11Buffer* GetBuffer() const
	{
		return buffer_.get();
	}
};

/**
	@brief	IndexBuffer of DirectX11
*/
class IndexBuffer
	: public DeviceObject,
	  public Effekseer::Backend::IndexBuffer
{
private:
	EffekseerRenderer::DirtiedBlock blocks_;

	GraphicsDevice* graphicsDevice_ = nullptr;
	D3D11BufferPtr buffer_ = nullptr;
	int32_t stride_ = 0;

public:
	IndexBuffer(GraphicsDevice* graphicsDevice);

	~IndexBuffer() override;

	bool Allocate(const void* src, int32_t elementCount, int32_t stride);

	void Deallocate();

	void OnLostDevice() override;

	void OnResetDevice() override;

	bool Init(const void* src, int32_t elementCount, int32_t stride);

	void UpdateData(const void* src, int32_t size, int32_t offset);

	ID3D11Buffer* GetBuffer() const
	{
		return buffer_.get();
	}
};

class UniformBuffer
	: public DeviceObject,
	  public Effekseer::Backend::UniformBuffer
{
private:
	GraphicsDevice* graphicsDevice_ = nullptr;
	D3D11BufferPtr buffer_;
	Effekseer::CustomVector<uint8_t> data_;
	bool isDirtied_ = false;

public:
	UniformBuffer(GraphicsDevice* graphicsDevice);
	~UniformBuffer() override;

	bool Init(int32_t size, const void* initialData);

	ID3D11Buffer* GetBuffer() const
	{
		return buffer_.get();
	}

	void UpdateData(const void* src, int32_t size, int32_t offset);

	void UpdateDataActually();

	bool GetIsDirtied() const;
};

class ComputeBuffer
	: public DeviceObject,
	  public Effekseer::Backend::ComputeBuffer
{
private:
	GraphicsDevice* graphicsDevice_ = nullptr;
	D3D11BufferPtr buffer_;
	D3D11ShaderResourceViewPtr srv_;
	D3D11UnorderedAccessViewPtr uav_;

public:
	ComputeBuffer(GraphicsDevice* graphicsDevice);
	~ComputeBuffer() override;

	bool Init(int32_t elementCount, int32_t elementSize, const void* initialData);

	void UpdateData(const void* src, int32_t size, int32_t offset);

	ID3D11Buffer* GetBuffer() const
	{
		return buffer_.get();
	}

	ID3D11ShaderResourceView* GetSRV() const
	{
		return srv_.get();
	}

	ID3D11UnorderedAccessView* GetUAV() const
	{
		return uav_.get();
	}
};

class Texture
	: public DeviceObject,
	  public Effekseer::Backend::Texture
{
	D3D11Texture2DPtr texture2d_;
	D3D11Texture3DPtr texture3d_;
	D3D11ShaderResourceViewPtr srv_;
	D3D11RenderTargetViewPtr rtv_;
	D3D11DepthStencilViewPtr dsv_;

	GraphicsDevice* graphicsDevice_ = nullptr;

public:
	Texture(GraphicsDevice* graphicsDevice);
	~Texture() override;

	bool Init(const Effekseer::Backend::TextureParameter& param, const Effekseer::CustomVector<uint8_t>& initialData);

	bool Init(const Effekseer::Backend::RenderTextureParameter& param);

	bool Init(const Effekseer::Backend::DepthTextureParameter& param);

	bool Init(ID3D11ShaderResourceView* srv, ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsv);

	ID3D11Texture2D* GetTexture() const
	{
		return texture2d_.get();
	}

	ID3D11Resource* GetResource() const
	{
		if (texture2d_ != nullptr)
		{
			return texture2d_.get();
		}

		return texture3d_.get();
	}

	ID3D11ShaderResourceView* GetSRV() const
	{
		return srv_.get();
	};

	ID3D11RenderTargetView* GetRTV() const
	{
		return rtv_.get();
	};

	ID3D11DepthStencilView* GetDSV() const
	{
		return dsv_.get();
	};
};

class VertexLayout
	: public DeviceObject,
	  public Effekseer::Backend::VertexLayout
{
private:
	Effekseer::CustomVector<Effekseer::Backend::VertexLayoutElement> elements_;

public:
	VertexLayout() = default;
	~VertexLayout() = default;

	bool Init(const Effekseer::Backend::VertexLayoutElement* elements, int32_t elementCount);

	const Effekseer::CustomVector<Effekseer::Backend::VertexLayoutElement>& GetElements() const
	{
		return elements_;
	}
};

class RenderPass
	: public DeviceObject,
	  public Effekseer::Backend::RenderPass
{
private:
	GraphicsDevice* graphicsDevice_ = nullptr;
	Effekseer::FixedSizeVector<Effekseer::Backend::TextureRef, Effekseer::Backend::RenderTargetMax> textures_;
	Effekseer::Backend::TextureRef depthTexture_;

public:
	RenderPass(GraphicsDevice* graphicsDevice);
	~RenderPass() override;

	bool Init(Effekseer::FixedSizeVector<Effekseer::Backend::TextureRef, Effekseer::Backend::RenderTargetMax>& textures, Effekseer::Backend::TextureRef depthTexture);

	Effekseer::FixedSizeVector<Effekseer::Backend::TextureRef, Effekseer::Backend::RenderTargetMax>& GetTextures()
	{
		return textures_;
	}

	Effekseer::Backend::TextureRef& GetDepthTexture()
	{
		return depthTexture_;
	}
};

class Shader
	: public DeviceObject,
	  public Effekseer::Backend::Shader
{
private:
	GraphicsDevice* graphicsDevice_ = nullptr;
	Effekseer::CustomVector<uint8_t> vsData_;
	D3D11VertexShaderPtr vs_;
	D3D11PixelShaderPtr ps_;
	D3D11ComputeShaderPtr cs_;

public:
	Shader(GraphicsDevice* graphicsDevice);
	~Shader() override;
	bool Init(const void* vertexShaderData, int32_t vertexShaderDataSize, const void* pixelShaderData, int32_t pixelShaderDataSize);
	bool InitAsCompute(const void* computeShaderData, int32_t computeShaderDataSize);

	const Effekseer::CustomVector<uint8_t>& GetVertexShaderData() const
	{
		return vsData_;
	}

	ID3D11VertexShader* GetVertexShader() const
	{
		return vs_.get();
	}

	ID3D11PixelShader* GetPixelShader() const
	{
		return ps_.get();
	}

	ID3D11ComputeShader* GetComputeShader() const
	{
		return cs_.get();
	}
};

class PipelineState
	: public DeviceObject,
	  public Effekseer::Backend::PipelineState
{
	static const int32_t LayoutElementMax = 16;

private:
	GraphicsDevice* graphicsDevice_ = nullptr;
	Effekseer::Backend::PipelineStateParameter param_;

	D3D11RasterizerStatePtr rasterizerState_;
	D3D11DepthStencilStatePtr depthStencilState_;
	D3D11BlendStatePtr blendState_;
	D3D11InputLayoutPtr inputLayout_;

public:
	PipelineState(GraphicsDevice* graphicsDevice);
	~PipelineState() override;

	bool Init(const Effekseer::Backend::PipelineStateParameter& param);

	const Effekseer::Backend::PipelineStateParameter& GetParam() const
	{
		return param_;
	}

	ID3D11RasterizerState* GetRasterizerState() const
	{
		return rasterizerState_.get();
	}

	ID3D11DepthStencilState* GetDepthStencilState() const
	{
		return depthStencilState_.get();
	}

	ID3D11BlendState* GetBlendState() const
	{
		return blendState_.get();
	}

	ID3D11InputLayout* GetInputLayout() const
	{
		return inputLayout_.get();
	}
};

/**
	@brief	GraphicsDevice of DirectX11
*/
class GraphicsDevice
	: public Effekseer::Backend::GraphicsDevice
{
private:
	D3D11DevicePtr device_ = nullptr;
	D3D11DeviceContextPtr context_ = nullptr;
	std::array<std::array<D3D11SamplerStatePtr, 2>, 2> samplers_;
	std::set<DeviceObject*> objects_;

public:
	GraphicsDevice(ID3D11Device* device, ID3D11DeviceContext* context);

	~GraphicsDevice() override;

	ID3D11Device* GetDevice() const;

	ID3D11DeviceContext* GetContext() const;

	void LostDevice();

	void ResetDevice();

	void Register(DeviceObject* deviceObject);

	void Unregister(DeviceObject* deviceObject);

	Effekseer::Backend::VertexBufferRef CreateVertexBuffer(int32_t size, const void* initialData, bool isDynamic) override;

	Effekseer::Backend::IndexBufferRef CreateIndexBuffer(int32_t elementCount, const void* initialData, Effekseer::Backend::IndexBufferStrideType stride) override;

	Effekseer::Backend::TextureRef CreateTexture(const Effekseer::Backend::TextureParameter& param, const Effekseer::CustomVector<uint8_t>& initialData) override;

	Effekseer::Backend::TextureRef CreateRenderTexture(const Effekseer::Backend::RenderTextureParameter& param) override;

	Effekseer::Backend::TextureRef CreateDepthTexture(const Effekseer::Backend::DepthTextureParameter& param) override;

	bool CopyTexture(Effekseer::Backend::TextureRef& dst, Effekseer::Backend::TextureRef& src, const std::array<int, 3>& dstPos, const std::array<int, 3>& srcPos, const std::array<int, 3>& size, int32_t dstLayer, int32_t srcLayer) override;

	Effekseer::Backend::UniformBufferRef CreateUniformBuffer(int32_t size, const void* initialData) override;

	Effekseer::Backend::ComputeBufferRef CreateComputeBuffer(int32_t elementCount, int32_t elementSize, const void* initialData, bool readOnly) override;

	Effekseer::Backend::VertexLayoutRef CreateVertexLayout(const Effekseer::Backend::VertexLayoutElement* elements, int32_t elementCount) override;

	Effekseer::Backend::RenderPassRef CreateRenderPass(Effekseer::FixedSizeVector<Effekseer::Backend::TextureRef, Effekseer::Backend::RenderTargetMax>& textures, Effekseer::Backend::TextureRef& depthTexture) override;

	Effekseer::Backend::ShaderRef CreateShaderFromBinary(const void* vsData, int32_t vsDataSize, const void* psData, int32_t psDataSize) override;

	Effekseer::Backend::ShaderRef CreateShaderFromCodes(const Effekseer::CustomVector<Effekseer::StringView<char>>& vsCodes, const Effekseer::CustomVector<Effekseer::StringView<char>>& psCodes, Effekseer::Backend::UniformLayoutRef layout = nullptr) override;

	Effekseer::Backend::ShaderRef CreateComputeShader(const void* csData, int32_t csDataSize) override;

	Effekseer::Backend::PipelineStateRef CreatePipelineState(const Effekseer::Backend::PipelineStateParameter& param) override;

	void SetViewport(int32_t x, int32_t y, int32_t width, int32_t height) override;

	void Draw(const Effekseer::Backend::DrawParameter& drawParam) override;

	void BeginRenderPass(Effekseer::Backend::RenderPassRef& renderPass, bool isColorCleared, bool isDepthCleared, Effekseer::Color clearColor) override;

	void EndRenderPass() override;

	void Dispatch(const Effekseer::Backend::DispatchParameter& dispatchParam) override;

	std::string GetDeviceName() const override
	{
		return "DirectX11";
	}

	bool UpdateUniformBuffer(Effekseer::Backend::UniformBufferRef& buffer, int32_t size, int32_t offset, const void* data) override;

	bool UpdateComputeBuffer(Effekseer::Backend::ComputeBufferRef& buffer, int32_t size, int32_t offset, const void* data) override;

	//! for DirectX11
	Effekseer::Backend::TextureRef CreateTexture(ID3D11ShaderResourceView* srv, ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsv);
};

} // namespace Backend

} // namespace EffekseerRendererDX11

#endif