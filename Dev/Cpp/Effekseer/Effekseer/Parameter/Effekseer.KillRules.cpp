#include "../Utils/Effekseer.BinaryReader.h"
#include "Effekseer.KillRules.h"
#include "../SIMD/Mat44f.h"

namespace Effekseer
{

void KillRulesParameter::Load(BinaryReader<true>& pos, int version)
{
	if (version >= Version17Alpha5)
	{
		if (!pos.Peek(&Type, sizeof(int32_t)))
		{
			return pos.MarkFailed();
		}
		pos.Skip(sizeof(int32_t));

		if (!pos.Peek(&IsScaleAndRotationApplied, sizeof(int)))
		{
			return pos.MarkFailed();
		}
		pos.Skip(sizeof(int));

		if (Type == KillType::Box)
		{
			if (!pos.Peek(&Box.Center, sizeof(Vector3D)))
			{
				return pos.MarkFailed();
			}
			pos.Skip(sizeof(Vector3D));

			if (!pos.Peek(&Box.Size, sizeof(Vector3D)))
			{
				return pos.MarkFailed();
			}
			pos.Skip(sizeof(Vector3D));

			if (!pos.Peek(&Box.IsKillInside, sizeof(int)))
			{
				return pos.MarkFailed();
			}
			pos.Skip(sizeof(int));
		}
		else if (Type == KillType::Plane)
		{
			if (!pos.Peek(&Plane.PlaneAxis, sizeof(Vector3D)))
			{
				return pos.MarkFailed();
			}
			pos.Skip(sizeof(Vector3D));

			if (!pos.Peek(&Plane.PlaneOffset, sizeof(float)))
			{
				return pos.MarkFailed();
			}
			pos.Skip(sizeof(float));

			const auto length = Vector3D::Length(Vector3D{Plane.PlaneAxis.x, Plane.PlaneAxis.y, Plane.PlaneAxis.z});
			Plane.PlaneAxis.x /= length;
			Plane.PlaneAxis.y /= length;
			Plane.PlaneAxis.z /= length;
		}
		else if (Type == KillType::Sphere)
		{
			if (!pos.Peek(&Sphere.Center, sizeof(Vector3D)))
			{
				return pos.MarkFailed();
			}
			pos.Skip(sizeof(Vector3D));

			if (!pos.Peek(&Sphere.Radius, sizeof(float)))
			{
				return pos.MarkFailed();
			}
			pos.Skip(sizeof(float));

			if (!pos.Peek(&Sphere.IsKillInside, sizeof(int)))
			{
				return pos.MarkFailed();
			}
			pos.Skip(sizeof(int));
		}
	}
	else
	{
		Type = KillType::None;
		IsScaleAndRotationApplied = 1;
	}
}

void KillRulesParameter::Magnify(float magnification)
{
	if (Type == KillType::Box)
	{
		Box.Center *= magnification;
		Box.Size *= magnification;
	}
	else if (Type == KillType::Plane)
	{
		Plane.PlaneOffset *= magnification;
	}
	else if (Type == KillType::Sphere)
	{
		Sphere.Center *= magnification;
		Sphere.Radius *= magnification;
	}
}

void KillRulesParameter::MakeCoordinateSystemLH()
{
	if (Type == KillType::Box)
	{
		Box.Center.z *= -1.0F;
	}
	else if (Type == KillType::Plane)
	{
		Plane.PlaneAxis.z *= -1.0F;
	}
	else if (Type == KillType::Sphere)
	{
		Sphere.Center.z *= -1.0F;
	}
}

bool KillRulesParameter::CheckRemoved(const KillRulesParameter& param, const InstanceGlobal& instance_global, const SIMD::Vec3f& prev_global_position)
{
	if (param.Type == KillType::None)
	{
		return false;
	}

	SIMD::Vec3f localPosition{};
	if (param.IsScaleAndRotationApplied)
	{
		SIMD::Mat44f invertedGlobalMatrix = instance_global.InvertedEffectGlobalMatrix;
		localPosition = SIMD::Vec3f::Transform(prev_global_position, invertedGlobalMatrix);
	}
	else
	{
		SIMD::Mat44f globalMatrix = instance_global.EffectGlobalMatrix;
		localPosition = prev_global_position - globalMatrix.GetTranslation();
	}

	if (param.Type == KillType::Box)
	{
		localPosition = localPosition - param.Box.Center;
		localPosition = SIMD::Vec3f::Abs(localPosition);
		SIMD::Vec3f size = param.Box.Size;
		bool isWithin = localPosition.GetX() <= size.GetX() && localPosition.GetY() <= size.GetY() && localPosition.GetZ() <= size.GetZ();

		if (isWithin && param.Box.IsKillInside > 0)
		{
			return true;
		}
		else if (!isWithin && param.Box.IsKillInside == 0)
		{
			return true;
		}
	}
	else if (param.Type == KillType::Plane)
	{
		SIMD::Vec3f planeNormal = param.Plane.PlaneAxis;
		SIMD::Vec3f planePosition = planeNormal * param.Plane.PlaneOffset;
		float planeW = -SIMD::Vec3f::Dot(planePosition, planeNormal);
		float factor = SIMD::Vec3f::Dot(localPosition, planeNormal) + planeW;
		if (factor > 0.0F)
		{
			return true;
		}
	}
	else if (param.Type == KillType::Sphere)
	{
		SIMD::Vec3f delta = localPosition - param.Sphere.Center;
		float distance = delta.GetSquaredLength();
		float radius = param.Sphere.Radius;
		bool isWithin = distance <= (radius * radius);
		if (isWithin && param.Sphere.IsKillInside > 0)
		{
			return true;
		}
		else if (!isWithin && param.Sphere.IsKillInside == 0)
		{
			return true;
		}
	}

	return false;
}

} // namespace Effekseer
