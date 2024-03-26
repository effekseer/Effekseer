
#ifndef __EFFEKSEER_GPU_TIMER_H__
#define __EFFEKSEER_GPU_TIMER_H__

#include "../Effekseer.Base.Pre.h"

namespace Effekseer
{

class GpuTimer;
using GpuTimerRef = RefPtr<GpuTimer>;

enum class GpuStage : uint8_t
{
	None,
	Compute,
	Draw,
	DrawBack,
	DrawFront,
};

class GpuTimer : public ReferenceObject
{
public:
	GpuTimer() = default;

	virtual ~GpuTimer() = default;

	virtual void UpdateResults() {}

	virtual void BeginStage(GpuStage stage) {}

	virtual void EndStage(GpuStage stage) {}

	virtual void AddTimer(const void* object) {}

	virtual void RemoveTimer(const void* object) {}

	virtual void Start(const void* object) {}

	virtual void Stop(const void* object) {}

	virtual int32_t GetResult(const void* object) { return -1; }
};

class ScopedGpuStage
{
public:
	ScopedGpuStage(GpuTimerRef timer, GpuStage stage)
		: m_timer(timer), m_stage(stage)
	{
		if (m_timer)
		{
			m_timer->BeginStage(m_stage);
		}
	}

	~ScopedGpuStage()
	{
		if (m_timer)
		{
			m_timer->EndStage(m_stage);
		}
	}

private:
	GpuTimerRef m_timer;
	GpuStage m_stage;
};

class ScopedGpuTime
{
public:
	ScopedGpuTime(GpuTimerRef timer, const void* object)
		: m_timer(timer), m_object(object)
	{
		if (m_timer)
		{
			m_timer->Start(m_object);
		}
	}

	~ScopedGpuTime()
	{
		if (m_timer)
		{
			m_timer->Stop(m_object);
		}
	}

private:
	GpuTimerRef m_timer;
	const void* m_object;
};

} // namespace Effekseer

#endif // __EFFEKSEER_GPU_TIMER_H__
