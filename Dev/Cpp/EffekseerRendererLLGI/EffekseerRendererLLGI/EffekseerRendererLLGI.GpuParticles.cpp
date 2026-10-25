#include "EffekseerRendererLLGI.GpuParticles.h"
#include "EffekseerRendererLLGI.Shader.h"
#include "GraphicsDevice.h"

namespace EffekseerRendererLLGI
{

GpuParticleSystem::GpuParticleSystem(RendererImplemented* renderer)
	: EffekseerRenderer::GpuParticleSystem(renderer)
{
}

GpuParticleSystem::~GpuParticleSystem()
{
}

bool GpuParticleSystem::InitSystem(const Settings& settings)
{
	bool result = EffekseerRenderer::GpuParticleSystem::InitSystem(settings);
	if (result == false)
	{
		return false;
	}

	// Allocate ring buffer. Each slot corresponds to one frame that can be in-flight.
	auto renderer = (RendererImplemented*)renderer_;
	auto graphics = renderer->GetGraphicsDevice();
	int32_t swapBufferCount = graphics->GetGraphics()->GetSwapBufferCount();

	for (uint32_t index = 0; index < settings.EmitterMaxCount; index++)
	{
		emitters_[index].BufferFrameCount = swapBufferCount;
		emitters_[index].currentBufferIndex = 0;
		emitters_[index].currentFrameIndex = 0;

		// Free the single buffers created by base class
		emitters_[index].Buffer.Reset();
		emitters_[index].RenderConstantsBuffer.Reset();

		// Create ring buffer
		for (int f = 0; f < swapBufferCount; f++)
		{
			emitters_[index].Buffers.push_back(
				graphicsDevice_->CreateUniformBuffer(sizeof(GpuParticles::EmitterData), nullptr));
			emitters_[index].RenderConstantsBuffers.push_back(
				graphicsDevice_->CreateUniformBuffer(sizeof(GpuParticles::RenderConstants), nullptr));
		}
	}

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
