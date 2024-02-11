
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
	void UpdateResults();

public: // For device restore
	virtual void OnLostDevice();
	virtual void OnResetDevice();

public: // GpuTimer
	virtual void BeginFrame() override;
	virtual void EndFrame() override;
	virtual void AddTimer(const void* object) override;
	virtual void RemoveTimer(const void* object) override;
	virtual void Start(const void* object, uint32_t phase) override;
	virtual void Stop(const void* object, uint32_t phase) override;
	virtual int32_t GetResult(const void* object) override;

private:
	RendererImplemented* m_renderer = nullptr;

	struct TimeData
	{
		GLuint timeElapsedQuery[NUM_PHASES];
		bool queryRequested[NUM_PHASES] = {};
		int32_t result = 0;
	};
	std::unordered_map<const void*, TimeData> m_timeData;
	
	enum class State {
		NoResult,
		DuringFrame,
		AfterFrame,
		ResultUpdated,
	};
	State m_state = State::NoResult;
};

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
} // namespace EffekseerRendererGL
  //-----------------------------------------------------------------------------------
  //
  //-----------------------------------------------------------------------------------