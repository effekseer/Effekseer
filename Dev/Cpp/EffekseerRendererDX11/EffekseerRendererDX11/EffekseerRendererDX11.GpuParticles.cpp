#define NOMINMAX
#include "EffekseerRendererDX11.GpuParticles.h"
#include "../Effekseer/Effekseer/Noise/Effekseer.CurlNoise.h"
#include "EffekseerRendererDX11.RenderState.h"
#include "EffekseerRendererDX11.Shader.h"

namespace EffekseerRendererDX11
{

namespace
{
#define ShaderData(shaderName) shaderName::g_main
#define ShaderSize(shaderName) sizeof(shaderName::g_main)

namespace CS_ParticleClear
{
#include "ShaderHeader/gpu_particles_clear_cs.h"
}

namespace CS_ParticleSpawn
{
#include "ShaderHeader/gpu_particles_spawn_cs.h"
}

namespace CS_ParticleUpdate
{
#include "ShaderHeader/gpu_particles_update_cs.h"
}

namespace VS_ParticleRender
{
#include "ShaderHeader/gpu_particles_render_vs.h"
}

namespace PS_ParticleRender
{
#include "ShaderHeader/gpu_particles_render_ps.h"
}

} // namespace

GpuParticleSystem::GpuParticleSystem(RendererImplemented* renderer, bool has_ref_count)
	: DeviceObject(renderer, has_ref_count)
	, EffekseerRenderer::GpuParticleSystem(renderer)
{
}

GpuParticleSystem::~GpuParticleSystem()
{
}

void GpuParticleSystem::OnLostDevice()
{
}

void GpuParticleSystem::OnResetDevice()
{
}

bool GpuParticleSystem::InitSystem(const Settings& settings)
{
	bool result = EffekseerRenderer::GpuParticleSystem::InitSystem(settings);
	if (result == false)
	{
		return false;
	}

	auto graphics = GetRenderer()->GetGraphicsDevice();

	Shaders shaders;
	shaders.CsParticleClear = graphics->CreateComputeShader(ShaderData(CS_ParticleClear), ShaderSize(CS_ParticleClear));
	shaders.CsParticleSpawn = graphics->CreateComputeShader(ShaderData(CS_ParticleSpawn), ShaderSize(CS_ParticleSpawn));
	shaders.CsParticleUpdate = graphics->CreateComputeShader(ShaderData(CS_ParticleUpdate), ShaderSize(CS_ParticleUpdate));

	shaders.RsParticleRender = graphics->CreateShaderFromBinary(
		ShaderData(VS_ParticleRender), ShaderSize(VS_ParticleRender), ShaderData(PS_ParticleRender), ShaderSize(PS_ParticleRender));

	SetShaders(shaders);

	return true;
}

} // namespace EffekseerRendererDX11
