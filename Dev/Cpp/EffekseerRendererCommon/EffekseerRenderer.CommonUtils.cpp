

#include "EffekseerRenderer.CommonUtils.h"


namespace EffekseerRenderer
{

void ApplyDepthParameters(::Effekseer::Matrix43& mat,
	const ::Effekseer::Vector3D& cameraFront,
	const ::Effekseer::Vector3D& cameraPos,
	float depthOffset,
	bool isDepthOffsetScaledWithCamera,
	bool isDepthOffsetScaledWithEffect,
	::Effekseer::NodeRendererDepthParameter* depthParameter,
	bool isRightHand)
{
	if (depthOffset != 0)
	{
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
			auto cx = mat.Value[3][0] + cameraPos.X;
			auto cy = mat.Value[3][1] + cameraPos.Y;
			auto cz = mat.Value[3][2] + cameraPos.Z;
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

		auto objPos = ::Effekseer::Vector3D(mat.Value[3][0], mat.Value[3][1], mat.Value[3][2]);
		auto dir = cameraPos - objPos;
		Effekseer::Vector3D::Normal(dir, dir);

		if (isRightHand)
		{
			mat.Value[3][0] += dir.X * offset;
			mat.Value[3][1] += dir.Y * offset;
			mat.Value[3][2] += dir.Z * offset;
		}
		else
		{
			mat.Value[3][0] += dir.X * offset;
			mat.Value[3][1] += dir.Y * offset;
			mat.Value[3][2] += dir.Z * offset;
		}
	}

	if (depthParameter->SuppressionOfScalingByDepth < 1.0f)
	{
		auto cx = mat.Value[3][0] - cameraPos.X;
		auto cy = mat.Value[3][1] - cameraPos.Y;
		auto cz = mat.Value[3][2] - cameraPos.Z;
		auto cl = sqrt(cx * cx + cy * cy + cz * cz);
		//auto cl = cameraFront.X * cx + cameraFront.Y * cy * cameraFront.Z * cz;


		if (cl != 0.0)
		{
			auto scale = cl / 32.0f * (1.0f - depthParameter->SuppressionOfScalingByDepth) + depthParameter->SuppressionOfScalingByDepth;

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

void ApplyDepthParameters(::Effekseer::Matrix43& mat,
					  const ::Effekseer::Vector3D& cameraFront,
					  const ::Effekseer::Vector3D& cameraPos,
					  ::Effekseer::Vector3D& scaleValues,
					  float depthOffset,
					  bool isDepthOffsetScaledWithCamera,
					  bool isDepthOffsetScaledWithEffect,
						  ::Effekseer::NodeRendererDepthParameter* depthParameter,
						  bool isRightHand)
{
	if (depthOffset != 0)
	{
		auto offset = depthOffset;

		if (isDepthOffsetScaledWithEffect)
		{
			auto scale = (scaleValues.X + scaleValues.Y + scaleValues.Z) / 3.0f;

			offset *= scale;
		}

		if (isDepthOffsetScaledWithCamera)
		{
			auto cx = mat.Value[3][0] + cameraPos.X;
			auto cy = mat.Value[3][1] + cameraPos.Y;
			auto cz = mat.Value[3][2] + cameraPos.Z;
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

		auto objPos = ::Effekseer::Vector3D(mat.Value[3][0], mat.Value[3][1], mat.Value[3][2]);
		auto dir = cameraPos - objPos;
		Effekseer::Vector3D::Normal(dir, dir);

		if (isRightHand)
		{
			mat.Value[3][0] += dir.X * offset;
			mat.Value[3][1] += dir.Y * offset;
			mat.Value[3][2] += dir.Z * offset;
		}
		else
		{
			mat.Value[3][0] += dir.X * offset;
			mat.Value[3][1] += dir.Y * offset;
			mat.Value[3][2] += dir.Z * offset;
		}
	}

	if (depthParameter->SuppressionOfScalingByDepth < 1.0f)
	{
		auto cx = mat.Value[3][0] - cameraPos.X;
		auto cy = mat.Value[3][1] - cameraPos.Y;
		auto cz = mat.Value[3][2] - cameraPos.Z;
		auto cl = sqrt(cx * cx + cy * cy + cz * cz);

		if (cl != 0.0)
		{
			auto scale = cl / 32.0f * (1.0f - depthParameter->SuppressionOfScalingByDepth) + depthParameter->SuppressionOfScalingByDepth;

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

void ApplyDepthParameters(::Effekseer::Matrix44& mat,
						  const ::Effekseer::Vector3D& cameraFront,
						  const ::Effekseer::Vector3D& cameraPos,
						  float depthOffset,
						  bool isDepthOffsetScaledWithCamera,
						  bool isDepthOffsetScaledWithEffect,
						  ::Effekseer::NodeRendererDepthParameter* depthParameter,
						  bool isRightHand)
{
	if (depthOffset != 0)
	{
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
			auto cx = mat.Values[3][0] + cameraPos.X;
			auto cy = mat.Values[3][1] + cameraPos.Y;
			auto cz = mat.Values[3][2] + cameraPos.Z;
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

		auto objPos = ::Effekseer::Vector3D(mat.Values[3][0], mat.Values[3][1], mat.Values[3][2]);
		auto dir = cameraPos - objPos;
		Effekseer::Vector3D::Normal(dir, dir);

		if (isRightHand)
		{
			mat.Values[3][0] += dir.X * offset;
			mat.Values[3][1] += dir.Y * offset;
			mat.Values[3][2] += dir.Z * offset;
		}
		else
		{
			mat.Values[3][0] += dir.X * offset;
			mat.Values[3][1] += dir.Y * offset;
			mat.Values[3][2] += dir.Z * offset;
		}
	}

	if (depthParameter->SuppressionOfScalingByDepth < 1.0f)
	{
		auto cx = mat.Values[3][0] - cameraPos.X;
		auto cy = mat.Values[3][1] - cameraPos.Y;
		auto cz = mat.Values[3][2] - cameraPos.Z;
		auto cl = sqrt(cx * cx + cy * cy + cz * cz);

		if (cl != 0.0)
		{
			auto scale = cl / 32.0f * (1.0f - depthParameter->SuppressionOfScalingByDepth) + depthParameter->SuppressionOfScalingByDepth;

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
