
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
void GpuTimer::BeginStage(Effekseer::GpuStage stage)
{
	assert(stage != Effekseer::GpuStage::None);

	uint32_t index = static_cast<uint32_t>(stage);
	assert(m_stageState[index] != State::DuringStage);

	if (m_stageState[index] == State::AfterStage)
	{
		UpdateResults(stage);
	}

	m_stageState[index] = State::DuringStage;
	m_currentStage = stage;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void GpuTimer::EndStage(Effekseer::GpuStage stage)
{
	assert(stage != Effekseer::GpuStage::None);

	uint32_t index = static_cast<uint32_t>(stage);
	assert(m_stageState[index] == State::DuringStage);

	m_stageState[index] = State::AfterStage;
	m_currentStage = Effekseer::GpuStage::None;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void GpuTimer::UpdateResults(Effekseer::GpuStage stage)
{
	assert(stage != Effekseer::GpuStage::None);
	uint32_t index = static_cast<uint32_t>(stage);

	for (auto& kv : m_timeData)
	{
		auto& timeData = kv.second;
		timeData.result = 0;

		uint64_t elapsedTime = 0;
		for (uint32_t phase = 0; phase < NUM_PHASES; phase++)
		{
			if (timeData.queryedStage[phase] == stage)
			{
				uint64_t result = 0;
				GLExt::glGetQueryObjectui64v(timeData.timeElapsedQuery[phase], GL_QUERY_RESULT, &result);
				elapsedTime += result / 1000; // nanoseconds -> microseconds
				timeData.queryedStage[phase] = Effekseer::GpuStage::None;
			}
		}
		timeData.result = static_cast<int32_t>(elapsedTime);
	}

	m_stageState[index] = State::ResultUpdated;

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
void GpuTimer::Start(const void* object)
{
	assert(m_currentStage != Effekseer::GpuStage::None);

	auto it = m_timeData.find(object);
	if (it != m_timeData.end())
	{
		TimeData& timeData = it->second;
		for (uint32_t phase = 0; phase < NUM_PHASES; phase++)
		{
			if (timeData.queryedStage[phase] == Effekseer::GpuStage::None)
			{
				GLExt::glBeginQuery(GL_TIME_ELAPSED, timeData.timeElapsedQuery[phase]);
				timeData.queryedStage[phase] = m_currentStage;
				break;
			}
		}
	}

	GLCheckError();
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void GpuTimer::Stop(const void* object)
{
	assert(m_currentStage != Effekseer::GpuStage::None);

	auto it = m_timeData.find(object);
	if (it != m_timeData.end())
	{
		TimeData& timeData = it->second;
		for (uint32_t phase = 0; phase < NUM_PHASES; phase++)
		{
			if (timeData.queryedStage[phase] == m_currentStage)
			{
				GLExt::glEndQuery(GL_TIME_ELAPSED);
				break;
			}
		}
	}

	GLCheckError();
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
int32_t GpuTimer::GetResult(const void* object)
{
	for (uint32_t index = 1; index < 5; index++)
	{
		if (m_stageState[index] == State::AfterStage)
		{
			UpdateResults(static_cast<Effekseer::GpuStage>(index));
		}
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
