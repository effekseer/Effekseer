#pragma once

#include "EffekseerRendererLLGI.RendererImplemented.h"
#include <EffekseerRendererCommon/EffekseerRenderer.GpuParticles.h>
#include <map>

namespace EffekseerRendererLLGI
{

class GpuParticleSystem
	: public EffekseerRenderer::GpuParticleSystem
{
private:
	struct FramePool
	{
		std::shared_ptr<LLGI::SingleFrameMemoryPool> Memory;
		uint64_t Frame = 0;
	};
	// Usually one entry. Different frame owners must never reset each other's pool.
	std::map<std::shared_ptr<uint64_t>, FramePool> constantPools_;
	int32_t constantBufferSize_;

protected:
	bool UploadConstants(Effekseer::Backend::UniformBufferRef& buffer, int32_t size, const void* data) override;

public:
	GpuParticleSystem(RendererImplemented* renderer, int32_t constantBufferSize = 1024 * 1024);

	virtual ~GpuParticleSystem();

public: // GpuParticles
	virtual bool InitSystem(const Settings& settings) override;
};

} // namespace EffekseerRendererLLGI
