﻿

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include "Effekseer.InstanceGlobal.h"
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
	: m_instanceCount(0)
	, m_updatedFrame(0)
	, m_rootContainer(nullptr)
{
	dynamicInputParameters.fill(0);
	m_inputTriggerCounts.fill(0);
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
	m_updatedFrame += nextDeltaFrame_;
	nextDeltaFrame_ = 0.0f;

	// Reset the all trigger's count
	m_inputTriggerCounts.fill(0);
}

std::array<float, 4> InstanceGlobal::GetDynamicEquationResult(int32_t index)
{
	assert(0 <= index && index < dynamicEqResults.size());
	return dynamicEqResults[index];
}

uint32_t InstanceGlobal::GetInputTriggerCount(uint32_t index) const
{
	if (index < m_inputTriggerCounts.size())
	{
		return m_inputTriggerCounts[index];
	}
	return 0;
}

void InstanceGlobal::AddInputTriggerCount(uint32_t index)
{
	if (index < m_inputTriggerCounts.size())
	{
		m_inputTriggerCounts[index] = (uint8_t)Min(
			(uint32_t)m_inputTriggerCounts[index] + 1, 
			(uint32_t)std::numeric_limits<uint8_t>::max());
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void InstanceGlobal::IncInstanceCount()
{
	m_instanceCount++;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void InstanceGlobal::DecInstanceCount()
{
	m_instanceCount--;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
int InstanceGlobal::GetInstanceCount()
{
	return m_instanceCount;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
float InstanceGlobal::GetUpdatedFrame()
{
	return m_updatedFrame;
}

void InstanceGlobal::ResetUpdatedFrame()
{
	m_updatedFrame = 0.0f;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
InstanceContainer* InstanceGlobal::GetRootContainer() const
{
	return m_rootContainer;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void InstanceGlobal::SetRootContainer(InstanceContainer* container)
{
	m_rootContainer = container;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
const SIMD::Vec3f& InstanceGlobal::GetTargetLocation() const
{
	return m_targetLocation;
}

void InstanceGlobal::SetTargetLocation(const Vector3D& location)
{
	m_targetLocation = location;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace Effekseer

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------