
#include "Culling3D.WorldInternal.h"
#include "Culling3D.ObjectInternal.h"

namespace Culling3D
{
	World* World::Create(float xSize, float ySize, float zSize, int32_t layerCount)
	{
		return new WorldInternal(xSize, ySize, zSize, layerCount);
	}

	WorldInternal::WorldInternal(float xSize, float ySize, float zSize, int32_t layerCount)
	{
		this->xSize = xSize;
		this->ySize = ySize;
		this->zSize = zSize;

		this->gridSize = Max(Max(this->xSize, this->ySize), this->zSize);
		
		this->layerCount = layerCount;

		layers.resize(this->layerCount);

		for (size_t i = 0; i < layers.size(); i++)
		{
			float gridSize_ = this->gridSize / powf(2.0f, (float)i);

			int32_t xCount = (int32_t) (this->xSize / gridSize_);
			int32_t yCount = (int32_t) (this->ySize / gridSize_);
			int32_t zCount = (int32_t) (this->zSize / gridSize_);

			if (xCount * gridSize_ < this->xSize) xCount++;
			if (yCount * gridSize_ < this->ySize) yCount++;
			if (zCount * gridSize_ < this->zSize) zCount++;

			layers[i] = new Layer(xCount, yCount, zCount, xSize / 2.0f, ySize / 2.0f, zSize / 2.0f, gridSize_);
			
			this->minGridSize = gridSize_;
		}
	}

	WorldInternal::~WorldInternal()
	{
		for (size_t i = 0; i < layers.size(); i++)
		{
			delete layers[i];
		}

		layers.clear();

		for (std::set<Object*>::iterator it = containedObjects.begin(); it != containedObjects.end(); it++)
		{
			(*it)->Release();
		}
	}

	void WorldInternal::AddObject(Object* o)
	{
		SafeAddRef(o);
		containedObjects.insert(o);
		AddObjectInternal(o);
	}

	void WorldInternal::RemoveObject(Object* o)
	{
		RemoveObjectInternal(o);
		containedObjects.erase(o);
		SafeRelease(o);
	}

	void WorldInternal::AddObjectInternal(Object* o)
	{
		assert(o != NULL);

		ObjectInternal* o_ = (ObjectInternal*) o;

		if (o_->GetNextStatus().Type == OBJECT_SHAPE_TYPE_ALL)
		{
			outofLayers.AddObject(o);
			o_->SetWorld(this);
			return;
		}

		float radius = o_->GetNextStatus().CalcRadius();
		if (o_->GetNextStatus().Type == OBJECT_SHAPE_TYPE_NONE || radius <= minGridSize)
		{
			if (layers[layers.size() - 1]->AddObject(o))
			{
			}
			else
			{
				outofLayers.AddObject(o);
			}
			o_->SetWorld(this);
			return;
		}

		int32_t gridInd = (int32_t) (gridSize / (radius * 2.0f));

		if (gridInd * (radius) < gridSize) gridInd++;

		int32_t ind = 1;
		bool found = false;
		for (size_t i = 0; i < layers.size(); i++)
		{
			if (ind <= gridInd && gridInd < ind * 2)
			{
				if (layers[i]->AddObject(o))
				{
					((ObjectInternal*) o)->SetWorld(this);
					found = true;
				}
				else
				{
					break;
				}
			}

			ind *= 2;
		}

		if (!found)
		{
			((ObjectInternal*) o)->SetWorld(this);
			outofLayers.AddObject(o);
		}
	}

	void WorldInternal::RemoveObjectInternal(Object* o)
	{
		assert(o != NULL);

		ObjectInternal* o_ = (ObjectInternal*) o;

		if (o_->GetCurrentStatus().Type == OBJECT_SHAPE_TYPE_ALL)
		{
			outofLayers.RemoveObject(o);
			o_->SetWorld(NULL);
			return;
		}

		float radius = o_->GetCurrentStatus().CalcRadius();
		if (o_->GetCurrentStatus().Type == OBJECT_SHAPE_TYPE_NONE || radius <= minGridSize)
		{
			if (layers[layers.size() - 1]->RemoveObject(o))
			{
			}
			else
			{
				outofLayers.RemoveObject(o);
			}
			o_->SetWorld(NULL);
			return;
		}

		int32_t gridInd = (int32_t) (gridSize / (radius * 2.0f));

		if (gridInd * (radius * 2.0f) < gridSize) gridInd++;

		int32_t ind = 1;
		bool found = false;
		for (size_t i = 0; i < layers.size(); i++)
		{
			if (ind <= gridInd && gridInd < ind * 2)
			{
				if (layers[i]->RemoveObject(o))
				{
					((ObjectInternal*) o)->SetWorld(NULL);
					found = true;
				}
				else
				{
					break;
				}
			}

			ind *= 2;
		}

		if (!found)
		{
			((ObjectInternal*) o)->SetWorld(NULL);
			outofLayers.RemoveObject(o);
		}
	}

