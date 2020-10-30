

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
	return GetMallocFunc()(static_cast<uint32_t>(size));
}

void InstanceGlobal::operator delete(void* p)
{
	GetFreeFunc()(p, sizeof(InstanceGlobal));
}

InstanceGlobal::InstanceGlobal()
	: m_instanceCount(0)
	, m_updatedFrame(0)
	, m_rootContainer(NULL)
{
	dynamicInputParameters.fill(0);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
InstanceGlobal::~InstanceGlobal()
{
}

void InstanceGlobal::BeginDeltaFrame(float frame)
{
	NextDeltaFrame = frame;
}

void InstanceGlobal::EndDeltaFrame()
{
	m_updatedFrame += NextDeltaFrame;
	NextDeltaFrame = 0.0f;
}

std::array<float, 4> InstanceGlobal::GetDynamicEquationResult(int32_t index)
{
	assert(0 <= index && index < dynamicEqResults.size());
	return dynamicEqResults[index];
}

void InstanceGlobal::SetSeed(int64_t seed)
{
	m_seed = seed;
}

float InstanceGlobal::GetRand()
{
	const int a = 1103515245;
	const int c = 12345;
	const int m = 2147483647;

	m_seed = (m_seed * a + c) & m;
	auto ret = m_seed % 0x7fff;

	return (float)ret / (float)(0x7fff - 1);
}

float InstanceGlobal::GetRand(float min_, float max_)
{
	return GetRand() * (max_ - min_) + min_;
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
const Vec3f& InstanceGlobal::GetTargetLocation() const
{
	return m_targetLocation;
}

void InstanceGlobal::SetTargetLocation(const Vector3D& location)
{
	m_targetLocation = location;
}

float InstanceGlobal::Rand(void* userData)
{
	auto g = reinterpret_cast<InstanceGlobal*>(userData);
	return g->GetRand();
}

float InstanceGlobal::RandSeed(void* userData, float randSeed)
{
	auto seed = static_cast<int64_t>(randSeed * 1024 * 8);
	const int a = 1103515245;
	const int c = 12345;
	const int m = 2147483647;

	seed = (seed * a + c) & m;
	auto ret = seed % 0x7fff;

	return (float)ret / (float)(0x7fff - 1);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace Effekseer

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------