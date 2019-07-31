#include "EffekseerRendererDX12.Renderer.h"
#include "../../3rdParty/LLGI/src/DX12/LLGI.GraphicsDX12.h"
#include "../EffekseerRendererLLGI/EffekseerRendererLLGI.RendererImplemented.h"

namespace EffekseerRendererDX12
{

::EffekseerRenderer::Renderer* Create(ID3D12Device* device,
									  int32_t swapBufferCount,
									  ID3D12CommandQueue* commandQueue,
									  std::function<void()> flushAndWaitQueueFunc,
									  bool isReversedDepth,
									  int32_t squareMaxCount)
{

	/*
	ID3D12Device* device,
				 std::function<std::tuple<D3D12_CPU_DESCRIPTOR_HANDLE, ID3D12Resource*>()> getScreenFunc,
				 std::function<void()> waitFunc,
				 ID3D12CommandQueue* commandQueue,
				 int32_t swapBufferCount);
	*/
	std::function<std::tuple<D3D12_CPU_DESCRIPTOR_HANDLE, ID3D12Resource*>()> getScreenFunc =
		[]() -> std::tuple<D3D12_CPU_DESCRIPTOR_HANDLE, ID3D12Resource*> {
		return std::tuple<D3D12_CPU_DESCRIPTOR_HANDLE, ID3D12Resource*>();
	};

	auto graphics = new LLGI::GraphicsDX12(device, getScreenFunc, flushAndWaitQueueFunc, commandQueue, swapBufferCount);

	::EffekseerRendererLLGI::RendererImplemented* renderer = new ::EffekseerRendererLLGI::RendererImplemented(squareMaxCount);

	auto allocate_ = [](std::vector<LLGI::DataStructure>& ds, const unsigned char* data, int32_t size) -> void {

	};

	/*
	allocate_(renderer->fixedShader.StandardTexture_VS, g_standard_vsData, g_standard_vsLength);
	allocate_(renderer->fixedShader.Standard_VS, g_standard_vsData, g_standard_vsLength);
	allocate_(renderer->fixedShader.StandardTexture_PS, g_standard_psData, g_standard_psLength);
	allocate_(renderer->fixedShader.Standard_PS, g_standard_no_texture_psData, g_standard_no_texture_psLength);

	allocate_(renderer->fixedShader.StandardDistortedTexture_VS, g_standard_vsData, g_standard_vsLength);
	allocate_(renderer->fixedShader.StandardDistorted_VS, g_standard_vsData, g_standard_vsLength);
	allocate_(renderer->fixedShader.StandardDistortedTexture_PS, g_standard_psData, g_standard_psLength);
	allocate_(renderer->fixedShader.StandardDistorted_PS, g_standard_no_texture_psData, g_standard_no_texture_psLength);

	allocate_(renderer->fixedShader.ModelShader_VS, g_model_vsData, g_model_vsLength);
	allocate_(renderer->fixedShader.ModelShader_PS, g_model_psData, g_model_psLength);

	allocate_(renderer->fixedShader.ModelShaderLighting_VS, g_model_l_vsData, g_model_l_vsLength);
	allocate_(renderer->fixedShader.ModelShaderLighting_PS, g_model_l_psData, g_model_l_psLength);

	allocate_(renderer->fixedShader.ModelShaderTexture_VS, g_model_t_vsData, g_model_t_vsLength);
	allocate_(renderer->fixedShader.ModelShaderTexture_PS, g_model_t_psData, g_model_t_psLength);

	allocate_(renderer->fixedShader.ModelShaderLightingTexture_VS, g_model_lt_vsData, g_model_lt_vsLength);
	allocate_(renderer->fixedShader.ModelShaderLightingTexture_PS, g_model_lt_psData, g_model_lt_psLength);

	allocate_(renderer->fixedShader.ModelShaderLightingNormal_VS, g_model_ln_vsData, g_model_ln_vsLength);
	allocate_(renderer->fixedShader.ModelShaderLightingNormal_PS, g_model_ln_psData, g_model_ln_psLength);

	allocate_(renderer->fixedShader.ModelShaderLightingTextureNormal_VS, g_model_ltn_vsData, g_model_ltn_vsLength);
	allocate_(renderer->fixedShader.ModelShaderLightingTextureNormal_PS, g_model_ltn_psData, g_model_ltn_psLength);

	allocate_(renderer->fixedShader.ModelShaderDistortionTexture_VS, g_model_distortion_vsData, g_model_distortion_vsLength);
	allocate_(renderer->fixedShader.ModelShaderDistortion_VS, g_model_distortion_vsData, g_model_distortion_vsLength);
	allocate_(renderer->fixedShader.ModelShaderDistortionTexture_PS, g_model_distortion_psData, g_model_distortion_psLength);
	allocate_(renderer->fixedShader.ModelShaderDistortion_PS, g_model_distortion_no_texture_psData, g_model_distortion_no_texture_psLength);
	*/

	if (renderer->Initialize(graphics, (renderer->fixedShader_), isReversedDepth))
	{
		ES_SAFE_RELEASE(graphics);
		return renderer;
	}

	ES_SAFE_DELETE(renderer);

	return nullptr;
}

Effekseer::TextureData* CreateTextureData(::EffekseerRenderer::Renderer* renderer, ID3D12Resource* texture)
{
	auto r = static_cast<::EffekseerRendererLLGI::RendererImplemented*>(renderer);
	auto g = static_cast<LLGI::GraphicsDX12*>(r->GetGraphics());
	auto texture_ = g->CreateTexture((uint64_t)texture);

	auto textureData = new Effekseer::TextureData();
	textureData->UserPtr = texture_;
	textureData->UserID = 0;
	textureData->TextureFormat = Effekseer::TextureFormatType::ABGR8;
	textureData->Width = 0;
	textureData->Height = 0;
	return textureData;
}

void DeleteTextureData(::EffekseerRenderer::Renderer* renderer, Effekseer::TextureData* textureData)
{
	auto texture = (LLGI::Texture*)textureData->UserPtr;
	texture->Release();
	delete textureData;
}

void FlushAndWait(::EffekseerRenderer::Renderer* renderer)
{
	auto r = static_cast<::EffekseerRendererLLGI::RendererImplemented*>(renderer);
	auto g = static_cast<LLGI::GraphicsSwitch*>(r->GetGraphics());
	g->WaitFinish();
}

EffekseerRenderer::CommandList* CreateCommandList(::EffekseerRenderer::Renderer* renderer,
												  ::EffekseerRenderer::SingleFrameMemoryPool* memoryPool)
{
	auto r = static_cast<::EffekseerRendererLLGI::RendererImplemented*>(renderer);
	auto g = static_cast<LLGI::GraphicsDX12*>(r->GetGraphics());
	auto mp = static_cast<::EffekseerRendererLLGI::SingleFrameMemoryPool*>(memoryPool);
	auto commandList = g->CreateCommandList(mp->GetInternal());
	auto ret = new EffekseerRendererLLGI::CommandList(g, commandList, mp->GetInternal());
	ES_SAFE_RELEASE(commandList);
	return ret;
}

EffekseerRenderer::SingleFrameMemoryPool* CreateSingleFrameMemoryPool(::EffekseerRenderer::Renderer* renderer)
{
	auto r = static_cast<::EffekseerRendererLLGI::RendererImplemented*>(renderer);
	auto g = static_cast<LLGI::GraphicsDX12*>(r->GetGraphics());
	auto mp = g->CreateSingleFrameMemoryPool(1024 * 1024 * 8, 128);
	auto ret = new EffekseerRendererLLGI::SingleFrameMemoryPool(mp);
	ES_SAFE_RELEASE(mp);
	return ret;
}

void BeginCommandList(EffekseerRenderer::CommandList* commandList)
{
	assert(commandList != nullptr);
	auto c = static_cast<EffekseerRendererLLGI::CommandList*>(commandList);
	c->GetInternal()->Begin();
}

void EndCommandList(EffekseerRenderer::CommandList* commandList)
{
	assert(commandList != nullptr);
	auto c = static_cast<EffekseerRendererLLGI::CommandList*>(commandList);
	c->GetInternal()->End();
}

void ExecuteCommandList(EffekseerRenderer::CommandList* commandList)
{
	assert(commandList != nullptr);
	auto c = static_cast<EffekseerRendererLLGI::CommandList*>(commandList);
	c->GetGraphics()->Execute(c->GetInternal());
}

#if 0

RendererImplemented::RendererImplemented(int32_t squareMaxCount) : ::EffekseerRendererLLGI::RendererImplemented(squareMaxCount) {}

RendererImplemented::~RendererImplemented() {}

void RendererImplemented::OnLostDevice() { ::EffekseerRendererLLGI::RendererImplemented::OnLostDevice(); }

void RendererImplemented::OnResetDevice() { ::EffekseerRendererLLGI::RendererImplemented::OnResetDevice(); }

void RendererImplemented::Destroy() { ::EffekseerRendererLLGI::RendererImplemented::Destroy(); }

void RendererImplemented::SetRestorationOfStatesFlag(bool flag)
{
	::EffekseerRendererLLGI::RendererImplemented::SetRestorationOfStatesFlag(flag);
}

bool RendererImplemented::BeginRendering() { return ::EffekseerRendererLLGI::RendererImplemented::BeginRendering(); }

bool RendererImplemented::EndRendering() { return ::EffekseerRendererLLGI::RendererImplemented::EndRendering(); }

const ::Effekseer::Vector3D& RendererImplemented::GetLightDirection() const
{
	return ::EffekseerRendererLLGI::RendererImplemented::GetLightDirection();
}

void RendererImplemented::SetLightDirection(const ::Effekseer::Vector3D& direction)
{
	::EffekseerRendererLLGI::RendererImplemented::SetLightDirection(direction);
}

const ::Effekseer::Color& RendererImplemented::GetLightColor() const
{
	return ::EffekseerRendererLLGI::RendererImplemented::GetLightColor();
}

void RendererImplemented::SetLightColor(const ::Effekseer::Color& color)
{
	::EffekseerRendererLLGI::RendererImplemented::SetLightColor(color);
}

const ::Effekseer::Color& RendererImplemented::GetLightAmbientColor() const
{
	return ::EffekseerRendererLLGI::RendererImplemented::GetLightAmbientColor();
}

void RendererImplemented::SetLightAmbientColor(const ::Effekseer::Color& color)
{
	::EffekseerRendererLLGI::RendererImplemented::SetLightAmbientColor(color);
}

int32_t RendererImplemented::GetSquareMaxCount() const { return ::EffekseerRendererLLGI::RendererImplemented::GetSquareMaxCount(); }

const ::Effekseer::Matrix44& RendererImplemented::GetProjectionMatrix() const
{
	return ::EffekseerRendererLLGI::RendererImplemented::GetProjectionMatrix();
}

void RendererImplemented::SetProjectionMatrix(const ::Effekseer::Matrix44& mat)
{
	::EffekseerRendererLLGI::RendererImplemented::SetProjectionMatrix(mat);
}

const ::Effekseer::Matrix44& RendererImplemented::GetCameraMatrix() const
{
	return ::EffekseerRendererLLGI::RendererImplemented::GetCameraMatrix();
}

void RendererImplemented::SetCameraMatrix(const ::Effekseer::Matrix44& mat)
{
	::EffekseerRendererLLGI::RendererImplemented::SetCameraMatrix(mat);
}

::Effekseer::Matrix44& RendererImplemented::GetCameraProjectionMatrix()
{
	return ::EffekseerRendererLLGI::RendererImplemented::GetCameraProjectionMatrix();
}

::Effekseer::Vector3D RendererImplemented::GetCameraFrontDirection() const
{
	return ::EffekseerRendererLLGI::RendererImplemented::GetCameraFrontDirection();
}

::Effekseer::Vector3D RendererImplemented::GetCameraPosition() const
{
	return ::EffekseerRendererLLGI::RendererImplemented::GetCameraPosition();
}

void RendererImplemented::SetCameraParameter(const ::Effekseer::Vector3D& front, const ::Effekseer::Vector3D& position)
{
	::EffekseerRendererLLGI::RendererImplemented::SetCameraParameter(front, position);
}

::Effekseer::SpriteRenderer* RendererImplemented::CreateSpriteRenderer()
{
	return ::EffekseerRendererLLGI::RendererImplemented::CreateSpriteRenderer();
}

::Effekseer::RibbonRenderer* RendererImplemented::CreateRibbonRenderer()
{
	return ::EffekseerRendererLLGI::RendererImplemented::CreateRibbonRenderer();
}

::Effekseer::RingRenderer* RendererImplemented::CreateRingRenderer()
{
	return ::EffekseerRendererLLGI::RendererImplemented::CreateRingRenderer();
}

::Effekseer::ModelRenderer* RendererImplemented::CreateModelRenderer()
{
	return ::EffekseerRendererLLGI::RendererImplemented::CreateModelRenderer();
}

::Effekseer::TrackRenderer* RendererImplemented::CreateTrackRenderer()
{
	return ::EffekseerRendererLLGI::RendererImplemented::CreateTrackRenderer();
}

::Effekseer::TextureLoader* RendererImplemented::CreateTextureLoader(::Effekseer::FileInterface* fileInterface)
{
	return ::EffekseerRendererLLGI::RendererImplemented::CreateTextureLoader(fileInterface);
}

::Effekseer::ModelLoader* RendererImplemented::CreateModelLoader(::Effekseer::FileInterface* fileInterface)
{
	return ::EffekseerRendererLLGI::RendererImplemented::CreateModelLoader(fileInterface);
}

::Effekseer::MaterialLoader* RendererImplemented::CreateMaterialLoader(::Effekseer::FileInterface* fileInterface)
{
	return ::EffekseerRendererLLGI::RendererImplemented::CreateMaterialLoader(fileInterface);
}

void RendererImplemented::ResetRenderState() { return ::EffekseerRendererLLGI::RendererImplemented::ResetRenderState(); }

EffekseerRenderer::DistortingCallback* RendererImplemented::GetDistortingCallback()
{
	return ::EffekseerRendererLLGI::RendererImplemented::GetDistortingCallback();
}

void RendererImplemented::SetDistortingCallback(EffekseerRenderer::DistortingCallback* callback)
{
	::EffekseerRendererLLGI::RendererImplemented::SetDistortingCallback(callback);
}

int32_t RendererImplemented::GetDrawCallCount() const { return ::EffekseerRendererLLGI::RendererImplemented::GetDrawCallCount(); }

int32_t RendererImplemented::GetDrawVertexCount() const { return ::EffekseerRendererLLGI::RendererImplemented::GetDrawVertexCount(); }

void RendererImplemented::ResetDrawCallCount() { ::EffekseerRendererLLGI::RendererImplemented::ResetDrawCallCount(); }

void RendererImplemented::ResetDrawVertexCount() { ::EffekseerRendererLLGI::RendererImplemented::ResetDrawVertexCount(); }

void RendererImplemented::SetRenderMode(Effekseer::RenderMode renderMode)
{
	::EffekseerRendererLLGI::RendererImplemented::SetRenderMode(renderMode);
}

Effekseer::RenderMode RendererImplemented::GetRenderMode() { return ::EffekseerRendererLLGI::RendererImplemented::GetRenderMode(); }

#endif

} // namespace EffekseerRendererDX12