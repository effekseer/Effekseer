#pragma once

#include "EffekseerRendererDX9.RendererImplemented.h"
#include <unordered_map>

namespace EffekseerRendererDX9
{

class GpuTimer : public Backend::DeviceObject, public ::Effekseer::GpuTimer
{
	static constexpr uint32_t NUM_PHASES = 2;

public:
	GpuTimer(RendererImplemented* renderer);
	~GpuTimer() override;

	bool GetIsValid() const
	{
		return isValid_;
	}

	void InitDevice();
	void ReleaseDevice();
	void UpdateResults(Effekseer::GpuStage stage);

public:
	void OnLostDevice() override;
	void OnChangeDevice() override;
	void OnResetDevice() override;

public: // GpuTimer
	void UpdateResults() override;
	void BeginStage(Effekseer::GpuStage stage) override;
	void EndStage(Effekseer::GpuStage stage) override;
	void AddTimer(const void* object) override;
	void RemoveTimer(const void* object) override;
	void Start(const void* object) override;
	void Stop(const void* object) override;
	int32_t GetResult(const void* object) override;

private:
	struct TimeData
	{
		Backend::Direct3DQuery9Ptr startQuery[NUM_PHASES];
		Backend::Direct3DQuery9Ptr stopQuery[NUM_PHASES];
		Effekseer::GpuStage queryedStage[NUM_PHASES] = {};
		int32_t result = 0;
	};

	enum class State : uint8_t
	{
		NoResult,
		DuringStage,
		AfterStage,
		ResultUpdated,
	};

	bool CreateTimestampQuery(Backend::Direct3DQuery9Ptr& query);
	bool CreateStageQueries(uint32_t index);
	bool WaitForData(IDirect3DQuery9* query, void* data, DWORD size);

	RendererImplemented* renderer_ = nullptr;
	bool isValid_ = false;

	std::unordered_map<const void*, TimeData> timeData_;

	Backend::Direct3DQuery9Ptr disjoint_[8];
	Backend::Direct3DQuery9Ptr frequency_[8];

	State stageState_[8] = {};
	Effekseer::GpuStage currentStage_ = {};
};

} // namespace EffekseerRendererDX9
