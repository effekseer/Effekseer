#pragma once

#include "../Utils/Effekseer.BinaryReader.h"
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

	void Load(BinaryReader<true>& pos, int version)
	{
		if (version >= 1)
		{
			if (!pos.Peek(&SoundType, sizeof(int)))
			{
				return pos.MarkFailed();
			}
			pos.Skip(sizeof(int));
		}

		if (SoundType == ParameterSoundType_Use)
		{
			if (!pos.Peek(&WaveId, sizeof(int32_t)))
			{
				return pos.MarkFailed();
			}
			pos.Skip(sizeof(int32_t));
			if (!pos.Peek(&Volume, sizeof(random_float)))
			{
				return pos.MarkFailed();
			}
			pos.Skip(sizeof(random_float));
			if (!pos.Peek(&Pitch, sizeof(random_float)))
			{
				return pos.MarkFailed();
			}
			pos.Skip(sizeof(random_float));
			if (!pos.Peek(&PanType, sizeof(ParameterSoundPanType)))
			{
				return pos.MarkFailed();
			}
			pos.Skip(sizeof(ParameterSoundPanType));
			if (!pos.Peek(&Pan, sizeof(random_float)))
			{
				return pos.MarkFailed();
			}
			pos.Skip(sizeof(random_float));
			if (!pos.Peek(&Distance, sizeof(float)))
			{
				return pos.MarkFailed();
			}
			pos.Skip(sizeof(float));
			if (!pos.Peek(&Delay, sizeof(random_int)))
			{
				return pos.MarkFailed();
			}
			pos.Skip(sizeof(random_int));
		}
	}
};

} // namespace Effekseer