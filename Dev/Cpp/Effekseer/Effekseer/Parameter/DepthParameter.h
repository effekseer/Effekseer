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
struct ParameterDepthValues
{
	float DepthOffset = 0;
	bool IsDepthOffsetScaledWithCamera = false;
	bool IsDepthOffsetScaledWithParticleScale = false;
	ZSortType ZSort = ZSortType::None;
	int32_t DrawingPriority = 0;
	float SoftParticle = 0.0f;

	NodeRendererDepthParameter DepthParameter;

	void Load(unsigned char*& pos, int version)
	{
		if (version >= 12)
		{
			memcpy(&DepthOffset, pos, sizeof(float));
			pos += sizeof(float);

			auto isDepthOffsetScaledWithCamera = 0;
			memcpy(&isDepthOffsetScaledWithCamera, pos, sizeof(int32_t));
			pos += sizeof(int32_t);

			IsDepthOffsetScaledWithCamera = isDepthOffsetScaledWithCamera > 0;

			auto isDepthOffsetScaledWithParticleScale = 0;
			memcpy(&isDepthOffsetScaledWithParticleScale, pos, sizeof(int32_t));
			pos += sizeof(int32_t);

			IsDepthOffsetScaledWithParticleScale = isDepthOffsetScaledWithParticleScale > 0;

			if (version >= 15)
			{
				memcpy(&DepthParameter.SuppressionOfScalingByDepth, pos, sizeof(float));
				pos += sizeof(float);

				memcpy(&DepthParameter.DepthClipping, pos, sizeof(float));
				pos += sizeof(float);
			}

			if (version >= 13)
			{
				memcpy(&ZSort, pos, sizeof(int32_t));
				pos += sizeof(int32_t);

				memcpy(&DrawingPriority, pos, sizeof(int32_t));
				pos += sizeof(int32_t);
			}

			memcpy(&SoftParticle, pos, sizeof(float));
			pos += sizeof(float);
		}
	}
};
} // namespace Effekseer