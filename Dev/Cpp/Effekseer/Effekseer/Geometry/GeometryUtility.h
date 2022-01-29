#pragma once

#include "../Effekseer.Matrix44.h"
#include "../Effekseer.Vector3D.h"
#include "../SIMD/Mat44f.h"
#include "../SIMD/Utils.h"
#include "../SIMD/Vec3f.h"

namespace Effekseer
{

struct Plane
{
	Vector3D Normal;
	float Distance;
};

struct Sphare
{
	Vector3D Center;
	float Radius;
};

class GeometryUtility
{
public:
	static bool IsContain(const std::array<Plane, 6>& planes, const Sphare& obj)
	{
		for (const auto& plane : planes)
		{
			const auto distance = Vector3D::Dot(plane.Normal, obj.Center);
			if (distance > plane.Distance + obj.Radius)
			{
				return false;
			}
		}

		return true;
	}

	static std::array<Plane, 6> CalculateFrustumPlanes(const Matrix44& viewProjectionMatrix, float projectedNear, float projectedFar, CoordinateSystem coordinateSystem)
	{
		if (projectedNear == projectedFar)
		{
			return std::array<Plane, 6>{};
		}

		std::array<SIMD::Vec4f, 8> points4;

		points4[0] = SIMD::Float4{-1.0f, -1.0f, projectedNear, 1.0f};
		points4[1] = SIMD::Float4{+1.0f, -1.0f, projectedNear, 1.0f};
		points4[2] = SIMD::Float4{-1.0f, +1.0f, projectedNear, 1.0f};
		points4[3] = SIMD::Float4{+1.0f, +1.0f, projectedNear, 1.0f};
		points4[4] = SIMD::Float4{-1.0f, -1.0f, projectedFar, 1.0f};
		points4[5] = SIMD::Float4{+1.0f, -1.0f, projectedFar, 1.0f};
		points4[6] = SIMD::Float4{-1.0f, +1.0f, projectedFar, 1.0f};
		points4[7] = SIMD::Float4{+1.0f, +1.0f, projectedFar, 1.0f};

		Matrix44 vpmInv;
		Matrix44::Inverse(vpmInv, viewProjectionMatrix);
		SIMD::Mat44f vpmInvSimd{vpmInv};

		for (auto& p : points4)
		{
			p = SIMD::Vec4f::Transform(p, vpmInvSimd);
			p = p / SIMD::Float4{
						p.GetW(),
						p.GetW(),
						p.GetW(),
						p.GetW()};
		}

		std::array<SIMD::Vec3f, 8> points3;

		for (size_t i = 0; i < points4.size(); i++)
		{
			points3[i] = {
				points4[i].GetX(),
				points4[i].GetY(),
				points4[i].GetZ(),
			};
		}

		std::array<std::array<int, 3>, 6> planeComponents;

		planeComponents[0] = {5, 1, 0};
		planeComponents[1] = {2, 3, 7};
		planeComponents[2] = {1, 3, 2};
		planeComponents[3] = {4, 6, 7};
		planeComponents[4] = {0, 2, 6};
		planeComponents[5] = {7, 3, 1};

		if (coordinateSystem == CoordinateSystem::LH)
		{
			for (size_t i = 0; i < planeComponents.size(); i++)
			{
				std::swap(planeComponents[i][0], planeComponents[i][2]);
			}
		}

		std::array<Plane, 6> ret;

		for (size_t i = 0; i < ret.size(); i++)
		{
			const auto component = planeComponents[i];
			const auto normal = SIMD::Vec3f::Cross(points3[component[1]] - points3[component[0]], points3[component[2]] - points3[component[0]]).Normalize();
			const auto distance = SIMD::Vec3f::Dot(points3[component[0]], normal);

			ret[i].Normal = SIMD::ToStruct(normal);
			ret[i].Distance = distance;
		}

		return ret;
	}
};

} // namespace Effekseer