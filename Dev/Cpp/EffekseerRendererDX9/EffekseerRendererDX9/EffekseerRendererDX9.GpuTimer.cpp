#include "EffekseerRendererDX9.GpuTimer.h"

namespace EffekseerRendererDX9
{

GpuTimer::GpuTimer(RendererImplemented* renderer)
	: renderer_(renderer)
{
	renderer_->AddRef();
	auto graphicsDevice = renderer_->GetGraphicsDevice().DownCast<Backend::GraphicsDevice>();
	if (graphicsDevice)
	{
		graphicsDevice->Register(this);
	}

	InitDevice();

	if (isValid_)
	{
		renderer_->GetStandardRenderer()->UpdateGpuTimerCount(+1);
	}
}

GpuTimer::~GpuTimer()
{
	if (isValid_)
	{
		renderer_->GetStandardRenderer()->UpdateGpuTimerCount(-1);
	}

	ReleaseDevice();

	auto graphicsDevice = renderer_->GetGraphicsDevice().DownCast<Backend::GraphicsDevice>();
	if (graphicsDevice)
	{
		graphicsDevice->Unregister(this);
	}

	renderer_->Release();
}

bool GpuTimer::CreateTimestampQuery(Backend::Direct3DQuery9Ptr& query)
{
	auto device = renderer_->GetDevice();
	if (device == nullptr)
	{
		return false;
	}

	IDirect3DQuery9* q = nullptr;
	if (FAILED(device->CreateQuery(D3DQUERYTYPE_TIMESTAMP, &q)))
	{
		return false;
	}

	query = Effekseer::CreateUniqueReference(q);
	return true;
}

bool GpuTimer::CreateStageQueries(uint32_t index)
{
	auto device = renderer_->GetDevice();
	if (device == nullptr)
	{
		return false;
	}

	IDirect3DQuery9* disjoint = nullptr;
	if (FAILED(device->CreateQuery(D3DQUERYTYPE_TIMESTAMPDISJOINT, &disjoint)))
	{
		return false;
	}
	disjoint_[index] = Effekseer::CreateUniqueReference(disjoint);

	IDirect3DQuery9* frequency = nullptr;
	if (FAILED(device->CreateQuery(D3DQUERYTYPE_TIMESTAMPFREQ, &frequency)))
	{
		disjoint_[index].reset();
		return false;
	}
	frequency_[index] = Effekseer::CreateUniqueReference(frequency);

	return true;
}

void GpuTimer::InitDevice()
{
	isValid_ = true;

	for (uint32_t index = 1; index < 5; index++)
	{
		if (!CreateStageQueries(index))
		{
			isValid_ = false;
			ReleaseDevice();
			return;
		}
	}

	for (auto& kv : timeData_)
	{
		auto& timeData = kv.second;
		for (uint32_t phase = 0; phase < NUM_PHASES; phase++)
		{
			if (!CreateTimestampQuery(timeData.startQuery[phase]) || !CreateTimestampQuery(timeData.stopQuery[phase]))
			{
				isValid_ = false;
				ReleaseDevice();
				return;
			}
		}
	}
}

void GpuTimer::ReleaseDevice()
{
	for (auto& kv : timeData_)
	{
		auto& timeData = kv.second;
		for (uint32_t phase = 0; phase < NUM_PHASES; phase++)
		{
			timeData.startQuery[phase].reset();
			timeData.stopQuery[phase].reset();
			timeData.queryedStage[phase] = Effekseer::GpuStage::None;
		}
	}

	for (uint32_t index = 1; index < 5; index++)
	{
		disjoint_[index].reset();
		frequency_[index].reset();
		stageState_[index] = State::NoResult;
	}

	currentStage_ = Effekseer::GpuStage::None;
}

void GpuTimer::OnLostDevice()
{
	ReleaseDevice();
}

void GpuTimer::OnChangeDevice()
{
	ReleaseDevice();
	InitDevice();
}

void GpuTimer::OnResetDevice()
{
	InitDevice();
}

bool GpuTimer::WaitForData(IDirect3DQuery9* query, void* data, DWORD size)
{
	if (query == nullptr)
	{
		return false;
	}

	while (true)
	{
		auto hr = query->GetData(data, size, D3DGETDATA_FLUSH);
		if (hr == S_OK)
		{
			return true;
		}
		if (hr == S_FALSE)
		{
			Sleep(1);
			continue;
		}
		return false;
	}
}

void GpuTimer::UpdateResults()
{
	if (!isValid_)
	{
		return;
	}

	for (uint32_t index = 1; index < 5; index++)
	{
		if (stageState_[index] == State::AfterStage)
		{
			UpdateResults(static_cast<Effekseer::GpuStage>(index));
		}
	}
}

void GpuTimer::BeginStage(Effekseer::GpuStage stage)
{
	if (!isValid_)
	{
		return;
	}

	assert(stage != Effekseer::GpuStage::None);

	uint32_t index = static_cast<uint32_t>(stage);
	assert(stageState_[index] != State::DuringStage);

	if (stageState_[index] == State::AfterStage)
	{
		UpdateResults(stage);
	}

	disjoint_[index]->Issue(D3DISSUE_BEGIN);
	stageState_[index] = State::DuringStage;
	currentStage_ = stage;
}

void GpuTimer::EndStage(Effekseer::GpuStage stage)
{
	if (!isValid_)
	{
		return;
	}

	assert(stage != Effekseer::GpuStage::None);

	uint32_t index = static_cast<uint32_t>(stage);
	assert(stageState_[index] == State::DuringStage);

	frequency_[index]->Issue(D3DISSUE_END);
	disjoint_[index]->Issue(D3DISSUE_END);

	stageState_[index] = State::AfterStage;
	currentStage_ = Effekseer::GpuStage::None;
}

void GpuTimer::UpdateResults(Effekseer::GpuStage stage)
{
	if (!isValid_)
	{
		return;
	}

	assert(stage != Effekseer::GpuStage::None);
	uint32_t index = static_cast<uint32_t>(stage);

	BOOL isDisjoint = FALSE;
	uint64_t frequency = 0;
	if (!WaitForData(disjoint_[index].get(), &isDisjoint, sizeof(isDisjoint)) || !WaitForData(frequency_[index].get(), &frequency, sizeof(frequency)))
	{
		return;
	}

	for (auto& kv : timeData_)
	{
		auto& timeData = kv.second;
		timeData.result = 0;

		if (isDisjoint || frequency == 0)
		{
			for (uint32_t phase = 0; phase < NUM_PHASES; phase++)
			{
				if (timeData.queryedStage[phase] == stage)
				{
					timeData.queryedStage[phase] = Effekseer::GpuStage::None;
				}
			}
			continue;
		}

		uint64_t elapsedTime = 0;
		for (uint32_t phase = 0; phase < NUM_PHASES; phase++)
		{
			if (timeData.queryedStage[phase] == stage)
			{
				uint64_t startTime = 0;
				uint64_t stopTime = 0;
				if (!WaitForData(timeData.startQuery[phase].get(), &startTime, sizeof(startTime)) ||
					!WaitForData(timeData.stopQuery[phase].get(), &stopTime, sizeof(stopTime)))
				{
					continue;
				}
				elapsedTime += (stopTime - startTime) * 1000000 / frequency;
				timeData.queryedStage[phase] = Effekseer::GpuStage::None;
			}
		}
		timeData.result = static_cast<int32_t>(elapsedTime);
	}

	stageState_[index] = State::ResultUpdated;
}

void GpuTimer::AddTimer(const void* object)
{
	if (!isValid_)
	{
		return;
	}

	assert(timeData_.find(object) == timeData_.end());

	TimeData timeData;
	for (uint32_t phase = 0; phase < NUM_PHASES; phase++)
	{
		if (!CreateTimestampQuery(timeData.startQuery[phase]) || !CreateTimestampQuery(timeData.stopQuery[phase]))
		{
			return;
		}
	}

	timeData_.emplace(object, std::move(timeData));
}

void GpuTimer::RemoveTimer(const void* object)
{
	timeData_.erase(object);
}

void GpuTimer::Start(const void* object)
{
	if (!isValid_)
	{
		return;
	}

	assert(currentStage_ != Effekseer::GpuStage::None);

	auto it = timeData_.find(object);
	if (it != timeData_.end())
	{
		auto& timeData = it->second;
		for (uint32_t phase = 0; phase < NUM_PHASES; phase++)
		{
			if (timeData.queryedStage[phase] == Effekseer::GpuStage::None)
			{
				timeData.startQuery[phase]->Issue(D3DISSUE_END);
				timeData.queryedStage[phase] = currentStage_;
				break;
			}
		}
	}
}

void GpuTimer::Stop(const void* object)
{
	if (!isValid_)
	{
		return;
	}

	assert(currentStage_ != Effekseer::GpuStage::None);

	auto it = timeData_.find(object);
	if (it != timeData_.end())
	{
		auto& timeData = it->second;
		for (uint32_t phase = 0; phase < NUM_PHASES; phase++)
		{
			if (timeData.queryedStage[phase] == currentStage_)
			{
				timeData.stopQuery[phase]->Issue(D3DISSUE_END);
				break;
			}
		}
	}
}

int32_t GpuTimer::GetResult(const void* object)
{
	UpdateResults();

	auto it = timeData_.find(object);
	if (it != timeData_.end())
	{
		return it->second.result;
	}
	return 0;
}

} // namespace EffekseerRendererDX9
