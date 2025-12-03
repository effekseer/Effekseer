
#pragma once

#include <unordered_map>
#include "GraphicsDevice.h"
#include "EffekseerRendererLLGI.RendererImplemented.h"
#include <LLGI.Query.h>

namespace EffekseerRendererLLGI
{

class GpuTimer : public ::Effekseer::GpuTimer
{
	static constexpr uint32_t NUM_PHASES = 2;
	static constexpr uint32_t NUM_TIMESTAMPS = 2;
	static constexpr uint32_t NUM_QUERIES_PER_TIMER = NUM_PHASES * NUM_TIMESTAMPS;

	static constexpr uint32_t TIMESTAMP_START = 0;
	static constexpr uint32_t TIMESTAMP_STOP = 1;
	static constexpr uint32_t QueryIndex(uint32_t phase, uint32_t timestamp) {
		return phase * NUM_TIMESTAMPS + timestamp;
	}

public:
	GpuTimer(RendererImplemented* renderer, bool hasRefCount);

	virtual ~GpuTimer();

	void UpdateResults(Effekseer::GpuStage stage);

public: // GpuTimer
	virtual void UpdateResults() override;
	virtual void BeginStage(Effekseer::GpuStage stage) override;
	virtual void EndStage(Effekseer::GpuStage stage) override;
	virtual void AddTimer(const void* object) override;
	virtual void RemoveTimer(const void* object) override;
	virtual void Start(const void* object) override;
	virtual void Stop(const void* object) override;
	virtual int32_t GetResult(const void* object) override;

private:
	RendererImplemented* renderer_ = nullptr;

	struct TimeData
	{
		LLGI::unique_ref<LLGI::Query> queries;
		Effekseer::GpuStage queryedStage[NUM_PHASES] = {};
		int32_t result = 0;
	};
	std::unordered_map<const void*, TimeData> timeData_;
	
	enum class State {
		NoResult,
		DuringStage,
		AfterStage,
		ResultUpdated,
	};
	State stageState_[8] = {};
	Effekseer::GpuStage currentStage_ = {};
};

} // namespace EffekseerRendererLLGI