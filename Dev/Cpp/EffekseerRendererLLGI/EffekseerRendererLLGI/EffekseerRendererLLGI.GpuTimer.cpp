#include "EffekseerRendererLLGI.GpuTimer.h"

namespace EffekseerRendererLLGI
{

GpuTimer::GpuTimer(RendererImplemented* renderer, bool hasRefCount)
	: renderer_(renderer)
{
	renderer_->AddRef();
	renderer_->GetStandardRenderer()->UpdateGpuTimerCount(+1);
}

GpuTimer::~GpuTimer()
{
	renderer_->GetStandardRenderer()->UpdateGpuTimerCount(-1);
	renderer_->Release();
}

void GpuTimer::BeginStage(Effekseer::GpuStage stage)
{
	assert(stage != Effekseer::GpuStage::None);

	uint32_t index = static_cast<uint32_t>(stage);
	assert(stageState_[index] != State::DuringStage);

	if (stageState_[index] == State::AfterStage)
	{
		UpdateResults(stage);
	}

	stageState_[index] = State::DuringStage;
	currentStage_ = stage;
}

void GpuTimer::EndStage(Effekseer::GpuStage stage)
{
	assert(stage != Effekseer::GpuStage::None);

	uint32_t index = static_cast<uint32_t>(stage);
	assert(stageState_[index] == State::DuringStage);

	stageState_[index] = State::AfterStage;
	currentStage_ = Effekseer::GpuStage::None;
}

void GpuTimer::UpdateResults()
{
	for (uint32_t index = 1; index < 5; index++)
	{
		if (stageState_[index] == State::AfterStage)
		{
			UpdateResults(static_cast<Effekseer::GpuStage>(index));
		}
	}

	for (auto& [object, timeData] : timeData_)
	{
		renderer_->ResetQuery(timeData.queries.get());
	}
}

void GpuTimer::UpdateResults(Effekseer::GpuStage stage)
{
	auto graphicsDevice = renderer_->GetGraphicsDevice().DownCast<EffekseerRendererLLGI::Backend::GraphicsDevice>();

	assert(stage != Effekseer::GpuStage::None);
	uint32_t index = static_cast<uint32_t>(stage);

	for (auto& [object, timeData] : timeData_)
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

	stageState_[index] = State::ResultUpdated;
}

void GpuTimer::AddTimer(const void* object)
{
	assert(timeData_.find(object) == timeData_.end());

	auto graphicsDevice = renderer_->GetGraphicsDevice().DownCast<EffekseerRendererLLGI::Backend::GraphicsDevice>();

	TimeData timeData;
	timeData.queries = LLGI::CreateUniqueReference(graphicsDevice->GetGraphics()->CreateQuery(
		LLGI::QueryType::Timestamp, NUM_QUERIES_PER_TIMER));

	timeData_.emplace(object, std::move(timeData));
}

void GpuTimer::RemoveTimer(const void* object)
{
	auto it = timeData_.find(object);
	if (it != timeData_.end())
	{
		TimeData& timeData = it->second;
		timeData_.erase(it);
	}
}

void GpuTimer::Start(const void* object)
{
	assert(currentStage_ != Effekseer::GpuStage::None);

	auto it = timeData_.find(object);
	if (it != timeData_.end())
	{
		TimeData& timeData = it->second;
		for (uint32_t phase = 0; phase < NUM_PHASES; phase++)
		{
			if (timeData.queryedStage[phase] == Effekseer::GpuStage::None)
			{
				renderer_->RecordTimestamp(timeData.queries.get(), QueryIndex(phase, TIMESTAMP_START));
				timeData.queryedStage[phase] = currentStage_;
				break;
			}
		}
	}
}

void GpuTimer::Stop(const void* object)
{
	assert(currentStage_ != Effekseer::GpuStage::None);

	auto it = timeData_.find(object);
	if (it != timeData_.end())
	{
		TimeData& timeData = it->second;
		for (uint32_t phase = 0; phase < NUM_PHASES; phase++)
		{
			if (timeData.queryedStage[phase] == currentStage_)
			{
				renderer_->RecordTimestamp(timeData.queries.get(), QueryIndex(phase, TIMESTAMP_STOP));
				break;
			}
		}
	}
}

int32_t GpuTimer::GetResult(const void* object)
{
	auto it = timeData_.find(object);
	if (it != timeData_.end())
	{
		TimeData& timeData = it->second;
		return timeData.result;
	}
	return 0;
}

} // namespace EffekseerRendererLLGI
