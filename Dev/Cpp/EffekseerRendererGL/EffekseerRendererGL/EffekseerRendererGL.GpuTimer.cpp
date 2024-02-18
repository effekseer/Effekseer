﻿
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
#include "EffekseerRendererGL.GpuTimer.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererGL
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
GpuTimer::GpuTimer(RendererImplemented* renderer)
	: DeviceObject(renderer->GetInternalGraphicsDevice().Get())
	, m_renderer(renderer)
{
	m_renderer->GetStandardRenderer()->UpdateGpuTimerCount(+1);
	m_renderer->AddRef();

	InitDevice();
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
GpuTimer::~GpuTimer()
{
	ReleaseDevice();

	m_renderer->GetStandardRenderer()->UpdateGpuTimerCount(-1);
	m_renderer->Release();
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void GpuTimer::InitDevice()
{
	for (auto& kv : m_timeData)
	{
		TimeData& timeData = kv.second;
		GLExt::glGenQueries(NUM_PHASES, timeData.timeElapsedQuery);
	}
	GLCheckError();
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void GpuTimer::ReleaseDevice()
{
	for (auto& kv : m_timeData)
	{
		TimeData& timeData = kv.second;
		GLExt::glDeleteQueries(NUM_PHASES, timeData.timeElapsedQuery);
	}
	GLCheckError();
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void GpuTimer::OnLostDevice()
{
	ReleaseDevice();
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void GpuTimer::OnResetDevice()
{
	InitDevice();
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void GpuTimer::BeginFrame()
{
	assert(m_state != State::DuringFrame);

	if (m_state == State::AfterFrame)
	{
		UpdateResults();
	}

	m_state = State::DuringFrame;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void GpuTimer::EndFrame()
{
	assert(m_state == State::DuringFrame);

	m_state = State::AfterFrame;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void GpuTimer::UpdateResults()
{
	for (auto& kv : m_timeData)
	{
		auto& timeData = kv.second;
		timeData.result = 0;

		uint64_t elapsedTime = 0;
		for (uint32_t i = 0; i < NUM_PHASES; i++)
		{
			if (timeData.queryRequested[i])
			{
				uint64_t result = 0;
				GLExt::glGetQueryObjectui64v(timeData.timeElapsedQuery[i], GL_QUERY_RESULT, &result);
				elapsedTime += result / 1000; // nanoseconds -> microseconds
				timeData.queryRequested[i] = false;
			}
		}
		timeData.result = static_cast<int32_t>(elapsedTime);
	}

	m_state = State::ResultUpdated;

	GLCheckError();
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void GpuTimer::AddTimer(const void* object)
{
	assert(m_timeData.find(object) == m_timeData.end());

	TimeData timeData;
	GLExt::glGenQueries(NUM_PHASES, timeData.timeElapsedQuery);

	m_timeData.emplace(object, std::move(timeData));

	GLCheckError();
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void GpuTimer::RemoveTimer(const void* object)
{
	auto it = m_timeData.find(object);
	if (it != m_timeData.end())
	{
		TimeData& timeData = it->second;
		GLExt::glDeleteQueries(NUM_PHASES, timeData.timeElapsedQuery);
		m_timeData.erase(it);
	}

	GLCheckError();
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void GpuTimer::Start(const void* object, uint32_t phase)
{
	assert(phase < NUM_PHASES);

	auto it = m_timeData.find(object);
	if (it != m_timeData.end())
	{
		TimeData& timeData = it->second;
		GLExt::glBeginQuery(GL_TIME_ELAPSED, timeData.timeElapsedQuery[phase]);
		timeData.queryRequested[phase] = true;
	}

	GLCheckError();
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void GpuTimer::Stop(const void* object, uint32_t phase)
{
	assert(phase < NUM_PHASES);

	auto it = m_timeData.find(object);
	if (it != m_timeData.end())
	{
		TimeData& timeData = it->second;
		GLExt::glEndQuery(GL_TIME_ELAPSED);
	}

	GLCheckError();
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
int32_t GpuTimer::GetResult(const void* object)
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
} // namespace EffekseerRendererGL
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------