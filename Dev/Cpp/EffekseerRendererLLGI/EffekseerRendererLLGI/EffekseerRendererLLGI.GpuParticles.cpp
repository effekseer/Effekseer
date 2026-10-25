#include "EffekseerRendererLLGI.GpuParticles.h"
#include "EffekseerRendererLLGI.Shader.h"
#include "GraphicsDevice.h"

namespace EffekseerRendererLLGI
{

GpuParticleSystem::GpuParticleSystem(RendererImplemented* renderer, int32_t constantBufferSize)
	: EffekseerRenderer::GpuParticleSystem(renderer)
	, constantBufferSize_(constantBufferSize)
{
}

GpuParticleSystem::~GpuParticleSystem()
{
}

bool GpuParticleSystem::UploadConstants(Effekseer::Backend::UniformBufferRef& buffer, int32_t size, const void* data)
{
	auto renderer = static_cast<RendererImplemented*>(renderer_);
	auto commandList = renderer->GetCommandList();
	auto frameID = commandList ? commandList->GetFrameID() : nullptr;
	if (!frameID) return false;
	auto& pool = constantPools_[frameID];
	if (!pool.Memory)
	{
		pool.Memory = LLGI::CreateSharedPtr(renderer->GetGraphics()->CreateSingleFrameMemoryPool(constantBufferSize_, 128));
		if (!pool.Memory || pool.Memory->GetSwapBufferCount() < commandList->GetMemoryPool()->GetSwapBufferCount())
		{
			pool.Memory.reset();
			return false;
		}
		pool.Memory->NewFrame();
		pool.Frame = *frameID;
	}
	else if (pool.Frame != *frameID)
	{
		// Advance only with the application's frame pool, never per ComputeFrame.
		pool.Memory->NewFrame();
		pool.Frame = *frameID;
	}

	auto allocation = LLGI::CreateSharedPtr(pool.Memory->CreateConstantBuffer(size));
	if (!allocation) return false; // Fixed capacity: abandon this compute/draw on exhaustion.
	auto dst = allocation->Lock();
	if (!dst) return false;
	memcpy(dst, data, size);
	allocation->Unlock();
	if (!buffer)
	{
		buffer = Effekseer::MakeRefPtr<Backend::UniformBuffer>(renderer->GetGraphicsDeviceInternal().Get());
	}
	buffer.DownCast<Backend::UniformBuffer>()->SetBuffer(std::move(allocation));
	return true;
}

bool GpuParticleSystem::InitSystem(const Settings& settings)
{
	bool result = EffekseerRenderer::GpuParticleSystem::InitSystem(settings);
	if (result == false)
	{
		return false;
	}

	auto renderer = (RendererImplemented*)renderer_;
	auto graphics = renderer->GetGraphicsDevice();

	const auto& csClear = Backend::Serialize(renderer->fixedShader_.GpuParticles_Clear_CS);
	const auto& csSpawn = Backend::Serialize(renderer->fixedShader_.GpuParticles_Spawn_CS);
	const auto& csUpdate = Backend::Serialize(renderer->fixedShader_.GpuParticles_Update_CS);
	const auto& vsRender = Backend::Serialize(renderer->fixedShader_.GpuParticles_Render_VS);
	const auto& psRender = Backend::Serialize(renderer->fixedShader_.GpuParticles_Render_PS);

	Shaders shaders;
	shaders.CsParticleClear = graphics->CreateComputeShader(csClear.data(), (int32_t)csClear.size());
	shaders.CsParticleSpawn = graphics->CreateComputeShader(csSpawn.data(), (int32_t)csSpawn.size());
	shaders.CsParticleUpdate = graphics->CreateComputeShader(csUpdate.data(), (int32_t)csUpdate.size());

	shaders.RsParticleRender = graphics->CreateShaderFromBinary(
		vsRender.data(), (int32_t)vsRender.size(), psRender.data(), (int32_t)psRender.size());

	if (shaders.CsParticleClear == nullptr || shaders.CsParticleSpawn == nullptr || shaders.CsParticleUpdate == nullptr ||
		shaders.RsParticleRender == nullptr)
	{
		return false;
	}

	if (!SetShaders(shaders))
	{
		return false;
	}

	auto validatePipeline = [](const Effekseer::Backend::PipelineStateRef& pipeline, LLGI::RenderPassPipelineState* renderPass)
	{
		auto llgiPipeline = pipeline.DownCast<Backend::PipelineState>();
		return llgiPipeline != nullptr && llgiPipeline->GetOrCreatePipelineState(renderPass) != nullptr;
	};

	EffekseerRenderer::GpuParticles::PipelineStateKey renderKey{};
	auto renderPipeline = GetOrCreatePipelineState(renderKey);
	auto renderPass = renderer->GetCurrentRenderPassPipelineState();
	return validatePipeline(pipelineParticleClear_, nullptr) && validatePipeline(pipelineParticleSpawn_, nullptr) &&
		   validatePipeline(pipelineParticleUpdate_, nullptr) && renderPass != nullptr &&
		   validatePipeline(renderPipeline, renderPass.get());
}

} // namespace EffekseerRendererLLGI
