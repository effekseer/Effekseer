
#pragma once

#include <stdint.h>
#include <string>

namespace Effekseer
{
namespace Tool
{
enum class RecordingModeType
{
	Sprite,
	SpriteSheet,
	Gif,
	Avi,
	H264,
};

enum class TransparenceType
{
	None = 0,
	Original = 1,
	Generate = 2,
	Generate2 = 3,
};

class RecordingParameter
{
	std::u16string path;
	std::u16string ext;

public:
	const char16_t* GetPath() const
	{
		return path.c_str();
	}
	const char16_t* GetExt() const
	{
		return ext.c_str();
	}
	void SetPath(const char16_t* value)
	{
		path = value;
	}
	void SetExt(const char16_t* value)
	{
		ext = value;
	}

	RecordingModeType RecordingMode;
	int32_t Count = 0;
	int32_t HorizontalCount = 0;
	int32_t OffsetFrame;
	int32_t Freq;
	TransparenceType Transparence;
	int32_t Scale = 1;
};

} // namespace Tool
} // namespace Effekseer