	void WorldInternal::Culling(const Matrix44& cameraProjMat, bool isOpenGL)
	{
		objs.clear();
		
		Matrix44 cameraProjMatInv = cameraProjMat;
		cameraProjMatInv.SetInverted();

		float maxx = 1.0f;
		float minx = -1.0f;

		float maxy = 1.0f;
		float miny = -1.0f;

		float maxz = 1.0f;
		float minz = 0.0f;
		if (isOpenGL) minz = -1.0f;

		Vector3DF eyebox[8];

		eyebox[0 + 0] = Vector3DF(minx, miny, maxz);
		eyebox[1 + 0] = Vector3DF(maxx, miny, maxz);
		eyebox[2 + 0] = Vector3DF(minx, maxy, maxz);
		eyebox[3 + 0] = Vector3DF(maxx, maxy, maxz);

		eyebox[0 + 4] = Vector3DF(minx, miny, minz);
		eyebox[1 + 4] = Vector3DF(maxx, miny, minz);
		eyebox[2 + 4] = Vector3DF(minx, maxy, minz);
		eyebox[3 + 4] = Vector3DF(maxx, maxy, minz);

		for (int32_t i = 0; i < 8; i++)
		{
			eyebox[i] = cameraProjMatInv.Transform3D(eyebox[i]);
		}

		const int32_t xdiv = 2;
		const int32_t ydiv = 2;
		const int32_t zdiv = 3;

		for (int32_t z = 0; z < zdiv; z++)
		{
			for (int32_t y = 0; y < ydiv; y++)
			{
				for (int32_t x = 0; x < xdiv; x++)
				{
					Vector3DF eyebox_[8];

					float xsize = 1.0f / (float) xdiv;
					float ysize = 1.0f / (float) ydiv;
					float zsize = 1.0f / (float) zdiv;

					for (int32_t e = 0; e < 8; e++)
					{
						float x_, y_, z_;
						if (e == 0){ x_ = xsize * x; y_ = ysize * y; z_ = zsize * z; }
						if (e == 1){ x_ = xsize * (x + 1); y_ = ysize * y; z_ = zsize * z; }
						if (e == 2){ x_ = xsize * x; y_ = ysize * (y + 1); z_ = zsize * z; }
						if (e == 3){ x_ = xsize * (x + 1); y_ = ysize * (y + 1); z_ = zsize * z; }
						if (e == 4){ x_ = xsize * x; y_ = ysize * y; z_ = zsize * (z + 1); }
						if (e == 5){ x_ = xsize * (x + 1); y_ = ysize * y; z_ = zsize * (z + 1); }
						if (e == 6){ x_ = xsize * x; y_ = ysize * (y + 1); z_ = zsize * (z + 1); }
						if (e == 7){ x_ = xsize * (x + 1); y_ = ysize * (y + 1); z_ = zsize * (z + 1); }

						Vector3DF yzMid[4];
						yzMid[0] = eyebox[0] * x_ + eyebox[1] * (1.0f - x_);
						yzMid[1] = eyebox[2] * x_ + eyebox[3] * (1.0f - x_);
						yzMid[2] = eyebox[4] * x_ + eyebox[5] * (1.0f - x_);
						yzMid[3] = eyebox[6] * x_ + eyebox[7] * (1.0f - x_);

						Vector3DF zMid[2];
						zMid[0] = yzMid[0] * y_ + yzMid[1] * (1.0f - y_);
						zMid[1] = yzMid[2] * y_ + yzMid[3] * (1.0f - y_);
				
						eyebox_[e] = zMid[0] * z_ + zMid[1] * (1.0f - z_);
					}
					


					Vector3DF max_(-FLT_MAX, -FLT_MAX, -FLT_MAX);
					Vector3DF min_(FLT_MAX, FLT_MAX, FLT_MAX);

					for (int32_t i = 0; i < 8; i++)
					{
						if (eyebox_[i].X > max_.X) max_.X = eyebox_[i].X;
						if (eyebox_[i].Y > max_.Y) max_.Y = eyebox_[i].Y;
						if (eyebox_[i].Z > max_.Z) max_.Z = eyebox_[i].Z;

						if (eyebox_[i].X < min_.X) min_.X = eyebox_[i].X;
						if (eyebox_[i].Y < min_.Y) min_.Y = eyebox_[i].Y;
						if (eyebox_[i].Z < min_.Z) min_.Z = eyebox_[i].Z;
					}

					/* 範囲内に含まれるグリッドを取得 */
					for (size_t i = 0; i < layers.size(); i++)
					{
						layers[i]->AddGrids(max_, min_, grids);
					}
				}
			}
		}

		/* 外領域追加 */
		grids.push_back(&outofLayers);

		/* グリッドからオブジェクト取得 */
		for (size_t i = 0; i < grids.size(); i++)
		{
			for (size_t j = 0; j < grids[i]->GetObjects().size(); j++)
			{
				Object* o = grids[i]->GetObjects()[j];
				objs.push_back(o);
			}
		}

		/* 取得したグリッドを破棄 */
		for (size_t i = 0; i < grids.size(); i++)
		{
			grids[i]->IsScanned = false;
		}

		grids.clear();
	}
}