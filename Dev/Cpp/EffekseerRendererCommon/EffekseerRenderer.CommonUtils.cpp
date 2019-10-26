

#include "EffekseerRenderer.CommonUtils.h"

namespace EffekseerRenderer
{

void CalcBillboard(::Effekseer::BillboardType billboardType,
				   Effekseer::Matrix43& dst,
				   ::Effekseer::Vector3D& s,
				   ::Effekseer::Vector3D& R,
				   ::Effekseer::Vector3D& F,
				   const ::Effekseer::Matrix43& src,
				   const ::Effekseer::Vector3D& frontDirection)
{
	auto frontDir = frontDirection;

	if (billboardType == ::Effekseer::BillboardType::Billboard || billboardType == ::Effekseer::BillboardType::RotatedBillboard ||
		billboardType == ::Effekseer::BillboardType::YAxisFixed)
	{
		::Effekseer::Matrix43 r;
		::Effekseer::Vector3D t;
		src.GetSRT(s, r, t);

		::Effekseer::Vector3D U;

		if (billboardType == ::Effekseer::BillboardType::Billboard)
		{
			::Effekseer::Vector3D Up(0.0f, 1.0f, 0.0f);

			::Effekseer::Vector3D::Normal(F, frontDir);
			::Effekseer::Vector3D::Normal(R, ::Effekseer::Vector3D::Cross(R, Up, F));
			::Effekseer::Vector3D::Normal(U, ::Effekseer::Vector3D::Cross(U, F, R));
		}
		else if (billboardType == ::Effekseer::BillboardType::RotatedBillboard)
		{
			::Effekseer::Vector3D Up(0.0f, 1.0f, 0.0f);

			::Effekseer::Vector3D::Normal(F, frontDir);

			::Effekseer::Vector3D::Normal(R, ::Effekseer::Vector3D::Cross(R, Up, F));
			::Effekseer::Vector3D::Normal(U, ::Effekseer::Vector3D::Cross(U, F, R));

			float c_zx = sqrt(1.0f - r.Value[2][1] * r.Value[2][1]);
			float s_z = 0.0f;
			float c_z = 0.0f;

			if (fabsf(c_zx) > 0.05f)
			{
				s_z = -r.Value[0][1] / c_zx;
				c_z = sqrt(1.0f - s_z * s_z);
				if (r.Value[1][1] < 0.0f)
					c_z = -c_z;
			}
			else
			{
				s_z = 0.0f;
				c_z = 1.0f;
			}

			::Effekseer::Vector3D r_temp = R;
			::Effekseer::Vector3D u_temp = U;

			R.X = r_temp.X * c_z + u_temp.X * s_z;
			R.Y = r_temp.Y * c_z + u_temp.Y * s_z;
			R.Z = r_temp.Z * c_z + u_temp.Z * s_z;

			U.X = u_temp.X * c_z - r_temp.X * s_z;
			U.Y = u_temp.Y * c_z - r_temp.Y * s_z;
			U.Z = u_temp.Z * c_z - r_temp.Z * s_z;
		}
		else if (billboardType == ::Effekseer::BillboardType::YAxisFixed)
		{
			U = ::Effekseer::Vector3D(r.Value[1][0], r.Value[1][1], r.Value[1][2]);

			::Effekseer::Vector3D::Normal(F, frontDir);

			::Effekseer::Vector3D::Normal(R, ::Effekseer::Vector3D::Cross(R, U, F));
			::Effekseer::Vector3D::Normal(F, ::Effekseer::Vector3D::Cross(F, R, U));
		}

		dst.Value[0][0] = R.X;
		dst.Value[0][1] = R.Y;
		dst.Value[0][2] = R.Z;
		dst.Value[1][0] = U.X;
		dst.Value[1][1] = U.Y;
		dst.Value[1][2] = U.Z;
		dst.Value[2][0] = F.X;
		dst.Value[2][1] = F.Y;
		dst.Value[2][2] = F.Z;
		dst.Value[3][0] = t.X;
		dst.Value[3][1] = t.Y;
		dst.Value[3][2] = t.Z;
	}
}

void SplineGenerator::AddVertex(const Effekseer::Vector3D& v)
{
	a.push_back(v);
	if (a.size() >= 2)
	{
		isSame.push_back(a[a.size() - 1] == a[a.size() - 2]);
	}
}

void SplineGenerator::Calculate()
{
	b.resize(a.size());
	c.resize(a.size());
	d.resize(a.size());
	w.resize(a.size());

	for (size_t i = 1; i < a.size() - 1; i++)
	{
		c[i] = (a[i - 1] + a[i] * (-2.0) + a[i + 1]) * 3.0;
	}

	for (size_t i = 1; i < a.size() - 1; i++)
	{
		auto tmp = Effekseer::Vector3D(4.0, 4.0, 4.0) - w[i - 1];
		c[i] = (c[i] - c[i - 1]) / tmp;
		w[i] = Effekseer::Vector3D(1.0, 1.0, 1.0) / tmp;
	}

	for (size_t i = (a.size() - 1) - 1; i > 0; i--)
	{
		c[i] = c[i] - c[i + 1] * w[i];
	}

	for (size_t i = 0; i < a.size() - 1; i++)
	{
		d[i] = (c[i + 1] - c[i]) / 3.0;
		b[i] = a[i + 1] - a[i] - c[i] - d[i];
	}
}

void SplineGenerator::Reset()
{
	a.clear();
	b.clear();
	c.clear();
	d.clear();
	w.clear();
	isSame.clear();
}

Effekseer::Vector3D SplineGenerator::GetValue(float t) const
{
	int32_t j = (int32_t)floorf(t);

	if (j < 0)
	{
		j = 0;
	}

	if (j > (int32_t)a.size())
	{
		j = (int32_t)a.size() - 1;
	}

	auto dt = t - j;

	if (j < (int32_t)isSame.size() && isSame[j])
		return a[j];

	return a[j] + (b[j] + (c[j] + d[j] * dt) * dt) * dt;
}

void ApplyDepthParameters(::Effekseer::Matrix43& mat,
	const ::Effekseer::Vector3D& cameraFront,
	const ::Effekseer::Vector3D& cameraPos,
	::Effekseer::NodeRendererDepthParameter* depthParameter,
	bool isRightHand)
{
	auto depthOffset = depthParameter->DepthOffset;
	auto isDepthOffsetScaledWithCamera = depthParameter->IsDepthOffsetScaledWithCamera;
	auto isDepthOffsetScaledWithEffect = depthParameter->IsDepthOffsetScaledWithParticleScale;

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
			auto cx = mat.Value[3][0] - cameraPos.X;
			auto cy = mat.Value[3][1] - cameraPos.Y;
			auto cz = mat.Value[3][2] - cameraPos.Z;
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
						  ::Effekseer::Vector3D& translationValues,
						  ::Effekseer::Vector3D& scaleValues,
						  const ::Effekseer::Vector3D& cameraFront,
						  const ::Effekseer::Vector3D& cameraPos,
						  ::Effekseer::NodeRendererDepthParameter* depthParameter,
						  bool isRightHand)
{
	auto depthOffset = depthParameter->DepthOffset;
	auto isDepthOffsetScaledWithCamera = depthParameter->IsDepthOffsetScaledWithCamera;
	auto isDepthOffsetScaledWithEffect = depthParameter->IsDepthOffsetScaledWithParticleScale;

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
			auto cx = translationValues.X - cameraPos.X;
			auto cy = translationValues.Y - cameraPos.Y;
			auto cz = translationValues.Z - cameraPos.Z;
			auto cl = sqrt(cx * cx + cy * cy + cz * cz);

			if (cl != 0.0)
			{
				auto scale = (cl - offset) / cl;

				scaleValues.X *= scale;
				scaleValues.Y *= scale;
				scaleValues.Z *= scale;
			}
		}

