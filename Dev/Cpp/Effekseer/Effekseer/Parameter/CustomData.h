#pragma once

#include "../Effekseer.Base.h"
#include "../Effekseer.FCurves.h"
#include "../Effekseer.InstanceGlobal.h"
#include "../Effekseer.InternalStruct.h"

#include <stdint.h>

namespace Effekseer
{

enum class ParameterCustomDataType : int32_t
{
	None = 0,
	Fixed2D = 20,
	Random2D = 21,
	Easing2D = 22,
	FCurve2D = 23,
	Fixed4D = 40,
	FCurveColor = 53,
	DynamicInput = 60,
	Unknown,
};

struct ParameterCustomDataFixed
{
	vector2d Values;
};

struct ParameterCustomDataRandom
{
	random_vector2d Values;
};

struct ParameterCustomDataEasing
{
	easing_vector2d Values;
};

struct ParameterCustomDataFCurve
{
	FCurveVector2D* Values;
};

struct ParameterCustomDataFCurveColor
{
	FCurveVectorColor* Values;
};

struct ParameterCustomData
{
	ParameterCustomDataType Type = ParameterCustomDataType::None;

	union
	{
		ParameterCustomDataFixed Fixed;
		ParameterCustomDataRandom Random;
		ParameterCustomDataEasing Easing;
		ParameterCustomDataFCurve FCurve;
		std::array<float, 4> Fixed4D;
		ParameterCustomDataFCurveColor FCurveColor;
	};

	ParameterCustomData() = default;

	~ParameterCustomData()
	{
		if (Type == ParameterCustomDataType::FCurve2D)
		{
			ES_SAFE_DELETE(FCurve.Values);
		}

		if (Type == ParameterCustomDataType::FCurveColor)
		{
			ES_SAFE_DELETE(FCurveColor.Values);
		}
	}

	void load(uint8_t*& pos, int32_t version)
	{
		memcpy(&Type, pos, sizeof(int));
		pos += sizeof(int);

		if (Type == ParameterCustomDataType::None)
		{
		}
		else if (Type == ParameterCustomDataType::Fixed2D)
		{
			memcpy(&Fixed.Values, pos, sizeof(Fixed));
			pos += sizeof(Fixed);
		}
		else if (Type == ParameterCustomDataType::Random2D)
		{
			memcpy(&Random.Values, pos, sizeof(Random));
			pos += sizeof(Random);
		}
		else if (Type == ParameterCustomDataType::Easing2D)
		{
			memcpy(&Easing.Values, pos, sizeof(Easing));
			pos += sizeof(Easing);
		}
		else if (Type == ParameterCustomDataType::FCurve2D)
		{
			FCurve.Values = new FCurveVector2D();
			pos += FCurve.Values->Load(pos, version);
		}
		else if (Type == ParameterCustomDataType::Fixed4D)
		{
			memcpy(Fixed4D.data(), pos, sizeof(float) * 4);
			pos += sizeof(float) * 4;
		}
		else if (Type == ParameterCustomDataType::FCurveColor)
		{
			FCurveColor.Values = new FCurveVectorColor();
			pos += FCurveColor.Values->Load(pos, version);
		}
		else if (Type == ParameterCustomDataType::DynamicInput)
		{
		}
		else
		{
			assert(0);
		}
	}
};

struct InstanceCustomData
{
	union
	{
		struct
		{
			SIMD::Vec2f start;
			SIMD::Vec2f end;
		} easing;

		struct
		{
			SIMD::Vec2f value;
		} random;

		struct
		{
			SIMD::Vec2f offset;
		} fcruve;

		struct
		{
			std::array<float, 4> offset;
		} fcurveColor;
	};
};

struct CustomDataFunctions
{
	static void InitCustomData(InstanceCustomData& instanceCustomData, RandObject& rand, const ParameterCustomData* parameterCustomData)
	{
		if (parameterCustomData->Type == ParameterCustomDataType::Fixed2D)
		{
			// none
		}
		else if (parameterCustomData->Type == ParameterCustomDataType::Easing2D)
		{
			instanceCustomData.easing.start = parameterCustomData->Easing.Values.start.getValue(rand);
			instanceCustomData.easing.end = parameterCustomData->Easing.Values.end.getValue(rand);
		}
		else if (parameterCustomData->Type == ParameterCustomDataType::Random2D)
		{
			instanceCustomData.random.value = parameterCustomData->Random.Values.getValue(rand);
		}
		else if (parameterCustomData->Type == ParameterCustomDataType::FCurve2D)
		{
			instanceCustomData.fcruve.offset = parameterCustomData->FCurve.Values->GetOffsets(rand);
		}
		else if (parameterCustomData->Type == ParameterCustomDataType::FCurveColor)
		{
			instanceCustomData.fcurveColor.offset = parameterCustomData->FCurveColor.Values->GetOffsets(rand);
		}
	}

	static std::array<float, 4> GetCustomData(const ParameterCustomData* parameterCustomData, const InstanceCustomData* instanceCustomData, const InstanceGlobal* instanceGlobal, float livingTime, float livedTime)
	{
		if (parameterCustomData->Type == ParameterCustomDataType::None)
		{
			return std::array<float, 4>{0, 0, 0, 0};
		}
		else if (parameterCustomData->Type == ParameterCustomDataType::Fixed2D)
		{
			auto v = parameterCustomData->Fixed.Values;
			return std::array<float, 4>{v.x, v.y, 0, 0};
		}
		else if (parameterCustomData->Type == ParameterCustomDataType::Random2D)
		{
			auto v = instanceCustomData->random.value;
			return std::array<float, 4>{v.GetX(), v.GetY(), 0, 0};
		}
		else if (parameterCustomData->Type == ParameterCustomDataType::Easing2D)
		{
			SIMD::Vec2f v = parameterCustomData->Easing.Values.getValue(
				instanceCustomData->easing.start, instanceCustomData->easing.end, Clamp(livingTime / livedTime, 1.0F, 0.0F));
			return std::array<float, 4>{v.GetX(), v.GetY(), 0, 0};
		}
		else if (parameterCustomData->Type == ParameterCustomDataType::FCurve2D)
		{
			auto values = parameterCustomData->FCurve.Values->GetValues(livingTime, livedTime);
			return std::array<float, 4>{
				values.GetX() + instanceCustomData->fcruve.offset.GetX(), values.GetY() + instanceCustomData->fcruve.offset.GetY(), 0, 0};
		}
		else if (parameterCustomData->Type == ParameterCustomDataType::Fixed4D)
		{
			return parameterCustomData->Fixed4D;
		}
		else if (parameterCustomData->Type == ParameterCustomDataType::FCurveColor)
		{
			auto values = parameterCustomData->FCurveColor.Values->GetValues(livingTime, livedTime);
			return std::array<float, 4>{(values[0] + instanceCustomData->fcurveColor.offset[0]) / 255.0f,
										(values[1] + instanceCustomData->fcurveColor.offset[1]) / 255.0f,
										(values[2] + instanceCustomData->fcurveColor.offset[2]) / 255.0f,
										(values[3] + instanceCustomData->fcurveColor.offset[3]) / 255.0f};
		}
		else if (parameterCustomData->Type == ParameterCustomDataType::DynamicInput)
		{
			return instanceGlobal->GetDynamicInputParameters();
		}
		else
		{
			assert(false);
		}

		return std::array<float, 4>{0, 0, 0, 0};
	}
};

} // namespace Effekseer