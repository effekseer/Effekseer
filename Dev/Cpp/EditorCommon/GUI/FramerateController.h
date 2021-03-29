#pragma once

#include <chrono>
#include <thread>

namespace Effekseer
{

class FramerateController
{
	float currentFPS_ = 0.0f;
	int32_t targetFPS_ = 60;
	float deltaSecond_ = 0.0f;

	std::chrono::system_clock::time_point previousTime_;
	std::chrono::nanoseconds framens_;

	static const int64_t nano = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds(1)).count();

public:
	FramerateController();
	~FramerateController();

	void Update();

	float GetDeltaSecond() const;
	float GetCurrentFramerate() const;

	int32_t GetTargetFramerate() const;
	void SetTargetFramerate(int32_t fps);
};

} // namespace Effekseer