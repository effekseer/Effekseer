
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
#include "EffekseerRendererDX11.GpuTimer.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererDX11
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
GpuTimer::GpuTimer(RendererImplemented* renderer, bool has_ref_count)
	: DeviceObject(renderer, has_ref_count)
{
	if (auto renderer = GetRenderer())
	{
		renderer->GetStandardRenderer()->UpdateGpuTimerCount(+1);
	}

	InitDevice();
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
GpuTimer::~GpuTimer()
{
	ReleaseDevice();

	if (auto renderer = GetRenderer())
	{
		renderer->GetStandardRenderer()->UpdateGpuTimerCount(-1);
	}
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void GpuTimer::InitDevice()
{
	auto device = GetRenderer()->GetDevice();

	{
		D3D11_QUERY_DESC desc = {};
		desc.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;

		for (uint32_t index = 1; index < 5; index++)
		{
			ID3D11Query* disjoint = nullptr;
			device->CreateQuery(&desc, &disjoint);
			disjoint_[index].reset(disjoint);
		}
	}

	for (auto& kv : time_data_)
	{
		TimeData& timeData = kv.second;

		D3D11_QUERY_DESC desc = {};
		desc.Query = D3D11_QUERY_TIMESTAMP;

		for (uint32_t phase = 0; phase < NUM_PHASES; phase++)
		{
			ID3D11Query* startQuery = nullptr;
			device->CreateQuery(&desc, &startQuery);
			timeData.startQuery[phase].reset(startQuery);

			ID3D11Query* stopQuery = nullptr;
			device->CreateQuery(&desc, &stopQuery);
			timeData.stopQuery[phase].reset(stopQuery);
		}
	}
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void GpuTimer::ReleaseDevice()
{
	for (auto& kv : time_data_)
	{
		TimeData& timeData = kv.second;
		for (uint32_t phase = 0; phase < NUM_PHASES; phase++)
		{
			timeData.startQuery[phase].reset();
			timeData.stopQuery[phase].reset();
		}
	}

	for (uint32_t index = 1; index < 5; index++)
	{
		disjoint_[index].reset();
	}
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
	assert(stage_states_[index] != State::DuringStage);

	if (stage_states_[index] == State::AfterStage)
	{
		// Avoid D3D11 warning
		UpdateResults(stage);
	}

	auto context = GetRenderer()->GetContext();
	context->Begin(disjoint_[index].get());

	stage_states_[index] = State::DuringStage;
	current_stage_ = stage;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void GpuTimer::EndStage(Effekseer::GpuStage stage)
{
	assert(stage != Effekseer::GpuStage::None);

	uint32_t index = static_cast<uint32_t>(stage);
	assert(stage_states_[index] == State::DuringStage);

	auto context = GetRenderer()->GetContext();
	context->End(disjoint_[index].get());

	stage_states_[index] = State::AfterStage;
	current_stage_ = Effekseer::GpuStage::None;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void GpuTimer::UpdateResults(Effekseer::GpuStage stage)
{
	HRESULT hr = S_OK;
	auto context = GetRenderer()->GetContext();

	assert(stage != Effekseer::GpuStage::None);
	uint32_t index = static_cast<uint32_t>(stage);

	D3D11_QUERY_DATA_TIMESTAMP_DISJOINT disjoint = {1, TRUE};

	while (true)
	{
		hr = context->GetData(disjoint_[index].get(), &disjoint, sizeof(disjoint), 0);
		if (hr == S_OK)
		{
			break;
		}
		else if (hr == S_FALSE)
		{
			Sleep(1);
		}
		else
		{
			return;
		}
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

	for (auto& kv : time_data_)
	{
		auto& timeData = kv.second;
		timeData.result = 0;

		uint64_t elapsedTime = 0;
		for (uint32_t phase = 0; phase < NUM_PHASES; phase++)
		{
			if (timeData.queryedStage[phase] == stage)
			{
				uint64_t startTime, stopTime;
				if (!getTimeStamp(timeData.startQuery[phase].get(), startTime))
					continue;
				if (!getTimeStamp(timeData.stopQuery[phase].get(), stopTime))
					continue;
				elapsedTime += (stopTime - startTime) * 1000000 / disjoint.Frequency;
				timeData.queryedStage[phase] = Effekseer::GpuStage::None;
			}
		}
		timeData.result = static_cast<int32_t>(elapsedTime);
	}

	stage_states_[index] = State::ResultUpdated;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void GpuTimer::AddTimer(const void* object)
{
	assert(time_data_.find(object) == time_data_.end());

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

	time_data_.emplace(object, std::move(timeData));
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void GpuTimer::RemoveTimer(const void* object)
{
	time_data_.erase(object);
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void GpuTimer::Start(const void* object)
{
	assert(current_stage_ != Effekseer::GpuStage::None);

	auto context = GetRenderer()->GetContext();

	auto it = time_data_.find(object);
	if (it != time_data_.end())
	{
		TimeData& timeData = it->second;
		for (uint32_t phase = 0; phase < NUM_PHASES; phase++)
		{
			if (timeData.queryedStage[phase] == Effekseer::GpuStage::None)
			{
				context->End(timeData.startQuery[phase].get());
				timeData.queryedStage[phase] = current_stage_;
				break;
			}
		}
	}
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void GpuTimer::Stop(const void* object)
{
	assert(current_stage_ != Effekseer::GpuStage::None);

	auto context = GetRenderer()->GetContext();

	auto it = time_data_.find(object);
	if (it != time_data_.end())
	{
		TimeData& timeData = it->second;
		for (uint32_t phase = 0; phase < NUM_PHASES; phase++)
		{
			if (timeData.queryedStage[phase] == current_stage_)
			{
				context->End(timeData.stopQuery[phase].get());
				break;
			}
		}
	}
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
int32_t GpuTimer::GetResult(const void* object)
{
	for (uint32_t index = 1; index < 5; index++)
	{
		if (stage_states_[index] == State::AfterStage)
		{
			UpdateResults(static_cast<Effekseer::GpuStage>(index));
		}
	}

	auto it = time_data_.find(object);
	if (it != time_data_.end())
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
