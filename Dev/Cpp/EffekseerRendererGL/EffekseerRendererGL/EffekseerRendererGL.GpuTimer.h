
#pragma once

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <unordered_map>
#include "EffekseerRendererGL.DeviceObject.h"
#include "EffekseerRendererGL.RendererImplemented.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererGL
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
class GpuTimer : public DeviceObject, public ::Effekseer::GpuTimer
{
	static constexpr uint32_t NUM_PHASES = 2;

public:
	GpuTimer(RendererImplemented* renderer);

	virtual ~GpuTimer();

	void InitDevice();
	void ReleaseDevice();
	void UpdateResults(Effekseer::GpuStage stage);

public: // For device restore
	virtual void OnLostDevice();
	virtual void OnResetDevice();

public: // GpuTimer
	virtual void BeginStage(Effekseer::GpuStage stage) override;
	virtual void EndStage(Effekseer::GpuStage stage) override;
	virtual void AddTimer(const void* object) override;
	virtual void RemoveTimer(const void* object) override;
	virtual void Start(const void* object) override;
	virtual void Stop(const void* object) override;
	virtual int32_t GetResult(const void* object) override;

private:
	RendererImplemented* m_renderer = nullptr;

	struct TimeData
	{
		GLuint timeElapsedQuery[NUM_PHASES] = {};
		Effekseer::GpuStage queryedStage[NUM_PHASES] = {};
		int32_t result = 0;
	};
	std::unordered_map<const void*, TimeData> m_timeData;
	
	enum class State : uint8_t {
		NoResult,
		DuringStage,
		AfterStage,
		ResultUpdated,
	};
	State m_stageState[8] = {};
	Effekseer::GpuStage m_currentStage = {};
};

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
} // namespace EffekseerRendererGL
  //-----------------------------------------------------------------------------------
  //
  //-----------------------------------------------------------------------------------