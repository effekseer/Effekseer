
#pragma once

#include "Culling3D.h"
#include "Culling3D.ReferenceObject.h"

#include "Culling3D.Layer.h"
#include "Culling3D.Grid.h"

#include <set>

namespace Culling3D
{
	class WorldInternal
		: public World
		, public ReferenceObject
	{
	private:
		float xSize;
		float ySize;
		float zSize;

		float	gridSize;
		float	minGridSize;
		int32_t	layerCount;

		std::vector<Layer*>	layers;

		Grid	outofLayers;
		Grid	allLayers;

		std::vector<Object*> objs;

		std::vector<Grid*> grids;

		std::set<Object*>	containedObjects;

	public:
		WorldInternal(float xSize, float ySize, float zSize, int32_t layerCount);
		virtual ~WorldInternal();

		void AddObject(Object* o);
		void RemoveObject(Object* o);

		void AddObjectInternal(Object* o);
		void RemoveObjectInternal(Object* o);

		void CastRay(Vector3DF from, Vector3DF to) override;

		void Culling(const Matrix44& cameraProjMat, bool isOpenGL);

		bool Reassign() override;

		void Dump(const char* path, const Matrix44& cameraProjMat, bool isOpenGL);

		int32_t GetObjectCount() { return objs.size(); }
		Object* GetObject(int32_t index) { return objs[index]; }

		virtual int32_t GetRef() { return ReferenceObject::GetRef(); }
		virtual int32_t AddRef() { return ReferenceObject::AddRef(); }
		virtual int32_t Release() { return ReferenceObject::Release(); }
	};
}