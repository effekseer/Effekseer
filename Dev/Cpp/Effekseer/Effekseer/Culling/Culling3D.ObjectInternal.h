
#pragma once

#include "Culling3D.h"
#include "Culling3D.ReferenceObject.h"

namespace Culling3D
{
	class ObjectInternal
		: public Object
		, public ReferenceObject
	{
	public:
		struct Status
		{
			Vector3DF	Position;
			float		Radius;
			Vector3DF	CuboidSize;
			eObjectShapeType	Type;

			float CalcRadius()
			{
				if (Type == OBJECT_SHAPE_TYPE_NONE) return 0;
				if (Type == OBJECT_SHAPE_TYPE_SPHERE) return Radius;
				if (Type == OBJECT_SHAPE_TYPE_CUBOID) return sqrt(CuboidSize.X * CuboidSize.X + CuboidSize.Y * CuboidSize.Y + CuboidSize.Z * CuboidSize.Z) / 2.0f;
				return 0.0f;
			}
		};

	private:
		void*		userData;
		World*		world;

		Status	currentStatus;
		Status	nextStatus;

	public:
		ObjectInternal();
		virtual ~ObjectInternal();

		void SetShapeType(eObjectShapeType type);

		Vector3DF GetPosition();
		void SetPosition(Vector3DF pos);

		void SetRadius(float radius);

		void SetCuboidSize(Vector3DF size);

		void* GetUserData();
		void SetUserData(void* userData);

		void SetWorld(World* world);

		Status	GetCurrentStatus() { return currentStatus; }
		Status	GetNextStatus() { return nextStatus; }

		int32_t ObjectIndex;

		virtual int32_t GetRef() { return ReferenceObject::GetRef(); }
		virtual int32_t AddRef() { return ReferenceObject::AddRef(); }
		virtual int32_t Release() { return ReferenceObject::Release(); }
	};
}