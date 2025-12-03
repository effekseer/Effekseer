
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
	, renderer_(renderer)
{
	renderer_->GetStandardRenderer()->UpdateGpuTimerCount(+1);
	renderer_->AddRef();

	InitDevice();
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
GpuTimer::~GpuTimer()
{
	ReleaseDevice();

	renderer_->GetStandardRenderer()->UpdateGpuTimerCount(-1);
	renderer_->Release();
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void GpuTimer::InitDevice()
{
	for (auto& kv : timeData_)
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
	for (auto& kv : timeData_)
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
	assert(stageState_[index] != State::DuringStage);

	if (stageState_[index] == State::AfterStage)
	{
		UpdateResults(stage);
	}

	stageState_[index] = State::DuringStage;
	currentStage_ = stage;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void GpuTimer::EndStage(Effekseer::GpuStage stage)
{
	assert(stage != Effekseer::GpuStage::None);

	uint32_t index = static_cast<uint32_t>(stage);
	assert(stageState_[index] == State::DuringStage);

	stageState_[index] = State::AfterStage;
	currentStage_ = Effekseer::GpuStage::None;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void GpuTimer::UpdateResults(Effekseer::GpuStage stage)
{
	assert(stage != Effekseer::GpuStage::None);
	uint32_t index = static_cast<uint32_t>(stage);

	for (auto& kv : timeData_)
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

	stageState_[index] = State::ResultUpdated;

	GLCheckError();
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void GpuTimer::AddTimer(const void* object)
{
	assert(timeData_.find(object) == timeData_.end());

	TimeData timeData;
	GLExt::glGenQueries(NUM_PHASES, timeData.timeElapsedQuery);

	timeData_.emplace(object, std::move(timeData));

	GLCheckError();
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void GpuTimer::RemoveTimer(const void* object)
{
	auto it = timeData_.find(object);
	if (it != timeData_.end())
	{
		TimeData& timeData = it->second;
		GLExt::glDeleteQueries(NUM_PHASES, timeData.timeElapsedQuery);
		timeData_.erase(it);
	}

	GLCheckError();
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
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
				GLExt::glBeginQuery(GL_TIME_ELAPSED, timeData.timeElapsedQuery[phase]);
				timeData.queryedStage[phase] = currentStage_;
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
	assert(currentStage_ != Effekseer::GpuStage::None);

	auto it = timeData_.find(object);
	if (it != timeData_.end())
	{
		TimeData& timeData = it->second;
		for (uint32_t phase = 0; phase < NUM_PHASES; phase++)
		{
			if (timeData.queryedStage[phase] == currentStage_)
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
		if (stageState_[index] == State::AfterStage)
		{
			UpdateResults(static_cast<Effekseer::GpuStage>(index));
		}
	}

	auto it = timeData_.find(object);
	if (it != timeData_.end())
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
