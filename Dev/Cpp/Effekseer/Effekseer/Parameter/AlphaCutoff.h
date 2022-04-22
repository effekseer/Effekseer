#pragma once

#include "../Effekseer.Base.h"

#include "../Effekseer.FCurves.h"
#include "../Effekseer.InternalScript.h"

#include "Easing.h"

#include <stdint.h>

namespace Effekseer
{

struct ParameterAlphaCutoff
{
	enum class EType : int32_t
	{
		FIXED,
		FOUR_POINT_INTERPOLATION,
		EASING,
		F_CURVE,

		FPI = FOUR_POINT_INTERPOLATION,
	};

	EType Type = EType::FIXED;

	struct
	{
		int32_t RefEq = -1;
		float Threshold = 0.0f;
	} Fixed;

	struct
	{
		random_float BeginThreshold;
		random_int TransitionFrameNum;
		random_float No2Threshold;
		random_float No3Threshold;
		random_int TransitionFrameNum2;
		random_float EndThreshold;
	} FourPointInterpolation;

	ParameterEasingFloat Easing{Version16Alpha1, Version16Alpha1};

	struct
	{
		std::unique_ptr<FCurveScalar> Threshold;
	} FCurve;

	float EdgeThreshold = 0.0f;
	Color EdgeColor = Color(0, 0, 0, 0);
	float EdgeColorScaling = 0.0f;

	void load(uint8_t*& pos, int32_t version)
	{
		memcpy(&Type, pos, sizeof(int32_t));
		pos += sizeof(int32_t);

		int32_t BufferSize = 0;
		memcpy(&BufferSize, pos, sizeof(int32_t));
		pos += sizeof(int32_t);

		switch (Type)
		{
		case Effekseer::ParameterAlphaCutoff::EType::FIXED:
			memcpy(&Fixed, pos, BufferSize);
			break;
		case Effekseer::ParameterAlphaCutoff::EType::FPI:
			memcpy(&FourPointInterpolation, pos, BufferSize);
			break;
		case Effekseer::ParameterAlphaCutoff::EType::EASING:
			Easing.Load(pos, BufferSize, version);
			break;
		case Effekseer::ParameterAlphaCutoff::EType::F_CURVE:
			FCurve.Threshold = std::make_unique<FCurveScalar>();
			FCurve.Threshold->Load(pos, version);
			break;
		}

		pos += BufferSize;

		memcpy(&EdgeThreshold, pos, sizeof(int32_t));
		pos += sizeof(int32_t);

		memcpy(&EdgeColor, pos, sizeof(Color));
		pos += sizeof(int32_t);

		if (version >= Version16Alpha7)
		{
			memcpy(&EdgeColorScaling, pos, sizeof(float));
			pos += sizeof(float);
		}
		else
		{
			int32_t temp = 0;
			memcpy(&temp, pos, sizeof(int32_t));
			pos += sizeof(int32_t);
			EdgeColorScaling = static_cast<float>(temp);
		}
	}
};

struct AlphaCuttoffState
{
	union
	{
		struct
		{
		} fixed;

		struct
		{
			float begin_threshold;
			int32_t transition_frame;
			float no2_threshold;
			float no3_threshold;
			int32_t transition_frame2;
			float end_threshold;
		} four_point_interpolation;

		InstanceEasing<float> easing;

