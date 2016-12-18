
#include "Culling3D.ObjectInternal.h"
#include "Culling3D.WorldInternal.h"

namespace Culling3D
{
	Object* Object::Create()
	{
		return new ObjectInternal();
	}

	ObjectInternal::ObjectInternal()
		: userData(NULL)
		, world(NULL)
		, ObjectIndex(-1)
	{
		currentStatus.Position = Vector3DF();
		currentStatus.radius = 0.0f;
		currentStatus.Type = OBJECT_SHAPE_TYPE_NONE;

		nextStatus.Position = Vector3DF();
		nextStatus.radius = 0.0f;
		nextStatus.Type = OBJECT_SHAPE_TYPE_NONE;
	}

	ObjectInternal::~ObjectInternal()
	{
	}

	Vector3DF ObjectInternal::GetPosition()
	{
		return nextStatus.Position;
	}

	void ObjectInternal::SetPosition(Vector3DF pos)
	{
		nextStatus.Position = pos;

		if (world != NULL)
		{
			WorldInternal* w = (WorldInternal*) world;
			w->RemoveObjectInternal(this);
			w->AddObjectInternal(this);
		}

		currentStatus = nextStatus;
	}

	void ObjectInternal::ChangeIntoAll()
	{
		nextStatus.Type = OBJECT_SHAPE_TYPE_ALL;
		nextStatus.CalcRadius();

		if (world != NULL)
		{
			WorldInternal* w = (WorldInternal*) world;
			w->RemoveObjectInternal(this);
			w->AddObjectInternal(this);
		}

		currentStatus = nextStatus;
	}

	void ObjectInternal::ChangeIntoSphere(float radius)
	{
		nextStatus.Data.Sphere.Radius = radius;
		nextStatus.Type = OBJECT_SHAPE_TYPE_SPHERE;
		nextStatus.CalcRadius();

		if (world != NULL)
		{
			WorldInternal* w = (WorldInternal*) world;
			w->RemoveObjectInternal(this);
			w->AddObjectInternal(this);
		}

		currentStatus = nextStatus;
	}

	void ObjectInternal::ChangeIntoCuboid(Vector3DF size)
	{
		nextStatus.Data.Cuboid.X = size.X;
		nextStatus.Data.Cuboid.Y = size.Y;
		nextStatus.Data.Cuboid.Z = size.Z;
		nextStatus.Type = OBJECT_SHAPE_TYPE_CUBOID;
		nextStatus.CalcRadius();

		if (world != NULL)
		{
			WorldInternal* w = (WorldInternal*) world;
			w->RemoveObjectInternal(this);
			w->AddObjectInternal(this);
		}

		currentStatus = nextStatus;
	}

	void* ObjectInternal::GetUserData()
	{
		return userData;
	}

	void ObjectInternal::SetUserData(void* userData)
	{
		this->userData = userData;
	}


	void ObjectInternal::SetWorld(World* world)
	{
		this->world = world;
	}
}