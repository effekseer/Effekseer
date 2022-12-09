#pragma once

#include "../Effekseer.Base.h"
#include "../Effekseer.Curve.h"
#include "../Effekseer.EffectImplemented.h"
#include "../Effekseer.InternalStruct.h"
#include "../Effekseer.Setting.h"
#include "DynamicParameter.h"
#include "Easing.h"

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

struct ParameterCommonValues_8
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

struct ParameterCommonValues
{
	int32_t RefEqMaxGeneration = -1;
	RefMinMax RefEqLife;
	RefMinMax RefEqGenerationTime;
	RefMinMax RefEqGenerationTimeOffset;

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

	ParameterCommonValues()
	{
		life.max = 1;
		life.min = 1;
		GenerationTime.max = 1;
		GenerationTime.min = 1;
		GenerationTimeOffset.max = 0;
		GenerationTimeOffset.min = 0;
	}

	void Load(unsigned char*& pos, const EffectImplemented* ef)
	{
		int32_t size = 0;

		memcpy(&size, pos, sizeof(int));
		pos += sizeof(int);

		if (ef->GetVersion() >= 14)
		{
			assert(size == sizeof(ParameterCommonValues));
			memcpy(this, pos, size);
			pos += size;
		}
		else if (ef->GetVersion() >= 9)
		{
			memcpy(&MaxGeneration, pos, size);
			pos += size;
		}
		else
		{
			assert(size == sizeof(ParameterCommonValues_8));
			ParameterCommonValues_8 param_8;
			memcpy(&param_8, pos, size);
			pos += size;

			MaxGeneration = param_8.MaxGeneration;
			TranslationBindType = static_cast<TranslationParentBindType>(param_8.TranslationBindType);

			RotationBindType = param_8.RotationBindType;
			ScalingBindType = param_8.ScalingBindType;
			RemoveWhenLifeIsExtinct = param_8.RemoveWhenLifeIsExtinct;
			RemoveWhenParentIsRemoved = param_8.RemoveWhenParentIsRemoved;
			RemoveWhenChildrenIsExtinct = param_8.RemoveWhenChildrenIsExtinct;
			life = param_8.life;
			GenerationTime.max = param_8.GenerationTime;
			GenerationTime.min = param_8.GenerationTime;
			GenerationTimeOffset.max = param_8.GenerationTimeOffset;
			GenerationTimeOffset.min = param_8.GenerationTimeOffset;
		}
	}
};

} // namespace Effekseer