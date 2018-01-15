
#ifndef	__EFFEKSEERRENDERER_COMMON_UTILS_H__
#define	__EFFEKSEERRENDERER_COMMON_UTILS_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <Effekseer.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <array>

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------

	static void ApplyDepthOffset(::Effekseer::Matrix43& mat, const ::Effekseer::Matrix44& camera, float depthOffset, bool isDepthOffsetScaledWithCamera, bool isDepthOffsetScaledWithEffect, bool isRightHand)
	{
		if (depthOffset != 0)
		{
			auto f = ::Effekseer::Vector3D(camera.Values[0][2], camera.Values[1][2], camera.Values[2][2]);

			auto offset = depthOffset;

			if (isDepthOffsetScaledWithEffect)
			{
				std::array<float, 3> scales;
				scales.fill(0.0);

				for (auto r = 0; r < 3; r++)
				{
					for (auto c = 0; c < 3; c++)
					{
						scales[c] += mat.Value[c][r] * mat.Value[c][r];
					}
				}

				for (auto c = 0; c < 3; c++)
				{
					scales[c] = sqrt(scales[c]);
				}

				auto scale = (scales[0] + scales[1] + scales[2]) / 3.0f;

				offset *= scale;
			}

			if (isDepthOffsetScaledWithCamera)
			{
				auto cx = mat.Value[3][0] + camera.Values[3][0];
				auto cy = mat.Value[3][1] + camera.Values[3][1];
				auto cz = mat.Value[3][2] + camera.Values[3][2];
				auto cl = sqrt(cx * cx + cy * cy + cz * cz);

				if (cl != 0.0)
				{
					auto scale = (cl - offset) / cl;

					for (auto r = 0; r < 3; r++)
					{
						for (auto c = 0; c < 3; c++)
						{
							mat.Value[c][r] *= scale;
						}
					}
				}
			}

			if (isRightHand)
			{
				mat.Value[3][0] += f.X * offset;
				mat.Value[3][1] += f.Y * offset;
				mat.Value[3][2] += f.Z * offset;
			}
			else
			{
				mat.Value[3][0] -= f.X * offset;
				mat.Value[3][1] -= f.Y * offset;
				mat.Value[3][2] -= f.Z * offset;
			}
		}
	}

	static void ApplyDepthOffset(::Effekseer::Matrix43& mat, const ::Effekseer::Matrix44& camera, ::Effekseer::Vector3D& scaleValues, float depthOffset, bool isDepthOffsetScaledWithCamera, bool isDepthOffsetScaledWithEffect, bool isRightHand)
{
	if (depthOffset != 0)
	{
		auto f = ::Effekseer::Vector3D(camera.Values[0][2], camera.Values[1][2], camera.Values[2][2]);

		auto offset = depthOffset;
		
		if (isDepthOffsetScaledWithEffect)
		{
			auto scale = (scaleValues.X + scaleValues.Y + scaleValues.Z) / 3.0f;

			offset *= scale;
		}

		if (isDepthOffsetScaledWithCamera)
		{
			auto cx = mat.Value[3][0] + camera.Values[3][0];
			auto cy = mat.Value[3][1] + camera.Values[3][1];
			auto cz = mat.Value[3][2] + camera.Values[3][2];
			auto cl = sqrt(cx * cx + cy * cy + cz * cz);

			if (cl != 0.0)
			{
				auto scale = (cl - offset) / cl;

				for (auto r = 0; r < 3; r++)
				{
					for (auto c = 0; c < 3; c++)
					{
						mat.Value[c][r] *= scale;
					}
				}
			}
		}

		if (isRightHand)
		{
			mat.Value[3][0] += f.X * offset;
			mat.Value[3][1] += f.Y * offset;
			mat.Value[3][2] += f.Z * offset;
		}
		else
		{
			mat.Value[3][0] -= f.X * offset;
			mat.Value[3][1] -= f.Y * offset;
			mat.Value[3][2] -= f.Z * offset;
		}

	}
}

	static void ApplyDepthOffset(::Effekseer::Matrix44& mat, const ::Effekseer::Matrix44& camera, float depthOffset, bool isDepthOffsetScaledWithCamera, bool isDepthOffsetScaledWithEffect, bool isRightHand)
{
	if (depthOffset != 0)
	{
		auto f = ::Effekseer::Vector3D(camera.Values[0][2], camera.Values[1][2], camera.Values[2][2]);

		auto offset = depthOffset;

		if (isDepthOffsetScaledWithEffect)
		{
			std::array<float, 3> scales;
			scales.fill(0.0);

			for (auto r = 0; r < 3; r++)
			{
				for (auto c = 0; c < 3; c++)
				{
					scales[c] += mat.Values[c][r] * mat.Values[c][r];
				}
			}

			for (auto c = 0; c < 3; c++)
			{
				scales[c] = sqrt(scales[c]);
			}

			auto scale = (scales[0] + scales[1] + scales[2]) / 3.0f;

			offset *= scale;
		}

		if (isDepthOffsetScaledWithCamera)
		{
			auto cx = mat.Values[3][0] + camera.Values[3][0];
			auto cy = mat.Values[3][1] + camera.Values[3][1];
			auto cz = mat.Values[3][2] + camera.Values[3][2];
			auto cl = sqrt(cx * cx + cy * cy + cz * cz);

			if (cl != 0.0)
			{
				auto scale = (cl - offset) / cl;

				for (auto r = 0; r < 3; r++)
				{
					for (auto c = 0; c < 3; c++)
					{
						mat.Values[c][r] *= scale;
					}
				}
			}
		}

		if (isRightHand)
		{
			mat.Values[3][0] += f.X * offset;
			mat.Values[3][1] += f.Y * offset;
			mat.Values[3][2] += f.Z * offset;
		}
		else
		{
			mat.Values[3][0] -= f.X * offset;
			mat.Values[3][1] -= f.Y * offset;
			mat.Values[3][2] -= f.Z * offset;
		}

	}
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
}
#endif // __EFFEKSEERRENDERER_COMMON_UTILS_H__