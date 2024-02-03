#pragma once

#include "EffekseerRendererDX11.DeviceObject.h"
#include <EffekseerRendererCommon/EffekseerRenderer.GpuParticles.h>
#include <random>

namespace EffekseerRendererDX11
{

class GpuParticles
	: public DeviceObject,
	  public EffekseerRenderer::GpuParticles
{
public:
	GpuParticles(RendererImplemented* renderer, bool hasRefCount);

	virtual ~GpuParticles();

public: // GpuParticles
	virtual bool InitSystem(const Settings& settings) override;

public: // For device restore
	virtual void OnLostDevice();
	virtual void OnResetDevice();
};

} // namespace EffekseerRendererDX11
