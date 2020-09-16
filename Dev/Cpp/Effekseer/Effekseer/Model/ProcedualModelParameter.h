#ifndef __EFFEKSEER_PROCEDUAL_MODEL_PARAMETER_H__
#define __EFFEKSEER_PROCEDUAL_MODEL_PARAMETER_H__

#include "../Utils/Effekseer.BinaryReader.h"
#include <stdint.h>
#include <stdio.h>

namespace Effekseer
{

enum class ProcedualModelType : int32_t
{
	Sphere,
	Cone,
	Cylinder,
};

struct ProcedualModelParameter
{
	ProcedualModelType Type;

	float AngleBegin;
	float AngleEnd;
	float DepthMin;
	float DepthMax;
	int AxisDivision;
	int AngleDivision;

	float TwistPower;
	float TwistUpPower;
	// hoge,,,

	union
	{
		struct
		{
			float Radius;
		} Sphere;

		struct
		{
			float Radius;
			float Depth;
		} Cone;

		struct
		{
			float Radius1;
			float Radius2;
			float Depth;
		} Cylinder;
	};

	bool operator<(const ProcedualModelParameter& rhs) const
	{
		if (Type != rhs.Type)
		{
			return static_cast<int32_t>(Type) < static_cast<int32_t>(Type);
		}

		if (Type == ProcedualModelType::Sphere)
		{
			if (Sphere.Radius != rhs.Sphere.Radius)
				return Sphere.Radius < rhs.Sphere.Radius;
		}
		else if (Type == ProcedualModelType::Cone)
		{
			if (Cone.Radius != rhs.Cone.Radius)
				return Cone.Radius < rhs.Cone.Radius;

			if (Cone.Depth != rhs.Cone.Depth)
				return Cone.Depth < rhs.Cone.Depth;
		}
		else if (Type == ProcedualModelType::Cylinder)
		{
			if (Cylinder.Radius1 != rhs.Cylinder.Radius1)
				return Cylinder.Radius1 < rhs.Cylinder.Radius1;

			if (Cylinder.Radius2 != rhs.Cylinder.Radius2)
				return Cylinder.Radius2 < rhs.Cylinder.Radius2;

			if (Cylinder.Depth != rhs.Cylinder.Depth)
				return Cylinder.Depth < rhs.Cylinder.Depth;
		}
		else
		{
			assert(0);
		}

		return false;
	}

	template <bool T>
	bool Load(BinaryReader<T>& reader)
	{
		reader.Read(Type);
		reader.Read(AngleBegin);
		reader.Read(AngleEnd);
		reader.Read(DepthMin);
		reader.Read(DepthMax);
		reader.Read(AxisDivision);
		reader.Read(AngleDivision);

		AngleBegin = AngleBegin / 180.0f * EFK_PI;
		AngleEnd = AngleEnd / 180.0f * EFK_PI;

		if (Type == ProcedualModelType::Sphere)
		{
			reader.Read(Sphere.Radius);
		}
		else if (Type == ProcedualModelType::Cone)
		{
			reader.Read(Cone.Radius);
			reader.Read(Cone.Depth);
		}
		else if (Type == ProcedualModelType::Cylinder)
		{
			reader.Read(Cylinder.Radius1);
			reader.Read(Cylinder.Radius2);
			reader.Read(Cylinder.Depth);
		}
		return true;
	}
};

} // namespace Effekseer

#endif