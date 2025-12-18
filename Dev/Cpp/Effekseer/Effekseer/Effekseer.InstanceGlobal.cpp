

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include "Effekseer.InstanceGlobal.h"
#include "Model/Effekseer.Model.h"
#include "Utils/Effekseer.CustomAllocator.h"
#include <assert.h>

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

void* InstanceGlobal::operator new(size_t size)
{
	assert(sizeof(InstanceGlobal) == size);
	return GetAlignedMallocFunc()(static_cast<uint32_t>(size), 16);
}

void InstanceGlobal::operator delete(void* p)
{
	GetAlignedFreeFunc()(p, sizeof(InstanceGlobal));
}

InstanceGlobal::InstanceGlobal()
	: instanceCount_(0)
	, updatedFrame_(0)
	, rootContainer_(nullptr)
{
	dynamicInputParameters_.fill(0);
	inputTriggerCounts_.fill(0);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
InstanceGlobal::~InstanceGlobal()
{
}

float InstanceGlobal::GetNextDeltaFrame() const
{
	return nextDeltaFrame_;
}

void InstanceGlobal::BeginDeltaFrame(float frame)
{
	nextDeltaFrame_ = frame;
}

void InstanceGlobal::EndDeltaFrame()
{
	updatedFrame_ += nextDeltaFrame_;
	nextDeltaFrame_ = 0.0f;

	// Reset the all trigger's count
	inputTriggerCounts_.fill(0);
}

std::array<float, 4> InstanceGlobal::GetDynamicEquationResult(int32_t index)
{
	assert(0 <= index && index < dynamicEqResults.size());
	return dynamicEqResults[index];
}

uint32_t InstanceGlobal::GetInputTriggerCount(uint32_t index) const
{
	if (index < inputTriggerCounts_.size())
	{
		return inputTriggerCounts_[index];
	}
	return 0;
}

void InstanceGlobal::AddInputTriggerCount(uint32_t index)
{
	if (index < inputTriggerCounts_.size())
	{
		inputTriggerCounts_[index] = (uint8_t)Min(
			(uint32_t)inputTriggerCounts_[index] + 1,
			(uint32_t)std::numeric_limits<uint8_t>::max());
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void InstanceGlobal::IncInstanceCount()
{
	instanceCount_++;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void InstanceGlobal::DecInstanceCount()
{
	instanceCount_--;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
int InstanceGlobal::GetInstanceCount()
{
	return instanceCount_;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
float InstanceGlobal::GetUpdatedFrame() const
{
	return updatedFrame_;
}

void InstanceGlobal::ResetUpdatedFrame()
{
	updatedFrame_ = 0.0f;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
InstanceContainer* InstanceGlobal::GetRootContainer() const
{
	return rootContainer_;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void InstanceGlobal::SetRootContainer(InstanceContainer* container)
{
	rootContainer_ = container;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
const SIMD::Vec3f& InstanceGlobal::GetTargetLocation() const
{
	return targetLocation_;
}

void InstanceGlobal::SetTargetLocation(const Vector3D& location)
{
	targetLocation_ = location;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace Effekseer

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------