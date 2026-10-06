// InstanceUpdateDispatcher.cpp
// Check external scheduling and the serial fallback with the public runtime API.

#include "../TestHelper.h"
#include "Effekseer.h"
#include <array>
#include <thread>

namespace
{

void TestExternalInstanceUpdateDispatcher()
{
	constexpr size_t EffectCount = 128;
	auto serial = Effekseer::Manager::Create(4096);
	auto external = Effekseer::Manager::Create(4096);
	const auto path = GetDirectoryPathAsU16(__FILE__) + u"../Resource/Simple_Sprite_FixedYAxis.efk";
	const auto data = LoadFile(path.c_str());
	EXPECT_TRUE(!data.empty());
	auto serialEffect = Effekseer::Effect::Create(serial, data.data(), static_cast<int32_t>(data.size()));
	auto externalEffect = Effekseer::Effect::Create(external, data.data(), static_cast<int32_t>(data.size()));
	EXPECT_TRUE(serialEffect != nullptr && externalEffect != nullptr);

	size_t dispatchCount = 0;
	external->SetInstanceUpdateDispatcher([&](size_t count, Effekseer::InstanceUpdateTask task, void* context)
										  {
			EXPECT_TRUE(count >= 4);
			dispatchCount++;
			const size_t middle = count / 2;
			std::thread worker([=]() { task(context, middle, count); });
			task(context, 0, middle);
			worker.join(); });

	std::array<Effekseer::Handle, EffectCount> serialHandles;
	std::array<Effekseer::Handle, EffectCount> externalHandles;
	for (size_t i = 0; i < EffectCount; i++)
	{
		serialHandles[i] = serial->Play(serialEffect, 0.0f, 0.0f, 0.0f);
		externalHandles[i] = external->Play(externalEffect, 0.0f, 0.0f, 0.0f);
		EXPECT_TRUE(serialHandles[i] >= 0 && externalHandles[i] >= 0);
		serial->SetRandomSeed(serialHandles[i], static_cast<int32_t>(i + 1));
		external->SetRandomSeed(externalHandles[i], static_cast<int32_t>(i + 1));
	}

	const auto compare = [&]()
	{
		EXPECT_TRUE(serial->GetTotalInstanceCount() == external->GetTotalInstanceCount());
		for (size_t i = 0; i < EffectCount; i++)
		{
			EXPECT_TRUE(serial->Exists(serialHandles[i]) == external->Exists(externalHandles[i]));
			EXPECT_TRUE(serial->GetInstanceCount(serialHandles[i]) == external->GetInstanceCount(externalHandles[i]));
		}
	};
	for (size_t frame = 0; frame < 90; frame++)
	{
		Effekseer::Manager::UpdateParameter parameter;
		parameter.DeltaFrame = frame % 3 == 0 ? 3.0f : 1.0f;
		serial->Update(parameter);
		external->Update(parameter);
		compare();
	}
	EXPECT_TRUE(dispatchCount > 0);
	EXPECT_TRUE(external->GetWorkerThreadHandle(0) == 0);

	external->SetInstanceUpdateDispatcher({});
	const size_t previousDispatchCount = dispatchCount;
	serial->Update(1.0f);
	external->Update(1.0f);
	EXPECT_TRUE(dispatchCount == previousDispatchCount);
	compare();
}

TestRegister InstanceUpdateDispatcher_External("InstanceUpdateDispatcher.External", []() -> void
											   { TestExternalInstanceUpdateDispatcher(); });

} // namespace

// EOF
