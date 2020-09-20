#ifndef __EFFEKSEER_PROCEDUAL_MODEL_PARAMETER_H__
#define __EFFEKSEER_PROCEDUAL_MODEL_PARAMETER_H__

#include "../Utils/Effekseer.BinaryReader.h"
#include <stdint.h>
#include <stdio.h>

namespace Effekseer
{

enum class ProcedualModelType : int32_t
{
	Mesh,
	Ribbon,
};

enum class ProcedualModelPrimitiveType : int32_t
{
	Sphere,
	Cone,
	Cylinder,
};

struct ProcedualModelParameter
{
	ProcedualModelType Type;
	ProcedualModelPrimitiveType PrimitiveType;

	union
	{
		struct
		{
			float AngleBegin;
			float AngleEnd;
			int AngleDivision;
			int DepthDivision;
		} Mesh;

		struct
		{
			float Rotate;
			int Vertices;
			int Count;
		} Ribbon;
	};

	// float TwistPower;
	// float TwistUpPower;
	// hoge,,,

	union
	{
		struct
		{
			float Radius;
			float DepthMin;
			float DepthMax;
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
			return static_cast<int32_t>(Type) < static_cast<int32_t>(rhs.Type);
		}

		if (Type == ProcedualModelType::Mesh)
		{
			if (Mesh.AngleBegin != rhs.Mesh.AngleBegin)
				return Mesh.AngleBegin < rhs.Mesh.AngleBegin;

			if (Mesh.AngleEnd != rhs.Mesh.AngleEnd)
				return Mesh.AngleEnd < rhs.Mesh.AngleEnd;

			if (Mesh.AngleDivision != rhs.Mesh.AngleDivision)
				return Mesh.AngleDivision < rhs.Mesh.AngleDivision;

			if (Mesh.DepthDivision != rhs.Mesh.DepthDivision)
				return Mesh.DepthDivision < rhs.Mesh.DepthDivision;
		}
		else if (Type == ProcedualModelType::Mesh)
		{
			if (Ribbon.Rotate != rhs.Ribbon.Rotate)
				return Ribbon.Rotate < rhs.Ribbon.Rotate;

			if (Ribbon.Vertices != rhs.Ribbon.Vertices)
				return Ribbon.Vertices < rhs.Ribbon.Vertices;

			if (Ribbon.Count != rhs.Ribbon.Count)
				return Ribbon.Count < rhs.Ribbon.Count;
		}
		else
		{
			assert(0);
		}

		if (PrimitiveType != rhs.PrimitiveType)
		{
			return static_cast<int32_t>(PrimitiveType) < static_cast<int32_t>(rhs.PrimitiveType);
		}

		if (PrimitiveType == ProcedualModelPrimitiveType::Sphere)
		{
			if (Sphere.Radius != rhs.Sphere.Radius)
				return Sphere.Radius < rhs.Sphere.Radius;

			if (Sphere.DepthMin != rhs.Sphere.DepthMin)
				return Sphere.DepthMax < rhs.Sphere.DepthMax;
		}
		else if (PrimitiveType == ProcedualModelPrimitiveType::Cone)
		{
			if (Cone.Radius != rhs.Cone.Radius)
				return Cone.Radius < rhs.Cone.Radius;

			if (Cone.Depth != rhs.Cone.Depth)
				return Cone.Depth < rhs.Cone.Depth;
		}
		else if (PrimitiveType == ProcedualModelPrimitiveType::Cylinder)
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

		if (Type == ProcedualModelType::Mesh)
		{
			reader.Read(Mesh.AngleBegin);
			reader.Read(Mesh.AngleEnd);
			reader.Read(Mesh.AngleDivision);
			reader.Read(Mesh.DepthDivision);
		}
		else if (Type == ProcedualModelType::Ribbon)
		{
			reader.Read(Ribbon.Rotate);
			reader.Read(Ribbon.Vertices);
			reader.Read(Ribbon.Count);
		}
		else
		{
			assert(0);
		}

		reader.Read(PrimitiveType);

		if (PrimitiveType == ProcedualModelPrimitiveType::Sphere)
		{
			reader.Read(Sphere.Radius);
			reader.Read(Sphere.DepthMin);
			reader.Read(Sphere.DepthMax);
		}
		else if (PrimitiveType == ProcedualModelPrimitiveType::Cone)
		{
			reader.Read(Cone.Radius);
			reader.Read(Cone.Depth);
		}
		else if (PrimitiveType == ProcedualModelPrimitiveType::Cylinder)
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