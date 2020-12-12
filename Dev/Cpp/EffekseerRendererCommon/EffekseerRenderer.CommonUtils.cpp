

#include "EffekseerRenderer.CommonUtils.h"

namespace EffekseerRenderer
{

void CalcBillboard(::Effekseer::BillboardType billboardType,
				   Effekseer::Mat43f& dst,
				   ::Effekseer::Vec3f& s,
				   ::Effekseer::Vec3f& R,
				   ::Effekseer::Vec3f& F,
				   const ::Effekseer::Mat43f& src,
				   const ::Effekseer::Vec3f& frontDirection)
{
	auto frontDir = frontDirection;

	if (billboardType == ::Effekseer::BillboardType::Billboard || billboardType == ::Effekseer::BillboardType::RotatedBillboard ||
		billboardType == ::Effekseer::BillboardType::YAxisFixed)
	{
		::Effekseer::Mat43f r;
		::Effekseer::Vec3f t;
		src.GetSRT(s, r, t);

		::Effekseer::Vec3f U;

		if (billboardType == ::Effekseer::BillboardType::Billboard)
		{
			::Effekseer::Vec3f Up(0.0f, 1.0f, 0.0f);

			F = frontDir;
			R = ::Effekseer::Vec3f::Cross(Up, F).Normalize();
			U = ::Effekseer::Vec3f::Cross(F, R).Normalize();
		}
		else if (billboardType == ::Effekseer::BillboardType::RotatedBillboard)
		{
			::Effekseer::Vec3f Up(0.0f, 1.0f, 0.0f);

			F = frontDir;
			R = ::Effekseer::Vec3f::Cross(Up, F).Normalize();
			U = ::Effekseer::Vec3f::Cross(F, R).Normalize();

			float c_zx2 = Effekseer::Vec3f::Dot(r.Y, r.Y) - r.Y.GetZ() * r.Y.GetZ();
			float c_zx = sqrt(std::max(0.0f, c_zx2));
			float s_z = 0.0f;
			float c_z = 0.0f;

			if (fabsf(c_zx) > 0.001f)
			{
				s_z = r.Y.GetX() / c_zx;
				c_z = r.Y.GetY() / c_zx;
			}
			else
			{
				s_z = 0.0f;
				c_z = 1.0f;
			}

			::Effekseer::Vec3f r_temp = R;
			::Effekseer::Vec3f u_temp = U;

			R = r_temp * c_z + u_temp * s_z;
			U = u_temp * c_z - r_temp * s_z;
		}
		else if (billboardType == ::Effekseer::BillboardType::YAxisFixed)
		{
			U = ::Effekseer::Vec3f(r.X.GetY(), r.Y.GetY(), r.Z.GetY());
			F = frontDir;
			R = ::Effekseer::Vec3f::Cross(U, F).Normalize();
			F = ::Effekseer::Vec3f::Cross(R, U).Normalize();
		}

		dst.X = {R.GetX(), U.GetX(), F.GetX(), t.GetX()};
		dst.Y = {R.GetY(), U.GetY(), F.GetY(), t.GetY()};
		dst.Z = {R.GetZ(), U.GetZ(), F.GetZ(), t.GetZ()};
	}
}

void SplineGenerator::AddVertex(const Effekseer::Vec3f& v)
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

	b[0] = b[b.size() - 1] = ::Effekseer::Vec3f(0.0f, 0.0f, 0.0f);
	c[0] = c[c.size() - 1] = ::Effekseer::Vec3f(0.0f, 0.0f, 0.0f);
	w[0] = ::Effekseer::Vec3f(0.0f, 0.0f, 0.0f);

	for (size_t i = 1; i < a.size() - 1; i++)
	{
		c[i] = (a[i - 1] + a[i] * (-2.0) + a[i + 1]) * 3.0;
	}

	for (size_t i = 1; i < a.size() - 1; i++)
	{
		auto tmp = Effekseer::Vec3f(4.0, 4.0, 4.0) - w[i - 1];
		c[i] = (c[i] - c[i - 1]) / tmp;
		w[i] = Effekseer::Vec3f(1.0, 1.0, 1.0) / tmp;
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

Effekseer::Vec3f SplineGenerator::GetValue(float t) const
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

static void FastScale(::Effekseer::Mat43f& mat, float scale)
{
	float x = mat.X.GetW();
	float y = mat.Y.GetW();
	float z = mat.Z.GetW();

	mat.X *= scale;
	mat.Y *= scale;
	mat.Z *= scale;

	mat.X.SetW(x);
	mat.Y.SetW(y);
	mat.Z.SetW(z);
}

static void FastScale(::Effekseer::Mat44f& mat, float scale)
{
	float x = mat.X.GetW();
	float y = mat.Y.GetW();
	float z = mat.Z.GetW();

	mat.X *= scale;
	mat.Y *= scale;
	mat.Z *= scale;

	mat.X.SetW(x);
	mat.Y.SetW(y);
	mat.Z.SetW(z);
}

void ApplyDepthParameters(::Effekseer::Mat43f& mat,
						  const ::Effekseer::Vec3f& cameraFront,
						  const ::Effekseer::Vec3f& cameraPos,
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
			auto scales = mat.GetScale();
			auto scale = (scales.GetX() + scales.GetY() + scales.GetZ()) / 3.0f;

			offset *= scale;
		}

		if (isDepthOffsetScaledWithCamera)
		{
			auto t = mat.GetTranslation();
			auto c = t - cameraPos;
			auto cl = c.GetLength();

			if (cl != 0.0)
			{
				auto scale = (cl - offset) / cl;
				FastScale(mat, scale);
			}
		}

		auto objPos = mat.GetTranslation();
		auto dir = cameraPos - objPos;
		dir = dir.Normalize();

		if (isRightHand)
		{
			mat.SetTranslation(mat.GetTranslation() + dir * offset);
		}
		else
		{
			mat.SetTranslation(mat.GetTranslation() + dir * offset);
		}
	}

