
#pragma once

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <unordered_map>
#include "EffekseerRendererDX11.DeviceObject.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererDX11
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
class GPUTimer : public DeviceObject, public ::Effekseer::GPUTimer
{
	static constexpr uint32_t NUM_PHASES = 2;

public:
	GPUTimer(RendererImplemented* renderer, bool hasRefCount);

	virtual ~GPUTimer();

	void InitDevice();
	void ReleaseDevice();
	void UpdateResults();

public: // For device restore
	virtual void OnLostDevice();
	virtual void OnResetDevice();

public: // GPUTimer
	virtual void BeginFrame() override;
	virtual void EndFrame() override;
	virtual void AddTimer(const void* object) override;
	virtual void RemoveTimer(const void* object) override;
	virtual void Start(const void* object, uint32_t phase) override;
	virtual void Stop(const void* object, uint32_t phase) override;
	virtual int32_t GetResult(const void* object) override;

private:
	struct TimeData
	{
		Backend::D3D11QueryPtr startQuery[NUM_PHASES];
		Backend::D3D11QueryPtr stopQuery[NUM_PHASES];
		bool queryRequested[NUM_PHASES] = {};
		int32_t result = 0;
	};
	std::unordered_map<const void*, TimeData> m_timeData;
	Backend::D3D11QueryPtr m_disjoint;
	
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
} // namespace EffekseerRendererDX11
  //-----------------------------------------------------------------------------------
  //
  //-----------------------------------------------------------------------------------