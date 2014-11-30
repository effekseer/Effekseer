
#pragma once

#include "Culling3D.h"
#include "Culling3D.Grid.h"

#include <vector>

namespace Culling3D
{
	class Layer
	{
	private:
		int32_t		gridXCount;
		int32_t		gridYCount;
		int32_t		gridZCount;

		float		offsetX;
		float		offsetY;
		float		offsetZ;


		float		gridSize;
		std::vector<Grid>	grids;

	public:
		Layer(int32_t gridXCount, int32_t gridYCount, int32_t gridZCount, float offsetX, float offsetY, float offsetZ, float gridSize);
		virtual ~Layer();

		bool AddObject(Object* o);

		bool RemoveObject(Object* o);

		void AddGrids(Vector3DF max_, Vector3DF min_, std::vector<Grid*>& grids_);
	};
}