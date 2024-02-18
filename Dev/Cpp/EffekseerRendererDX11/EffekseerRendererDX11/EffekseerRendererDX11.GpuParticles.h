#pragma once

#include "EffekseerRendererDX11.DeviceObject.h"
#include <EffekseerRendererCommon/EffekseerRenderer.GpuParticles.h>
#include <random>

namespace EffekseerRendererDX11
{

class GpuParticleSystem
	: public DeviceObject,
	  public EffekseerRenderer::GpuParticleSystem
{
public:
	GpuParticleSystem(RendererImplemented* renderer, bool hasRefCount);

	virtual ~GpuParticleSystem();

public: // GpuParticles
	virtual bool InitSystem(const Settings& settings) override;

public: // For device restore
	virtual void OnLostDevice();
	virtual void OnResetDevice();
};

} // namespace EffekseerRendererDX11
