#pragma once

#include "EffekseerRendererLLGI.RendererImplemented.h"
#include <EffekseerRendererCommon/EffekseerRenderer.GpuParticles.h>

namespace EffekseerRendererLLGI
{

class GpuParticleSystem
	: public EffekseerRenderer::GpuParticleSystem
{
public:
	GpuParticleSystem(RendererImplemented* renderer);

	virtual ~GpuParticleSystem();

public: // GpuParticles
	virtual bool InitSystem(const Settings& settings) override;
};

} // namespace EffekseerRendererLLGI