	if (depthParameter->SuppressionOfScalingByDepth < 1.0f)
	{
		auto t = mat.GetTranslation();
		auto c = t - cameraPos;
		auto cl = c.GetLength();
		// auto cl = cameraFront.X * cx + cameraFront.Y * cy * cameraFront.Z * cz;

		if (cl != 0.0)
		{
			auto scale = cl / 32.0f * (1.0f - depthParameter->SuppressionOfScalingByDepth) + depthParameter->SuppressionOfScalingByDepth;
			FastScale(mat, scale);
		}
	}
}

void ApplyDepthParameters(::Effekseer::Mat43f& mat,
						  ::Effekseer::Vec3f& translationValues,
						  ::Effekseer::Vec3f& scaleValues,
						  const ::Effekseer::Vec3f& cameraFront,
						  const ::Effekseer::Vec3f& cameraPos,
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
			auto scale = (scaleValues.GetX() + scaleValues.GetY() + scaleValues.GetZ()) / 3.0f;
			offset *= scale;
		}

		if (isDepthOffsetScaledWithCamera)
		{
			auto c = translationValues - cameraPos;
			auto cl = c.GetLength();

			if (cl != 0.0)
			{
				auto scale = (cl - offset) / cl;

				scaleValues *= scale;
			}
		}

		auto objPos = translationValues;
		auto dir = cameraPos - objPos;
		dir = dir.Normalize();

		if (isRightHand)
		{
			translationValues += dir * offset;
		}
		else
		{
			translationValues += dir * offset;
		}
	}

	if (depthParameter->SuppressionOfScalingByDepth < 1.0f)
	{
		auto cam2t = translationValues - cameraPos;
		auto cl = cam2t.GetLength();

		if (cl != 0.0)
		{
			auto scale = cl / 32.0f * (1.0f - depthParameter->SuppressionOfScalingByDepth) + depthParameter->SuppressionOfScalingByDepth;
			for (auto r = 0; r < 3; r++)
			{
				for (auto c = 0; c < 3; c++)
				{
					scaleValues *= scale;
				}
			}
		}
	}
}

void ApplyDepthParameters(::Effekseer::Mat43f& mat,
						  const ::Effekseer::Vec3f& cameraFront,
						  const ::Effekseer::Vec3f& cameraPos,
						  ::Effekseer::Vec3f& scaleValues,
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
			auto scale = (scaleValues.GetX() + scaleValues.GetY() + scaleValues.GetZ()) / 3.0f;
			offset *= scale;
		}

		if (isDepthOffsetScaledWithCamera)
		{
			auto t = mat.GetTranslation();
			auto c = t - cameraPos;
			auto cl = c.GetLength();

			if (cl != 0.0)
			{
				auto scale = (cl - offset) / cl;
				FastScale(mat, scale);
			}
		}

		auto objPos = mat.GetTranslation();
		auto dir = cameraPos - objPos;
		dir = dir.Normalize();

		if (isRightHand)
		{
			mat.SetTranslation(mat.GetTranslation() + dir * offset);
		}
		else
		{
			mat.SetTranslation(mat.GetTranslation() + dir * offset);
		}
	}

	if (depthParameter->SuppressionOfScalingByDepth < 1.0f)
	{
		auto t = mat.GetTranslation();
		auto c = t - cameraPos;
		auto cl = c.GetLength();

		if (cl != 0.0)
		{
			auto scale = cl / 32.0f * (1.0f - depthParameter->SuppressionOfScalingByDepth) + depthParameter->SuppressionOfScalingByDepth;
			FastScale(mat, scale);
		}
	}
}

void ApplyDepthParameters(::Effekseer::Mat44f& mat,
						  const ::Effekseer::Vec3f& cameraFront,
						  const ::Effekseer::Vec3f& cameraPos,
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
			auto scales = mat.GetScale();
			auto scale = (scales.GetX() + scales.GetY() + scales.GetZ()) / 3.0f;

			offset *= scale;
		}

		if (isDepthOffsetScaledWithCamera)
		{
			auto t = mat.GetTranslation();
			auto c = t - cameraPos;
			auto cl = c.GetLength();

			if (cl != 0.0)
			{
				auto scale = (cl - offset) / cl;
				FastScale(mat, scale);
			}
		}

		auto objPos = mat.GetTranslation();
		auto dir = cameraPos - objPos;
		dir = dir.Normalize();

		if (isRightHand)
		{
			mat.SetTranslation(mat.GetTranslation() + dir * offset);
		}
		else
		{
			mat.SetTranslation(mat.GetTranslation() + dir * offset);
		}
	}

	if (depthParameter->SuppressionOfScalingByDepth < 1.0f)
	{
		auto t = mat.GetTranslation();
		auto c = t - cameraPos;
		auto cl = c.GetLength();

		if (cl != 0.0)
		{
			auto scale = cl / 32.0f * (1.0f - depthParameter->SuppressionOfScalingByDepth) + depthParameter->SuppressionOfScalingByDepth;
			FastScale(mat, scale);
		}
	}
}

} // namespace EffekseerRenderer
