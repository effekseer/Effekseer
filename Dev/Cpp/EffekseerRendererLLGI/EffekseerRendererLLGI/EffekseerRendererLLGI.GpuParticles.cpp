#include "EffekseerRendererLLGI.GpuParticles.h"
#include "EffekseerRendererLLGI.Shader.h"

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

	auto renderer = (RendererImplemented*)m_rendererBase;
	auto graphics = renderer->GetGraphicsDevice();

	const auto& csClear = Backend::Serialize(renderer->fixedShader_.GpuParticles_Clear_CS);
	const auto& csSpawn = Backend::Serialize(renderer->fixedShader_.GpuParticles_Spawn_CS);
	const auto& csUpdate = Backend::Serialize(renderer->fixedShader_.GpuParticles_Update_CS);
	const auto& vsRender = Backend::Serialize(renderer->fixedShader_.GpuParticles_Render_VS);
	const auto& psRender = Backend::Serialize(renderer->fixedShader_.GpuParticles_Render_PS);

	Shaders shaders;
	shaders.csParticleClear = graphics->CreateComputeShader(csClear.data(), (int32_t)csClear.size());
	shaders.csParticleSpawn = graphics->CreateComputeShader(csSpawn.data(), (int32_t)csSpawn.size());
	shaders.csParticleUpdate = graphics->CreateComputeShader(csUpdate.data(), (int32_t)csUpdate.size());

	shaders.rsParticleRender = graphics->CreateShaderFromBinary(
		vsRender.data(), (int32_t)vsRender.size(),
		psRender.data(), (int32_t)psRender.size());

	SetShaders(shaders);

	return true;
}

}
