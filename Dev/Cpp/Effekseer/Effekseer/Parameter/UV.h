#pragma once

#include "../Effekseer.Base.h"
#include "../Effekseer.FCurves.h"

#include "Easing.h"

#include <stdint.h>

namespace Effekseer
{
enum class UVAnimationType : int32_t
{
	Default = 0,
	Fixed = 1,
	Animation = 2,
	Scroll = 3,
	FCurve = 4,
};

struct InstanceUVState
{
	//! The time offset for UV animation
	int32_t uvTimeOffset;

	// Scroll, FCurve area for UV
	RectF uvAreaOffset;

	// Scroll speed for UV
	SIMD::Vec2f uvScrollSpeed;
};

struct UVParameter
{
	UVAnimationType Type = UVAnimationType::Default;

	union
	{
		struct
		{
		} Default;

		struct
		{
			rectf Position;
		} Fixed;

		struct
		{
			rectf Position;
			int32_t FrameLength;
			int32_t FrameCountX;
			int32_t FrameCountY;

			enum
			{
				LOOPTYPE_ONCE = 0,
				LOOPTYPE_LOOP = 1,
				LOOPTYPE_REVERSELOOP = 2,

				LOOPTYPE_DWORD = 0x7fffffff,
			} LoopType;

			random_int StartFrame;

			enum
			{
				NONE = 0,
				LERP = 1,
			} InterpolationType;

		} Animation;

		struct
		{
			random_vector2d Position;
			random_vector2d Size;
			random_vector2d Speed;
		} Scroll;

		struct
		{
			FCurveVector2D* Position;
			FCurveVector2D* Size;
		} FCurve;
	};

	~UVParameter()
	{
		if (Type == UVAnimationType::FCurve)
		{
			ES_SAFE_DELETE(FCurve.Position);
			ES_SAFE_DELETE(FCurve.Size);
		}
	}

	void Load(uint8_t*& pos, int32_t version, int uvIndex)
	{
		memcpy(&Type, pos, sizeof(int));
		pos += sizeof(int);

		if (Type == UVAnimationType::Default)
		{
		}
		else if (Type == UVAnimationType::Fixed)
		{
			memcpy(&Fixed, pos, sizeof(Fixed));
			pos += sizeof(Fixed);
		}
		else if (Type == UVAnimationType::Animation)
		{
			memcpy(&Animation.Position, pos, sizeof(Animation.Position));
			pos += sizeof(Animation.Position);

			memcpy(&Animation.FrameLength, pos, sizeof(Animation.FrameLength));
			pos += sizeof(Animation.FrameLength);

			memcpy(&Animation.FrameCountX, pos, sizeof(Animation.FrameCountX));
			pos += sizeof(Animation.FrameCountX);

			memcpy(&Animation.FrameCountY, pos, sizeof(Animation.FrameCountY));
			pos += sizeof(Animation.FrameCountY);

			memcpy(&Animation.LoopType, pos, sizeof(Animation.LoopType));
			pos += sizeof(Animation.LoopType);

			memcpy(&Animation.StartFrame, pos, sizeof(Animation.StartFrame));
			pos += sizeof(Animation.StartFrame);

			if (version >= 1600 && uvIndex == 0)
			{
				memcpy(&Animation.InterpolationType, pos, sizeof(Animation.InterpolationType));
				pos += sizeof(Animation.InterpolationType);
			}
			else
			{
				Animation.InterpolationType = Animation.NONE;
			}
		}
		else if (Type == UVAnimationType::Scroll)
		{
			memcpy(&Scroll, pos, sizeof(Scroll));
			pos += sizeof(Scroll);
		}
		else if (Type == UVAnimationType::FCurve)
		{
			FCurve.Position = new FCurveVector2D();
			FCurve.Size = new FCurveVector2D();
			pos += FCurve.Position->Load(pos, version);
			pos += FCurve.Size->Load(pos, version);
		}
	}
};

struct UVFunctions
{
	static NodeRendererFlipbookParameter ToFlipbookParameter(const UVParameter& uv)
	{
		NodeRendererFlipbookParameter flipbook;

		flipbook.EnableInterpolation = (uv.Animation.InterpolationType != uv.Animation.NONE);
		flipbook.UVLoopType = uv.Animation.LoopType;
		flipbook.InterpolationType = uv.Animation.InterpolationType;
		flipbook.FlipbookDivideX = uv.Animation.FrameCountX;
		flipbook.FlipbookDivideY = uv.Animation.FrameCountY;
		flipbook.OneSize[0] = uv.Animation.Position.w;
		flipbook.OneSize[1] = uv.Animation.Position.h;
		flipbook.Offset[0] = uv.Animation.Position.x;
		flipbook.Offset[1] = uv.Animation.Position.y;

		return flipbook;
	}

	static bool IsInfiniteValue(int value)
	{
		return std::numeric_limits<int32_t>::max() / 1000 < value;
	}

