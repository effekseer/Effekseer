#pragma once

#include "../Effekseer.InternalStruct.h"

namespace Effekseer
{

enum class KillType : int32_t
{
	None = 0,
	Box = 1,
	Plane = 2,
	Sphere = 3
};

struct KillRulesParameter
{

	KillType Type = KillType::None;
	int IsScaleAndRotationApplied = 1;

	union
	{
		struct
		{
			vector3d Center; // In local space
			vector3d Size;	 // In local space
			int IsKillInside;
		} Box;

		struct
		{
			vector3d PlaneAxis; // in local space
			float PlaneOffset;	// in the direction of plane axis
		} Plane;

		struct
		{
			vector3d Center; // in local space
			float Radius;
			int IsKillInside;
		} Sphere;
	};

	void Load(unsigned char*& pos, int version)
	{
		if (version >= Version17Alpha5)
		{
			memcpy(&Type, pos, sizeof(int32_t));
			pos += sizeof(int32_t);

			memcpy(&IsScaleAndRotationApplied, pos, sizeof(int));
			pos += sizeof(int);

			if (Type == KillType::Box)
			{
				memcpy(&Box.Center, pos, sizeof(Vector3D));
				pos += sizeof(Vector3D);

				memcpy(&Box.Size, pos, sizeof(Vector3D));
				pos += sizeof(Vector3D);

				memcpy(&Box.IsKillInside, pos, sizeof(int));
				pos += sizeof(int);
			}
			else if (Type == KillType::Plane)
			{
				memcpy(&Plane.PlaneAxis, pos, sizeof(Vector3D));
				pos += sizeof(Vector3D);

				memcpy(&Plane.PlaneOffset, pos, sizeof(float));
				pos += sizeof(float);

				const auto length = Vector3D::Length(Vector3D{Plane.PlaneAxis.x, Plane.PlaneAxis.y, Plane.PlaneAxis.z});
				Plane.PlaneAxis.x /= length;
				Plane.PlaneAxis.y /= length;
				Plane.PlaneAxis.z /= length;
			}
			else if (Type == KillType::Sphere)
			{
				memcpy(&Sphere.Center, pos, sizeof(Vector3D));
				pos += sizeof(Vector3D);

				memcpy(&Sphere.Radius, pos, sizeof(float));
				pos += sizeof(float);

				memcpy(&Sphere.IsKillInside, pos, sizeof(int));
				pos += sizeof(int);
			}
		}
		else
		{
			Type = KillType::None;
			IsScaleAndRotationApplied = 1;
		}
	}

	void Magnify(float magnification)
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

	void MakeCoordinateSystemLH()
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
};

} // namespace Effekseer