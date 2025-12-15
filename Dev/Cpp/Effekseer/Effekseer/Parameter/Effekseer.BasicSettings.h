#pragma once

#include "../Effekseer.Base.h"
#include "../Effekseer.Curve.h"
#include "../Effekseer.EffectImplemented.h"
#include "../Effekseer.InternalStruct.h"
#include "../Effekseer.Setting.h"
#include "Effekseer.DynamicParameter.h"
#include "Effekseer.Easing.h"
#include "Effekseer.Trigger.h"

#include <stdint.h>

namespace Effekseer
{

enum class TranslationParentBindType : int32_t
{
	NotBind = 0,
	NotBind_Root = 3,
	WhenCreating = 1,
	Always = 2,
	NotBind_FollowParent = 4,
	WhenCreating_FollowParent = 5,
};

inline bool operator==(const TranslationParentBindType& lhs, const BindType& rhs)
{
	return (lhs == static_cast<TranslationParentBindType>(rhs));
}

enum class RemovalTiming : int32_t
{
	None = 0,
	WhenLifeIsExtinct = 1 << 0,
	WhenParentIsRemoved = 1 << 1,
	WhenChildrenIsExtinct = 1 << 2,
	WhenTriggered = 1 << 3,
};

inline bool HasRemovalTiming(RemovalTiming value, RemovalTiming target)
{
	return (static_cast<int32_t>(value) & static_cast<int32_t>(target)) != 0;
}

enum class GenerationTiming : int32_t
{
	Continuous = 0,
	Trigger = 1,
};

struct ParameterCommonValues_BackCompatibility_8
{
	int MaxGeneration;
	BindType TranslationBindType;
	BindType RotationBindType;
	BindType ScalingBindType;
	int RemoveWhenLifeIsExtinct;
	int RemoveWhenParentIsRemoved;
	int RemoveWhenChildrenIsExtinct;
	random_int life;
	float GenerationTime;
	float GenerationTimeOffset;
};

struct ParameterCommonValues_BackCompatibility_9
{
	int MaxGeneration = 1;
	TranslationParentBindType TranslationBindType = TranslationParentBindType::Always;
	BindType RotationBindType = BindType::Always;
	BindType ScalingBindType = BindType::Always;
	int RemoveWhenLifeIsExtinct = 1;
	int RemoveWhenParentIsRemoved = 0;
	int RemoveWhenChildrenIsExtinct = 0;
	random_int life;
	random_float GenerationTime;
	random_float GenerationTimeOffset;
};

struct ParameterCommonValues_BackCompatibility_17
{
	int32_t RefEqMaxGeneration;
	RefMinMax RefEqLife;
	RefMinMax RefEqGenerationTime;
	RefMinMax RefEqGenerationTimeOffset;

	int MaxGeneration;
	TranslationParentBindType TranslationBindType;
	BindType RotationBindType;
	BindType ScalingBindType;
	int RemoveWhenLifeIsExtinct;
	int RemoveWhenParentIsRemoved;
	int RemoveWhenChildrenIsExtinct;
	random_int life;
	random_float GenerationTime;
	random_float GenerationTimeOffset;
};

struct GenerationParameter
{
	RefMinMax RefEqInterval;
	RefMinMax RefEqOffset;
	RefMinMax RefEqBurst;

	GenerationTiming Type = GenerationTiming::Continuous;
	random_float Interval;
	random_float Offset;
	random_int Burst;

	TriggerValues TriggerToStart;
	TriggerValues TriggerToStop;
	TriggerValues TriggerToGenerate;
};

struct RemovalParameter
{
	RemovalTiming Flags = RemovalTiming::WhenLifeIsExtinct;
	TriggerValues TriggerToRemove;
};

struct ParameterCommonValues_18
{
	int32_t RefEqMaxGeneration;
	RefMinMax RefEqLife;
	RefMinMax RefEqInterval;
	RefMinMax RefEqOffset;
	RefMinMax RefEqBurst;

	int32_t MaxGeneration;
	TranslationParentBindType TranslationBindType;
	BindType RotationBindType;
	BindType ScalingBindType;

	GenerationTiming GenerationType;
	RemovalTiming RemovalFlags;

	random_int life;
	random_float Interval;
	random_float Offset;
	random_int Burst;

	uint16_t TriggerToStart;
	uint16_t TriggerToStop;
	uint16_t TriggerToRemove;
	uint16_t TriggerToGenerate;
};

struct ParameterCommonValues
{
	int32_t RefEqMaxGeneration = -1;
	RefMinMax RefEqLife;

	int MaxGeneration = 1;
	TranslationParentBindType TranslationBindType = TranslationParentBindType::Always;
	BindType RotationBindType = BindType::Always;
	BindType ScalingBindType = BindType::Always;
	random_int life;
	GenerationParameter Generation;
	RemovalParameter Removal;