	static void InitUVState(InstanceUVState& data, RandObject& rand, const UVParameter& UV)
	{
		const auto UVType = UV.Type;

		if (UVType == UVAnimationType::Animation)
		{
			auto& uvTimeOffset = data.uvTimeOffset;
			uvTimeOffset = (int32_t)UV.Animation.StartFrame.getValue(rand);

			if (!IsInfiniteValue(UV.Animation.FrameLength))
			{
				uvTimeOffset *= UV.Animation.FrameLength;
			}
		}
		else if (UVType == UVAnimationType::Scroll)
		{
			auto& uvAreaOffset = data.uvAreaOffset;
			auto& uvScrollSpeed = data.uvScrollSpeed;

			auto xy = UV.Scroll.Position.getValue(rand);
			auto zw = UV.Scroll.Size.getValue(rand);

			uvAreaOffset.X = xy.GetX();
			uvAreaOffset.Y = xy.GetY();
			uvAreaOffset.Width = zw.GetX();
			uvAreaOffset.Height = zw.GetY();

			uvScrollSpeed = UV.Scroll.Speed.getValue(rand);
		}
		else if (UVType == UVAnimationType::FCurve)
		{
			auto& uvAreaOffset = data.uvAreaOffset;

			uvAreaOffset.X = UV.FCurve.Position->X.GetOffset(rand);
			uvAreaOffset.Y = UV.FCurve.Position->Y.GetOffset(rand);
			uvAreaOffset.Width = UV.FCurve.Size->X.GetOffset(rand);
			uvAreaOffset.Height = UV.FCurve.Size->Y.GetOffset(rand);
		}
	}

	static RectF GetUV(const InstanceUVState& data, const UVParameter& UV, float m_LivingTime, float m_LivedTime)
	{
		const auto uvType = UV.Type;

		RectF uv(0.0f, 0.0f, 1.0f, 1.0f);

		if (uvType == UVAnimationType::Default)
		{
			return RectF(0.0f, 0.0f, 1.0f, 1.0f);
		}
		else if (uvType == UVAnimationType::Fixed)
		{
			uv = RectF(UV.Fixed.Position.x, UV.Fixed.Position.y, UV.Fixed.Position.w, UV.Fixed.Position.h);
		}
		else if (uvType == UVAnimationType::Animation)
		{
			auto uvTimeOffsetF = static_cast<float>(data.uvTimeOffset);

			float time{};
			int frameLength = UV.Animation.FrameLength;

			if (IsInfiniteValue(frameLength))
			{
				time = uvTimeOffsetF;
				frameLength = 1;
			}
			else
			{
				time = m_LivingTime + uvTimeOffsetF;
			}

			int32_t frameNum = (int32_t)(time / frameLength);
			int32_t frameCount = UV.Animation.FrameCountX * UV.Animation.FrameCountY;

			if (UV.Animation.LoopType == UV.Animation.LOOPTYPE_ONCE)
			{
				if (frameNum >= frameCount)
				{
					frameNum = frameCount - 1;
				}
			}
			else if (UV.Animation.LoopType == UV.Animation.LOOPTYPE_LOOP)
			{
				frameNum %= frameCount;
			}
			else if (UV.Animation.LoopType == UV.Animation.LOOPTYPE_REVERSELOOP)
			{
				bool rev = (frameNum / frameCount) % 2 == 1;
				frameNum %= frameCount;
				if (rev)
				{
					frameNum = frameCount - 1 - frameNum;
				}
			}

			int32_t frameX = frameNum % UV.Animation.FrameCountX;
			int32_t frameY = frameNum / UV.Animation.FrameCountX;

			uv = RectF(UV.Animation.Position.x + UV.Animation.Position.w * frameX,
					   UV.Animation.Position.y + UV.Animation.Position.h * frameY,
					   UV.Animation.Position.w,
					   UV.Animation.Position.h);
		}
		else if (uvType == UVAnimationType::Scroll)
		{
			auto time = (int32_t)m_LivingTime;

			uv = RectF(data.uvAreaOffset.X + data.uvScrollSpeed.GetX() * time,
					   data.uvAreaOffset.Y + data.uvScrollSpeed.GetY() * time,
					   data.uvAreaOffset.Width,
					   data.uvAreaOffset.Height);
		}
		else if (uvType == UVAnimationType::FCurve)
		{
			auto fcurvePos = UV.FCurve.Position->GetValues(m_LivingTime, m_LivedTime);
			auto fcurveSize = UV.FCurve.Size->GetValues(m_LivingTime, m_LivedTime);

			uv = RectF(data.uvAreaOffset.X + fcurvePos.GetX(),
					   data.uvAreaOffset.Y + fcurvePos.GetY(),
					   data.uvAreaOffset.Width + fcurveSize.GetX(),
					   data.uvAreaOffset.Height + fcurveSize.GetY());
		}

		// For webgl bug (it makes slow if sampling points are too far on WebGL)
		const float looppoint_uv = 4.0f;

		if (uv.X < -looppoint_uv && uv.X + uv.Width < -looppoint_uv)
		{
			uv.X += (-static_cast<int32_t>(uv.X) - looppoint_uv);
		}

		if (uv.X > looppoint_uv && uv.X + uv.Width > looppoint_uv)
		{
			uv.X -= (static_cast<int32_t>(uv.X) - looppoint_uv);
		}

		if (uv.Y < -looppoint_uv && uv.Y + uv.Height < -looppoint_uv)
		{
			uv.Y += (-static_cast<int32_t>(uv.Y) - looppoint_uv);
		}

		if (uv.Y > looppoint_uv && uv.Y + uv.Height > looppoint_uv)
		{
			uv.Y -= (static_cast<int32_t>(uv.Y) - looppoint_uv);
		}

		return uv;
	}
};

} // namespace Effekseer