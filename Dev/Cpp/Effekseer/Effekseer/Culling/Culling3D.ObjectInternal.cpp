
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
		currentStatus.Radius = 0.0f;
		currentStatus.Type = OBJECT_SHAPE_TYPE_NONE;
		currentStatus.CuboidSize = Vector3DF();

		nextStatus.Position = Vector3DF();
		nextStatus.Radius = 0.0f;
		nextStatus.Type = OBJECT_SHAPE_TYPE_NONE;
		nextStatus.CuboidSize = Vector3DF();
	}

	ObjectInternal::~ObjectInternal()
	{
	}

	void ObjectInternal::SetShapeType(eObjectShapeType type)
	{
		nextStatus.Type = type;

		if (world != NULL)
		{
			WorldInternal* w = (WorldInternal*) world;
			w->RemoveObjectInternal(this);
			w->AddObjectInternal(this);
		}

		currentStatus = nextStatus;
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

	void ObjectInternal::SetRadius(float radius)
	{
		nextStatus.Radius = radius;

		if (world != NULL)
		{
			WorldInternal* w = (WorldInternal*) world;
			w->RemoveObjectInternal(this);
			w->AddObjectInternal(this);
		}

		currentStatus = nextStatus;
	}

	void ObjectInternal::SetCuboidSize(Vector3DF size)
	{
		nextStatus.CuboidSize = size;

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