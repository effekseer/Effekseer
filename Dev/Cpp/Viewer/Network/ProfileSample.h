#pragma once

#include <Effekseer.h>

namespace Effekseer::Tool
{

struct ProfileSample
{
	bool IsValid = false;

	struct Manager
	{
		uint32_t HandleCount = 0;
		float CPUTime = 0.0f;
		float GPUTime = 0.0f;
	};
	std::vector<Manager> Managers;

	struct Effect
	{
		std::u16string Key;
		uint32_t HandleCount = 0;
		float GPUTime = 0.0f;
	};
	std::vector<Effect> Effects;
};

} // namespace Effekseer::Tool
