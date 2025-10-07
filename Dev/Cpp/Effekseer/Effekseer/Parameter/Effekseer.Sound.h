#pragma once

#include "../Effekseer.InternalStruct.h"

namespace Effekseer
{

enum ParameterSoundType
{
	ParameterSoundType_None = 0,
	ParameterSoundType_Use = 1,

	ParameterSoundType_DWORD = 0x7fffffff,
};

enum ParameterSoundPanType
{
	ParameterSoundPanType_2D = 0,
	ParameterSoundPanType_3D = 1,

	ParameterSoundPanType_DWORD = 0x7fffffff,
};

struct ParameterSound
{
	ParameterSoundType SoundType = ParameterSoundType_None;
	int32_t WaveId;
	random_float Volume;
	random_float Pitch;
	ParameterSoundPanType PanType;
	random_float Pan;
	float Distance;
	random_int Delay;

	void Load(unsigned char*& pos, int version)
	{
		if (version >= 1)
		{
			memcpy(&SoundType, pos, sizeof(int));
			pos += sizeof(int);
		}

		if (SoundType == ParameterSoundType_Use)
		{
			memcpy(&WaveId, pos, sizeof(int32_t));
			pos += sizeof(int32_t);
			memcpy(&Volume, pos, sizeof(random_float));
			pos += sizeof(random_float);
			memcpy(&Pitch, pos, sizeof(random_float));
			pos += sizeof(random_float);
			memcpy(&PanType, pos, sizeof(ParameterSoundPanType));
			pos += sizeof(ParameterSoundPanType);
			memcpy(&Pan, pos, sizeof(random_float));
			pos += sizeof(random_float);
			memcpy(&Distance, pos, sizeof(float));
			pos += sizeof(float);
			memcpy(&Delay, pos, sizeof(random_int));
			pos += sizeof(random_int);
		}
	}
};

} // namespace Effekseer