
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
#include "EffekseerRendererDX11.GPUTimer.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererDX11
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
GPUTimer::GPUTimer(RendererImplemented* renderer, bool hasRefCount)
	: DeviceObject(renderer, hasRefCount)
{
	if (auto renderer = GetRenderer())
	{
		renderer->GetStandardRenderer()->UpdateGPUTimerCount(+1);
	}

	InitDevice();
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
GPUTimer::~GPUTimer()
{
	ReleaseDevice();

	if (auto renderer = GetRenderer())
	{
		renderer->GetStandardRenderer()->UpdateGPUTimerCount(-1);
	}
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void GPUTimer::InitDevice()
{
	auto device = GetRenderer()->GetDevice();

	{
		D3D11_QUERY_DESC desc = {};
		desc.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;
		ID3D11Query* disjoint = nullptr;
		device->CreateQuery(&desc, &disjoint);
		m_disjoint.reset(disjoint);
	}

	for (auto& kv : m_timeData)
	{
		TimeData& timeData = kv.second;

		D3D11_QUERY_DESC desc = {};
		desc.Query = D3D11_QUERY_TIMESTAMP;

		for (uint32_t i = 0; i < NUM_PHASES; i++)
		{
			ID3D11Query* startQuery = nullptr;
			device->CreateQuery(&desc, &startQuery);
			timeData.startQuery[i].reset(startQuery);

			ID3D11Query* stopQuery = nullptr;
			device->CreateQuery(&desc, &stopQuery);
			timeData.stopQuery[i].reset(stopQuery);
		}
	}
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void GPUTimer::ReleaseDevice()
{
	for (auto& kv : m_timeData)
	{
		TimeData& timeData = kv.second;
		for (uint32_t i = 0; i < NUM_PHASES; i++)
		{
			timeData.startQuery[i].reset();
			timeData.stopQuery[i].reset();
		}
	}

	m_disjoint.reset();
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void GPUTimer::OnLostDevice()
{
	ReleaseDevice();
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void GPUTimer::OnResetDevice()
{
	InitDevice();
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void GPUTimer::BeginFrame()
{
	assert(m_state != State::DuringFrame);

	if (m_state == State::AfterFrame)
	{
		// Avoid D3D11 warning
		UpdateResults();
	}

	auto context = GetRenderer()->GetContext();
	context->Begin(m_disjoint.get());
	m_state = State::DuringFrame;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void GPUTimer::EndFrame()
{
	assert(m_state == State::DuringFrame);

	auto context = GetRenderer()->GetContext();
	context->End(m_disjoint.get());
	m_state = State::AfterFrame;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void GPUTimer::UpdateResults()
{
	HRESULT hr = S_OK;
	auto context = GetRenderer()->GetContext();

	D3D11_QUERY_DATA_TIMESTAMP_DISJOINT disjoint = {1, TRUE};
	hr = context->GetData(m_disjoint.get(), &disjoint, sizeof(disjoint), 0);
	if (hr != S_OK)
	{
		return;
	}


	auto getTimeStamp = [context](ID3D11Query* query, uint64_t& time)
	{
		while (true)
		{
			HRESULT hr = context->GetData(query, &time, sizeof(time), 0);
			if (hr == S_OK)
			{
				return true;
			}
			else if (hr == S_FALSE)
			{
				Sleep(1);
			}
			else
			{
				return false;
			}
		}
	};

	for (auto& kv : m_timeData)
	{
		auto& timeData = kv.second;
		timeData.result = 0;

		uint64_t elapsedTime = 0;
		for (uint32_t i = 0; i < NUM_PHASES; i++)
		{
			if (timeData.queryRequested[i])
			{
				uint64_t startTime, stopTime;
				if (!getTimeStamp(timeData.startQuery[i].get(), startTime)) continue;
				if (!getTimeStamp(timeData.stopQuery[i].get(), stopTime)) continue;
				elapsedTime += (stopTime - startTime) * 1000000 / disjoint.Frequency;
				timeData.queryRequested[i] = false;
			}
		}
		timeData.result = static_cast<int32_t>(elapsedTime);
	}

	m_state = State::ResultUpdated;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void GPUTimer::AddTimer(const void* object)
{
	assert(m_timeData.find(object) == m_timeData.end());

	auto device = GetRenderer()->GetDevice();

	D3D11_QUERY_DESC desc = {};
	desc.Query = D3D11_QUERY_TIMESTAMP;

	TimeData timeData;
	for (uint32_t i = 0; i < NUM_PHASES; i++)
	{
		ID3D11Query* startQuery = nullptr;
		device->CreateQuery(&desc, &startQuery);
		timeData.startQuery[i].reset(startQuery);

		ID3D11Query* stopQuery = nullptr;
		device->CreateQuery(&desc, &stopQuery);
		timeData.stopQuery[i].reset(stopQuery);
	}

	m_timeData.emplace(object, std::move(timeData));
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void GPUTimer::RemoveTimer(const void* object)
{
	m_timeData.erase(object);
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void GPUTimer::Start(const void* object, uint32_t phase)
{
	assert(phase < NUM_PHASES);

	auto context = GetRenderer()->GetContext();

	auto it = m_timeData.find(object);
	if (it != m_timeData.end())
	{
		TimeData& timeData = it->second;
		context->End(timeData.startQuery[phase].get());
		timeData.queryRequested[phase] = true;
	}
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void GPUTimer::Stop(const void* object, uint32_t phase)
{
	assert(phase < NUM_PHASES);

	auto context = GetRenderer()->GetContext();

	auto it = m_timeData.find(object);
	if (it != m_timeData.end())
	{
		TimeData& timeData = it->second;
		context->End(timeData.stopQuery[phase].get());
	}
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
int32_t GPUTimer::GetResult(const void* object)
{
	assert(m_state == State::ResultUpdated || m_state == State::AfterFrame);

	if (m_state == State::AfterFrame)
	{
		UpdateResults();
	}

	auto it = m_timeData.find(object);
	if (it != m_timeData.end())
	{
		TimeData& timeData = it->second;
		return timeData.result;
	}
	return 0;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
} // namespace EffekseerRendererDX11
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
