#pragma once

#include "../Effekseer.InstanceGlobal.h"
#include "../Effekseer.InternalStruct.h"
#include "../SIMD/Vec3f.h"
#include "../Utils/BinaryVersion.h"
#include <optional>
#include <variant>

namespace Effekseer
{

enum class KillType : int32_t
{
	None = 0,
	Box = 1,
	Plane = 2,
	Sphere = 3
};

struct SphereShape
{
	Vector3D Center;
	float Radius;
};

struct BoxShape
{
	Vector3D Center;
	Vector3D Size;
};

struct KillRulesParameter
{
	KillType Type = KillType::None;
	int IsScaleAndRotationApplied = 1;

	std::optional<std::variant<SphereShape, BoxShape>> shape_;

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

	void Load(unsigned char*& pos, int version);

	void Magnify(float magnification);

	void MakeCoordinateSystemLH();

	static bool CheckRemoved(const KillRulesParameter& param, const InstanceGlobal& instance_global, const SIMD::Vec3f& prev_global_position);
};

} // namespace Effekseer