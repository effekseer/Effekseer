
#ifndef __EFFEKSEER_GPU_TIMER_H__
#define __EFFEKSEER_GPU_TIMER_H__

#include "../Effekseer.Base.Pre.h"

namespace Effekseer
{

class GpuTimer;
using GpuTimerRef = RefPtr<GpuTimer>;

class GpuTimer : public ReferenceObject
{
public:
	GpuTimer() = default;

	virtual ~GpuTimer() = default;

	virtual void BeginFrame() {}

	virtual void EndFrame() {}

	virtual void AddTimer(const void* object) {}

	virtual void RemoveTimer(const void* object) {}

	virtual void Start(const void* object, uint32_t phase) {}

	virtual void Stop(const void* object, uint32_t phase) {}

	virtual int32_t GetResult(const void* object) { return -1; }
};

} // namespace Effekseer

#endif // __EFFEKSEER_GPU_TIMER_H__
