
#ifndef	__EFFEKSEERRENDERER_COMMON_UTILS_H__
#define	__EFFEKSEERRENDERER_COMMON_UTILS_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <Effekseer.h>
#include <assert.h>
#include <string.h>
#include <math.h>

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------

static void ApplyDepthOffset(::Effekseer::Matrix43& mat, const ::Effekseer::Matrix44& camera, float depthOffset, bool isDepthOffsetScaledWithCamera, bool isDepthOffsetScaledWithEffect)
{
	if (depthOffset != 0)
	{
		auto f = ::Effekseer::Vector3D(camera.Values[0][2], camera.Values[1][2], camera.Values[2][2]);

		auto offset = depthOffset;
		mat.Value[3][0] += f.X * depthOffset;
		mat.Value[3][1] += f.Y * depthOffset;
		mat.Value[3][2] += f.Z * depthOffset;

		if (isDepthOffsetScaledWithCamera)
		{
			auto cx = mat.Value[3][0] - camera.Values[3][0];
			auto cy = mat.Value[3][1] - camera.Values[3][1];
			auto cz = mat.Value[3][2] - camera.Values[3][2];
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
	}
}

static void ApplyDepthOffset(::Effekseer::Matrix44& mat, const ::Effekseer::Matrix44& camera, float depthOffset, bool isDepthOffsetScaledWithCamera, bool isDepthOffsetScaledWithEffect)
{
	if (depthOffset != 0)
	{
		auto f = ::Effekseer::Vector3D(camera.Values[0][2], camera.Values[1][2], camera.Values[2][2]);

		auto offset = depthOffset;
		mat.Values[3][0] += f.X * depthOffset;
		mat.Values[3][1] += f.Y * depthOffset;
		mat.Values[3][2] += f.Z * depthOffset;

		if (isDepthOffsetScaledWithCamera)
		{
			auto cx = mat.Values[3][0] - camera.Values[3][0];
			auto cy = mat.Values[3][1] - camera.Values[3][1];
			auto cz = mat.Values[3][2] - camera.Values[3][2];
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
	}
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
}
#endif // __EFFEKSEERRENDERER_COMMON_UTILS_H__