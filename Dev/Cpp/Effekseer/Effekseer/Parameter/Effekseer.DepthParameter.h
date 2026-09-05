#pragma once

#include "../Utils/Effekseer.BinaryReader.h"
#include "../Effekseer.Base.h"
#include "../Effekseer.Curve.h"
#include "../Effekseer.EffectImplemented.h"
#include "../Effekseer.InternalStruct.h"
#include "../Effekseer.Setting.h"
#include "Effekseer.DynamicParameter.h"
#include "Effekseer.Easing.h"

#include <stdint.h>

namespace Effekseer
{
struct ParameterDepthValues
{
	float DepthOffset = 0;
	bool IsDepthOffsetScaledWithCamera = false;
	bool IsDepthOffsetScaledWithParticleScale = false;
	ZSortType ZSort = ZSortType::None;
	int32_t DrawingPriority = 0;
	float SoftParticle = 0.0f;

	NodeRendererDepthParameter DepthParameter;

	void Load(BinaryReader<true>& pos, int version)
	{
		if (version >= 12)
		{
			if (!pos.Peek(&DepthOffset, sizeof(float)))
			{
				return pos.MarkFailed();
			}
			pos.Skip(sizeof(float));

			auto isDepthOffsetScaledWithCamera = 0;
			if (!pos.Peek(&isDepthOffsetScaledWithCamera, sizeof(int32_t)))
			{
				return pos.MarkFailed();
			}
			pos.Skip(sizeof(int32_t));

			IsDepthOffsetScaledWithCamera = isDepthOffsetScaledWithCamera > 0;

			auto isDepthOffsetScaledWithParticleScale = 0;
			if (!pos.Peek(&isDepthOffsetScaledWithParticleScale, sizeof(int32_t)))
			{
				return pos.MarkFailed();
			}
			pos.Skip(sizeof(int32_t));

			IsDepthOffsetScaledWithParticleScale = isDepthOffsetScaledWithParticleScale > 0;

			if (version >= 15)
			{
				if (!pos.Peek(&DepthParameter.SuppressionOfScalingByDepth, sizeof(float)))
				{
					return pos.MarkFailed();
				}
				pos.Skip(sizeof(float));

				if (!pos.Peek(&DepthParameter.DepthClipping, sizeof(float)))
				{
					return pos.MarkFailed();
				}
				pos.Skip(sizeof(float));
			}

			if (version >= 13)
			{
				if (!pos.Peek(&ZSort, sizeof(int32_t)))
				{
					return pos.MarkFailed();
				}
				pos.Skip(sizeof(int32_t));

				if (!pos.Peek(&DrawingPriority, sizeof(int32_t)))
				{
					return pos.MarkFailed();
				}
				pos.Skip(sizeof(int32_t));
			}

			if (!pos.Peek(&SoftParticle, sizeof(float)))
			{
				return pos.MarkFailed();
			}
			pos.Skip(sizeof(float));
		}
	}
};
} // namespace Effekseer