		auto objPos = translationValues;
		auto dir = cameraPos - objPos;
		Effekseer::Vector3D::Normal(dir, dir);

		if (isRightHand)
		{
			translationValues.X += dir.X * offset;
			translationValues.Y += dir.Y * offset;
			translationValues.Z += dir.Z * offset;
		}
		else
		{
			translationValues.X += dir.X * offset;
			translationValues.Y += dir.Y * offset;
			translationValues.Z += dir.Z * offset;
		}
	}

	if (depthParameter->SuppressionOfScalingByDepth < 1.0f)
	{
		auto cx = translationValues.X - cameraPos.X;
		auto cy = translationValues.Y - cameraPos.Y;
		auto cz = translationValues.Z - cameraPos.Z;
		auto cl = sqrt(cx * cx + cy * cy + cz * cz);

		if (cl != 0.0)
		{
			auto scale = cl / 32.0f * (1.0f - depthParameter->SuppressionOfScalingByDepth) + depthParameter->SuppressionOfScalingByDepth;

			for (auto r = 0; r < 3; r++)
			{
				for (auto c = 0; c < 3; c++)
				{
					scaleValues.X *= scale;
					scaleValues.Y *= scale;
					scaleValues.Z *= scale;
				}
			}
		}
	}
}

void ApplyDepthParameters(::Effekseer::Matrix43& mat,
					  const ::Effekseer::Vector3D& cameraFront,
					  const ::Effekseer::Vector3D& cameraPos,
					  ::Effekseer::Vector3D& scaleValues,
						  ::Effekseer::NodeRendererDepthParameter* depthParameter,
						  bool isRightHand)
{
	auto depthOffset = depthParameter->DepthOffset;
	auto isDepthOffsetScaledWithCamera = depthParameter->IsDepthOffsetScaledWithCamera;
	auto isDepthOffsetScaledWithEffect = depthParameter->IsDepthOffsetScaledWithParticleScale;

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
			auto cx = mat.Value[3][0] - cameraPos.X;
			auto cy = mat.Value[3][1] - cameraPos.Y;
			auto cz = mat.Value[3][2] - cameraPos.Z;
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
						  ::Effekseer::NodeRendererDepthParameter* depthParameter,
						  bool isRightHand)
{
	auto depthOffset = depthParameter->DepthOffset;
	auto isDepthOffsetScaledWithCamera = depthParameter->IsDepthOffsetScaledWithCamera;
	auto isDepthOffsetScaledWithEffect = depthParameter->IsDepthOffsetScaledWithParticleScale;

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
			auto cx = mat.Values[3][0] - cameraPos.X;
			auto cy = mat.Values[3][1] - cameraPos.Y;
			auto cz = mat.Values[3][2] - cameraPos.Z;
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

} // namespace EffekseerRenderer