	ParameterCommonValues()
	{
		life.max = 1;
		life.min = 1;

		Generation.RefEqBurst.Max = -1;
		Generation.RefEqBurst.Min = -1;
		Generation.RefEqInterval.Max = -1;
		Generation.RefEqInterval.Min = -1;
		Generation.RefEqOffset.Max = -1;
		Generation.RefEqOffset.Min = -1;

		Generation.Interval.max = 1;
		Generation.Interval.min = 1;
		Generation.Offset.max = 0;
		Generation.Offset.min = 0;
		Generation.Burst.max = 1;
		Generation.Burst.min = 1;
		Generation.Type = GenerationTiming::Continuous;

		Removal.Flags = RemovalTiming::WhenLifeIsExtinct;
		Removal.TriggerToRemove = {};
	}

	void Load(unsigned char*& pos, const EffectImplemented* ef)
	{
		int32_t size = 0;

		memcpy(&size, pos, sizeof(int));
		pos += sizeof(int);

		if (ef->GetVersion() >= Version18Alpha3)
		{
			ParameterCommonValues_18 param{};
			auto copySize = Min<int32_t>(size, static_cast<int32_t>(sizeof(ParameterCommonValues_18)));
			memcpy(&param, pos, copySize);
			pos += size;

			RefEqMaxGeneration = param.RefEqMaxGeneration;
			RefEqLife = param.RefEqLife;
			Generation.RefEqInterval = param.RefEqInterval;
			Generation.RefEqOffset = param.RefEqOffset;
			Generation.RefEqBurst = param.RefEqBurst;

			MaxGeneration = param.MaxGeneration;
			TranslationBindType = param.TranslationBindType;
			RotationBindType = param.RotationBindType;
			ScalingBindType = param.ScalingBindType;

			Generation.Type = param.GenerationType;
			Removal.Flags = param.RemovalFlags;

			life = param.life;
			Generation.Interval = param.Interval;
			Generation.Offset = param.Offset;
			Generation.Burst = param.Burst;

			auto decodeTrigger = [](uint16_t v) -> TriggerValues
			{
				TriggerValues tv;
				tv.type = static_cast<TriggerType>(v & 0xFF);
				tv.index = static_cast<uint8_t>((v >> 8) & 0xFF);
				return tv;
			};

			Generation.TriggerToStart = decodeTrigger(param.TriggerToStart);
			Generation.TriggerToStop = decodeTrigger(param.TriggerToStop);
			Removal.TriggerToRemove = decodeTrigger(param.TriggerToRemove);
			Generation.TriggerToGenerate = decodeTrigger(param.TriggerToGenerate);
		}
		else if (ef->GetVersion() >= 14)
		{
			assert(size == sizeof(ParameterCommonValues_BackCompatibility_17));
			ParameterCommonValues_BackCompatibility_17 param_17;
			memcpy(&param_17, pos, size);
			pos += size;

			RefEqMaxGeneration = param_17.RefEqMaxGeneration;
			RefEqLife = param_17.RefEqLife;
			Generation.RefEqInterval = param_17.RefEqGenerationTime;
			Generation.RefEqOffset = param_17.RefEqGenerationTimeOffset;
			Generation.RefEqBurst.Max = -1;
			Generation.RefEqBurst.Min = -1;

			MaxGeneration = param_17.MaxGeneration;
			TranslationBindType = param_17.TranslationBindType;

			RotationBindType = param_17.RotationBindType;
			ScalingBindType = param_17.ScalingBindType;
			Removal.Flags = RemovalTiming::None;
			if (param_17.RemoveWhenLifeIsExtinct > 0)
			{
				Removal.Flags = static_cast<RemovalTiming>(static_cast<int32_t>(Removal.Flags) | static_cast<int32_t>(RemovalTiming::WhenLifeIsExtinct));
			}
			if (param_17.RemoveWhenParentIsRemoved > 0)
			{
				Removal.Flags = static_cast<RemovalTiming>(static_cast<int32_t>(Removal.Flags) | static_cast<int32_t>(RemovalTiming::WhenParentIsRemoved));
			}
			if (param_17.RemoveWhenChildrenIsExtinct > 0)
			{
				Removal.Flags = static_cast<RemovalTiming>(static_cast<int32_t>(Removal.Flags) | static_cast<int32_t>(RemovalTiming::WhenChildrenIsExtinct));
			}
			life = param_17.life;
			Generation.Interval = param_17.GenerationTime;
			Generation.Offset = param_17.GenerationTimeOffset;
			Generation.Type = GenerationTiming::Continuous;
			Generation.Burst.max = 1;
			Generation.Burst.min = 1;
			Generation.TriggerToStart = {};
			Generation.TriggerToStop = {};
			Generation.TriggerToGenerate = {};
			Removal.TriggerToRemove = {};
		}
		else if (ef->GetVersion() >= 9)
		{
			ParameterCommonValues_BackCompatibility_9 param_9{};
			memcpy(&param_9, pos, size);
			pos += size;

			RefEqMaxGeneration = -1;
			RefEqLife.Max = -1;
			RefEqLife.Min = -1;
			Generation.RefEqInterval.Max = -1;
			Generation.RefEqInterval.Min = -1;
			Generation.RefEqOffset.Max = -1;
			Generation.RefEqOffset.Min = -1;
			Generation.RefEqBurst.Max = -1;
			Generation.RefEqBurst.Min = -1;

			MaxGeneration = param_9.MaxGeneration;
			TranslationBindType = static_cast<TranslationParentBindType>(param_9.TranslationBindType);

			RotationBindType = param_9.RotationBindType;
			ScalingBindType = param_9.ScalingBindType;
			Removal.Flags = RemovalTiming::None;
			if (param_9.RemoveWhenLifeIsExtinct > 0)
			{
				Removal.Flags = static_cast<RemovalTiming>(static_cast<int32_t>(Removal.Flags) | static_cast<int32_t>(RemovalTiming::WhenLifeIsExtinct));
			}
			if (param_9.RemoveWhenParentIsRemoved > 0)
			{
				Removal.Flags = static_cast<RemovalTiming>(static_cast<int32_t>(Removal.Flags) | static_cast<int32_t>(RemovalTiming::WhenParentIsRemoved));
			}
			if (param_9.RemoveWhenChildrenIsExtinct > 0)
			{
				Removal.Flags = static_cast<RemovalTiming>(static_cast<int32_t>(Removal.Flags) | static_cast<int32_t>(RemovalTiming::WhenChildrenIsExtinct));
			}
			life = param_9.life;
			Generation.Interval = param_9.GenerationTime;
			Generation.Offset = param_9.GenerationTimeOffset;
			Generation.Type = GenerationTiming::Continuous;
			Generation.Burst.max = 1;
			Generation.Burst.min = 1;
			Generation.TriggerToStart = {};
			Generation.TriggerToStop = {};
			Generation.TriggerToGenerate = {};
			Removal.TriggerToRemove = {};
		}
		else
		{
			assert(size == sizeof(ParameterCommonValues_BackCompatibility_8));
			ParameterCommonValues_BackCompatibility_8 param_8;
			memcpy(&param_8, pos, size);
			pos += size;

			RefEqMaxGeneration = -1;
			RefEqLife.Max = -1;
			RefEqLife.Min = -1;
			Generation.RefEqInterval.Max = -1;
			Generation.RefEqInterval.Min = -1;
			Generation.RefEqOffset.Max = -1;
			Generation.RefEqOffset.Min = -1;
			Generation.RefEqBurst.Max = -1;
			Generation.RefEqBurst.Min = -1;

			MaxGeneration = param_8.MaxGeneration;
			TranslationBindType = static_cast<TranslationParentBindType>(param_8.TranslationBindType);

			RotationBindType = param_8.RotationBindType;
			ScalingBindType = param_8.ScalingBindType;
			Removal.Flags = RemovalTiming::None;
			if (param_8.RemoveWhenLifeIsExtinct > 0)
			{
				Removal.Flags = static_cast<RemovalTiming>(static_cast<int32_t>(Removal.Flags) | static_cast<int32_t>(RemovalTiming::WhenLifeIsExtinct));
			}
			if (param_8.RemoveWhenParentIsRemoved > 0)
			{
				Removal.Flags = static_cast<RemovalTiming>(static_cast<int32_t>(Removal.Flags) | static_cast<int32_t>(RemovalTiming::WhenParentIsRemoved));
			}
			if (param_8.RemoveWhenChildrenIsExtinct > 0)
			{
				Removal.Flags = static_cast<RemovalTiming>(static_cast<int32_t>(Removal.Flags) | static_cast<int32_t>(RemovalTiming::WhenChildrenIsExtinct));
			}
			life = param_8.life;
			Generation.Interval.max = param_8.GenerationTime;
			Generation.Interval.min = param_8.GenerationTime;
			Generation.Offset.max = param_8.GenerationTimeOffset;
			Generation.Offset.min = param_8.GenerationTimeOffset;
			Generation.Type = GenerationTiming::Continuous;
			Generation.Burst.max = 1;
			Generation.Burst.min = 1;
			Generation.TriggerToStart = {};
			Generation.TriggerToStop = {};
			Generation.TriggerToGenerate = {};
			Removal.TriggerToRemove = {};
		}
	}
};

} // namespace Effekseer
