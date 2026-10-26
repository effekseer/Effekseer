#pragma once

#include "EffekseerRendererLLGI.RendererImplemented.h"
#include <EffekseerRendererCommon/EffekseerRenderer.GpuParticles.h>

namespace EffekseerRendererLLGI
{

class GpuParticleSystem
	: public EffekseerRenderer::GpuParticleSystem
{
protected:
	bool UploadConstants(Effekseer::Backend::UniformBufferRef& buffer, int32_t size, const void* data) override;

public:
	GpuParticleSystem(RendererImplemented* renderer);

	virtual ~GpuParticleSystem();

public: // GpuParticles
	virtual bool InitSystem(const Settings& settings) override;
};

} // namespace EffekseerRendererLLGI
