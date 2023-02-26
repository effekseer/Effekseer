#include "EffekseerRendererLLGI.GpuTimer.h"

namespace EffekseerRendererLLGI
{

GpuTimer::GpuTimer(RendererImplemented* renderer, bool hasRefCount)
	: m_renderer(renderer)
{
	m_renderer->AddRef();
	m_renderer->GetStandardRenderer()->UpdateGpuTimerCount(+1);
}

GpuTimer::~GpuTimer()
{
	m_renderer->GetStandardRenderer()->UpdateGpuTimerCount(-1);
	m_renderer->Release();
}

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

void GpuTimer::EndStage(Effekseer::GpuStage stage)
{
	assert(stage != Effekseer::GpuStage::None);

	uint32_t index = static_cast<uint32_t>(stage);
	assert(m_stageState[index] == State::DuringStage);

	m_stageState[index] = State::AfterStage;
	m_currentStage = Effekseer::GpuStage::None;
}

void GpuTimer::UpdateResults()
{
	for (uint32_t index = 1; index < 5; index++)
	{
		if (m_stageState[index] == State::AfterStage)
		{
			UpdateResults(static_cast<Effekseer::GpuStage>(index));
		}
	}

	for (auto& [object, timeData] : m_timeData)
	{
		m_renderer->ResetQuery(timeData.queries.get());
	}
}

void GpuTimer::UpdateResults(Effekseer::GpuStage stage)
{
	auto graphicsDevice = m_renderer->GetGraphicsDevice().DownCast<EffekseerRendererLLGI::Backend::GraphicsDevice>();

	assert(stage != Effekseer::GpuStage::None);
	uint32_t index = static_cast<uint32_t>(stage);

	for (auto& [object, timeData] : m_timeData)
	{
		timeData.result = 0;

		uint64_t elapsedTime = 0;
		for (uint32_t phase = 0; phase < NUM_PHASES; phase++)
		{
			if (timeData.queryedStage[phase] == stage)
			{
				if (timeData.queries)
				{
					uint64_t startTime = timeData.queries->GetQueryResult(QueryIndex(phase, TIMESTAMP_START));
					uint64_t stopTime = timeData.queries->GetQueryResult(QueryIndex(phase, TIMESTAMP_STOP));
					elapsedTime += graphicsDevice->GetGraphics()->TimestampToMicroseconds(stopTime - startTime);
				}
				timeData.queryedStage[phase] = Effekseer::GpuStage::None;
			}
		}
		timeData.result = static_cast<int32_t>(elapsedTime);
	}

	m_stageState[index] = State::ResultUpdated;
}

void GpuTimer::AddTimer(const void* object)
{
	assert(m_timeData.find(object) == m_timeData.end());

	auto graphicsDevice = m_renderer->GetGraphicsDevice().DownCast<EffekseerRendererLLGI::Backend::GraphicsDevice>();

	TimeData timeData;
	timeData.queries = LLGI::CreateUniqueReference(graphicsDevice->GetGraphics()->CreateQuery(
		LLGI::QueryType::Timestamp, NUM_QUERIES_PER_TIMER));

	m_timeData.emplace(object, std::move(timeData));
}

void GpuTimer::RemoveTimer(const void* object)
{
	auto it = m_timeData.find(object);
	if (it != m_timeData.end())
	{
		TimeData& timeData = it->second;
		m_timeData.erase(it);
	}
}

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
				m_renderer->RecordTimestamp(timeData.queries.get(), QueryIndex(phase, TIMESTAMP_START));
				timeData.queryedStage[phase] = m_currentStage;
				break;
			}
		}
	}
}

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
				m_renderer->RecordTimestamp(timeData.queries.get(), QueryIndex(phase, TIMESTAMP_STOP));
				break;
			}
		}
	}
}

int32_t GpuTimer::GetResult(const void* object)
{
	auto it = m_timeData.find(object);
	if (it != m_timeData.end())
	{
		TimeData& timeData = it->second;
		return timeData.result;
	}
	return 0;
}

} // namespace EffekseerRendererLLGI
