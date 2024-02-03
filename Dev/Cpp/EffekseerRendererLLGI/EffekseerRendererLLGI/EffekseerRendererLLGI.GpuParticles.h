#pragma once

#include "EffekseerRendererLLGI.RendererImplemented.h"
#include <EffekseerRendererCommon/EffekseerRenderer.GpuParticles.h>

namespace EffekseerRendererLLGI
{

class GpuParticles
	: public EffekseerRenderer::GpuParticles
{
public:
	GpuParticles(RendererImplemented* renderer);

	virtual ~GpuParticles();

public: // GpuParticles
	virtual bool InitSystem(const Settings& settings) override;
};

} // namespace EffekseerRendererLLGI
