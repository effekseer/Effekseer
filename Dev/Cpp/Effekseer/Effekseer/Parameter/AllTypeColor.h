#pragma once

#include "../Effekseer.Base.h"

#include "../Effekseer.FCurves.h"
#include "../Effekseer.InternalScript.h"
#include "Easing.h"
#include "Effekseer.Parameters.h"

#include <stdint.h>

namespace Effekseer
{

class AllTypeColorParameter
{
public:
	enum
	{
		Fixed = 0,
		Random = 1,
		Easing = 2,
		FCurve_RGBA = 3,
		Gradient_ = 4,
		Parameter_DWORD = 0x7fffffff,
	} type;

	union
	{
		struct
		{
			Color all;
		} fixed;

		struct
		{
			random_color all;
		} random;

		struct
		{
			easing_color all;
		} easing;

		struct
		{
			FCurveVectorColor* FCurve;
		} fcurve_rgba;
	};

	// TODO : make variant
	std::unique_ptr<Gradient> gradient;

	AllTypeColorParameter()
	{
		type = Fixed;
	}

	~AllTypeColorParameter()
	{
		if (type == FCurve_RGBA)
		{
			ES_SAFE_DELETE(fcurve_rgba.FCurve);
		}
	}

	void load(uint8_t*& pos, int32_t version)
	{
		memcpy(&type, pos, sizeof(int));
		pos += sizeof(int);

		if (type == Fixed)
		{
			memcpy(&fixed, pos, sizeof(fixed));
			pos += sizeof(fixed);
		}
		else if (type == Random)
		{
			random.all.load(version, pos);
		}
		else if (type == Easing)
		{
			easing.all.load(version, pos);
		}
		else if (type == FCurve_RGBA)
		{
			fcurve_rgba.FCurve = new FCurveVectorColor();
			int32_t size = fcurve_rgba.FCurve->Load(pos, version);
			pos += size;
		}
		else if (type == Gradient_)
		{
			gradient = std::make_unique<Gradient>();
			LoadGradient(*gradient, pos, version);
		}
	}
};

struct InstanceAllTypeColorState
{
	union
	{
		struct
		{
			Color _color;
		} fixed;

		struct
		{
			Color _color;
		} random;

		struct
		{
			Color start;
			Color end;

		} easing;

		struct
		{
			std::array<float, 4> offset;
		} fcurve_rgba;
	};
};

struct AllTypeColorFunctions
{
	static void Init(InstanceAllTypeColorState& allColorValues, IRandObject& rand, const AllTypeColorParameter& SpriteAllColor)
	{
		if (SpriteAllColor.type == AllTypeColorParameter::Fixed)
		{
			allColorValues.fixed._color = SpriteAllColor.fixed.all;
		}
		else if (SpriteAllColor.type == AllTypeColorParameter::Random)
		{
			allColorValues.random._color = SpriteAllColor.random.all.getValue(rand);
		}
		else if (SpriteAllColor.type == AllTypeColorParameter::Easing)
		{
			allColorValues.easing.start = SpriteAllColor.easing.all.getStartValue(rand);
			allColorValues.easing.end = SpriteAllColor.easing.all.getEndValue(rand);
		}
		else if (SpriteAllColor.type == AllTypeColorParameter::FCurve_RGBA)
		{
			allColorValues.fcurve_rgba.offset = SpriteAllColor.fcurve_rgba.FCurve->GetOffsets(rand);
		}
	}

	static Effekseer::Color Calculate(InstanceAllTypeColorState& allColorValues, const AllTypeColorParameter& SpriteAllColor, float m_LivingTime, float m_LivedTime)
	{
		Effekseer::Color _originalColor = {};

		if (SpriteAllColor.type == AllTypeColorParameter::Fixed)
		{
			return allColorValues.fixed._color;
		}
		else if (SpriteAllColor.type == AllTypeColorParameter::Random)
		{
			return allColorValues.random._color;
		}
		if (SpriteAllColor.type == AllTypeColorParameter::Easing)
		{
			float t = Clamp(m_LivingTime / m_LivedTime, 1.0f, 0.0f);

			SpriteAllColor.easing.all.setValueToArg(
				_originalColor, allColorValues.easing.start, allColorValues.easing.end, t);
		}
		else if (SpriteAllColor.type == AllTypeColorParameter::FCurve_RGBA)
		{
			auto fcurveColor = SpriteAllColor.fcurve_rgba.FCurve->GetValues(m_LivingTime, m_LivedTime);
			_originalColor.R = (uint8_t)Clamp((allColorValues.fcurve_rgba.offset[0] + fcurveColor[0]), 255, 0);
			_originalColor.G = (uint8_t)Clamp((allColorValues.fcurve_rgba.offset[1] + fcurveColor[1]), 255, 0);
			_originalColor.B = (uint8_t)Clamp((allColorValues.fcurve_rgba.offset[2] + fcurveColor[2]), 255, 0);
			_originalColor.A = (uint8_t)Clamp((allColorValues.fcurve_rgba.offset[3] + fcurveColor[3]), 255, 0);
		}
		else if (SpriteAllColor.type == AllTypeColorParameter::Gradient_)
		{
			const auto value = SpriteAllColor.gradient->GetColor(Clamp(m_LivingTime / m_LivedTime, 1.0f, 0.0f));
			_originalColor.R = (uint8_t)Clamp(value[0] * 255.0f, 255, 0);
			_originalColor.G = (uint8_t)Clamp(value[1] * 255.0f, 255, 0);
			_originalColor.B = (uint8_t)Clamp(value[2] * 255.0f, 255, 0);
			_originalColor.A = (uint8_t)Clamp(value[3] * 255.0f, 255, 0);
		}

		return _originalColor;
	}
};

} // namespace Effekseer