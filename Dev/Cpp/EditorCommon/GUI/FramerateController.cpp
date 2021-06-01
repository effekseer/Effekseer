#include "FramerateController.h"

#ifdef _WIN32
#pragma comment(lib, "Winmm.lib")
#include <windows.h>
#endif

namespace Effekseer
{

FramerateController::FramerateController()
{
	previousTime_ = std::chrono::system_clock::now();
	SetTargetFramerate(60);

#ifdef _WIN32
	timeBeginPeriod(1);
#endif
}

FramerateController::~FramerateController()
{
#ifdef _WIN32
	timeEndPeriod(1);
#endif
}

void FramerateController::Update()
{
	auto currentTime = std::chrono::system_clock::now();
	auto deltans = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime - previousTime_);

	// sleep
	if (deltans < framens_)
	{
		auto sleepns = ((previousTime_ + framens_) - currentTime);

		// sleep by milliseconds
		std::this_thread::sleep_for(std::chrono::milliseconds((sleepns.count() / 1000000) - 1));

		// adjust with busy loop
		do
		{
			currentTime = std::chrono::system_clock::now();
			deltans = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime - previousTime_);
		} while (deltans < framens_);
	}

	// measure time
	deltaSecond_ = static_cast<float>(deltans.count()) / nano;
	currentFPS_ = 1.0f / deltaSecond_;
	previousTime_ = currentTime;
}

float FramerateController::GetDeltaSecond() const
{
	return deltaSecond_;
}
float FramerateController::GetCurrentFramerate() const
{
	return currentFPS_;
}

int32_t FramerateController::GetTargetFramerate() const
{
	return targetFPS_;
}
void FramerateController::SetTargetFramerate(int32_t fps)
{
	targetFPS_ = fps;
	framens_ = std::chrono::nanoseconds(nano / fps);
}

} // namespace Effekseer