
#pragma once

#include <stdint.h>
#include <Platform/PlatformMisc.h>

namespace EffekseerMaterial
{

class Config
{

private:

public:
	Config();
	~Config();
	bool Save(const char* path);
	bool Load(const char* path);

	int32_t WindowWidth = 0;
	int32_t WindowHeight = 0;
	int32_t WindowPosX = -10000;
	int32_t WindowPosY = -10000;
	bool WindowIsMaximumMode = false;
	Effekseer::SystemLanguage Language = Effekseer::SystemLanguage::Unknown;
};

} // namespace EffekseerMaterial