		struct
		{
			float offset;
		} fcurve;
	};
};

struct AlphaCutoffFunctions
{
	static float CalcAlphaThreshold(RandObject& rand, const Instance* parent, const ParameterAlphaCutoff& alphaCutoff, const AlphaCuttoffState& alpha_cutoff_values, const Effect* effect, const InstanceGlobal* instanceGlobal, float livingTime, float livedTime)
	{
		if (alphaCutoff.Type == ParameterAlphaCutoff::EType::FIXED)
		{
			if (alphaCutoff.Fixed.RefEq >= 0)
			{
				auto alphaThreshold = static_cast<float>(alphaCutoff.Fixed.Threshold);
				ApplyEq(alphaThreshold,
						effect,
						instanceGlobal,
						parent,
						&rand,
						alphaCutoff.Fixed.RefEq,
						alphaThreshold);

				return alphaThreshold;
			}
			else
			{
				return alphaCutoff.Fixed.Threshold;
			}
		}
		else if (alphaCutoff.Type == ParameterAlphaCutoff::EType::FPI)
		{
			float t = Clamp(livingTime / livedTime, 1.0F, 0.0F);
			auto val = alpha_cutoff_values.four_point_interpolation;

			float p[4][2];
			p[0][0] = 0.0f;
			p[0][1] = val.begin_threshold;
			p[1][0] = float(val.transition_frame) / livedTime;
			p[1][1] = val.no2_threshold;
			p[2][0] = (livedTime - float(val.transition_frame2)) / livedTime;
			p[2][1] = val.no3_threshold;
			p[3][0] = 1.0f;
			p[3][1] = val.end_threshold;

			float alphaThreshold = 1.0f;

			for (int32_t i = 1; i < 4; i++)
			{
				if (0 < p[i][0] && p[i - 1][0] <= t && t <= p[i][0])
				{
					float r = (t - p[i - 1][0]) / (p[i][0] - p[i - 1][0]);
					alphaThreshold = p[i - 1][1] + (p[i][1] - p[i - 1][1]) * r;
					break;
				}
			}

			return alphaThreshold;
		}
		else if (alphaCutoff.Type == ParameterAlphaCutoff::EType::EASING)
		{
			return alphaCutoff.Easing.GetValue(alpha_cutoff_values.easing, Clamp(livingTime / livedTime, 1.0F, 0.0F));
		}
		else if (alphaCutoff.Type == ParameterAlphaCutoff::EType::F_CURVE)
		{
			auto fcurve = alphaCutoff.FCurve.Threshold->GetValues(livingTime, livedTime);
			auto alphaThreshold = fcurve + alpha_cutoff_values.fcurve.offset;
			alphaThreshold /= 100.0f;
			return alphaThreshold;
		}

		return 0.0f;
	}

	static float InitAlphaThreshold(AlphaCuttoffState& alpha_cutoff_values, RandObject& rand, const ParameterAlphaCutoff& alphaCutoff, const Instance* parent, const Effect* effect, const InstanceGlobal* instanceGlobal)
	{
		if (alphaCutoff.Type == ParameterAlphaCutoff::EType::FIXED)
		{
			if (alphaCutoff.Fixed.RefEq < 0)
			{
				return alphaCutoff.Fixed.Threshold;
			}
		}
		else if (alphaCutoff.Type == ParameterAlphaCutoff::EType::FPI)
		{
			auto& fpiValue = alpha_cutoff_values.four_point_interpolation;
			auto& nodeAlphaCutoffValue = alphaCutoff.FourPointInterpolation;

			fpiValue.begin_threshold = nodeAlphaCutoffValue.BeginThreshold.getValue(rand);
			fpiValue.transition_frame = static_cast<int32_t>(nodeAlphaCutoffValue.TransitionFrameNum.getValue(rand));
			fpiValue.no2_threshold = nodeAlphaCutoffValue.No2Threshold.getValue(rand);
			fpiValue.no3_threshold = nodeAlphaCutoffValue.No3Threshold.getValue(rand);
			fpiValue.transition_frame2 = static_cast<int32_t>(nodeAlphaCutoffValue.TransitionFrameNum2.getValue(rand));
			fpiValue.end_threshold = nodeAlphaCutoffValue.EndThreshold.getValue(rand);
		}
		else if (alphaCutoff.Type == ParameterAlphaCutoff::EType::EASING)
		{
			alphaCutoff.Easing.Init(alpha_cutoff_values.easing, effect, instanceGlobal, parent, &rand);
		}
		else if (alphaCutoff.Type == ParameterAlphaCutoff::EType::F_CURVE)
		{
			auto& fcurveValue = alpha_cutoff_values.fcurve;
			auto& nodeAlphaCutoffValue = alphaCutoff.FCurve;

			fcurveValue.offset = nodeAlphaCutoffValue.Threshold->GetOffsets(rand);
		}

		return 0.0f;
	}
};

} // namespace Effekseer