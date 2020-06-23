
#ifndef __EFFEKSEER_MODEL_H__
#define __EFFEKSEER_MODEL_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "Effekseer.Base.h"
#include "Effekseer.Manager.h"
#include "Effekseer.Vector2D.h"
#include "Effekseer.Vector3D.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief
	\~English	Model class
	\~Japanese	モデルクラス
*/
class Model
{
public:
	static const int32_t Version = 1;

	struct Vertex
	{
		Vector3D Position;
		Vector3D Normal;
		Vector3D Binormal;
		Vector3D Tangent;
		Vector2D UV;
		Color VColor;
	};

	struct VertexWithIndex
	{
		Vector3D Position;
		Vector3D Normal;
		Vector3D Binormal;
		Vector3D Tangent;
		Vector2D UV;
		Color VColor;
		uint8_t Index[4];
	};

	struct Face
	{
		int32_t Indexes[3];
	};

	struct Emitter
	{
		Vector3D Position;
		Vector3D Normal;
		Vector3D Binormal;
		Vector3D Tangent;
	};

private:
	uint8_t* m_data;
	int32_t m_size;

	int32_t m_version;

	struct InternalModel
	{
		int32_t m_vertexCount;
		Vertex* m_vertexes;

		int32_t m_faceCount;
		Face* m_faces;
	};

	InternalModel* models;

	int32_t m_modelCount;
	int32_t m_frameCount;

protected:
	int32_t m_vertexSize = sizeof(Vertex);

public:
	/**
	@brief
	\~English	Constructor
	\~Japanese	コンストラクタ
	*/
	Model(void* data, int32_t size)
		: m_data(NULL)
		, m_size(size)
		, m_version(0)
		, models(nullptr)
	{
		m_data = new uint8_t[m_size];
		memcpy(m_data, data, m_size);

		uint8_t* p = (uint8_t*)m_data;

		memcpy(&m_version, p, sizeof(int32_t));
		p += sizeof(int32_t);

		// load scale except version 3(for compatibility)
		if (m_version == 2 || m_version >= 5)
		{
			// Scale
			p += sizeof(int32_t);
		}

		memcpy(&m_modelCount, p, sizeof(int32_t));
		p += sizeof(int32_t);

		if (m_version >= 5)
		{
			memcpy(&m_frameCount, p, sizeof(int32_t));
			p += sizeof(int32_t);
		}
		else
		{
			m_frameCount = 1;
		}

		models = new InternalModel[m_frameCount];

		for (int32_t f = 0; f < m_frameCount; f++)
		{
			memcpy(&models[f].m_vertexCount, p, sizeof(int32_t));
			p += sizeof(int32_t);

			if (m_version >= 1)
			{
				models[f].m_vertexes = (Vertex*)p;
				p += (sizeof(Vertex) * models[f].m_vertexCount);
			}
			else
			{
				// allocate new buffer
				models[f].m_vertexes = new Vertex[models[f].m_vertexCount];

				for (int32_t i = 0; i < models[f].m_vertexCount; i++)
				{
					memcpy((void*)&models[f].m_vertexes[i], p, sizeof(Vertex) - sizeof(Color));
					models[f].m_vertexes[i].VColor = Color(255, 255, 255, 255);

					p += sizeof(Vertex) - sizeof(Color);
				}
			}

			memcpy(&models[f].m_faceCount, p, sizeof(int32_t));
			p += sizeof(int32_t);

			models[f].m_faces = (Face*)p;
			p += (sizeof(Face) * models[f].m_faceCount);
		}
	}

	Vertex* GetVertexes(int32_t index = 0) const
	{
		return models[index].m_vertexes;
	}
	int32_t GetVertexCount(int32_t index = 0)
	{
		return models[index].m_vertexCount;
	}

	Face* GetFaces(int32_t index = 0) const
	{
		return models[index].m_faces;
	}
	int32_t GetFaceCount(int32_t index = 0)
	{
		return models[index].m_faceCount;
	}

	int32_t GetFrameCount() const
	{
		return m_frameCount;
	}

	int32_t GetModelCount()
	{
		return m_modelCount;
	}

	int32_t GetVertexSize() const
	{
		return m_vertexSize;
	}

	/**
		@brief
		\~English	Destructor
		\~Japanese	デストラクタ
	*/
	virtual ~Model()
	{
		if (m_version == 0)
		{
			ES_SAFE_DELETE_ARRAY(models[0].m_vertexes);
		}

		ES_SAFE_DELETE_ARRAY(models);
		ES_SAFE_DELETE_ARRAY(m_data);
	}

	Emitter GetEmitter(IRandObject* g, int32_t time, CoordinateSystem coordinate, float magnification)
	{
		time = time % GetFrameCount();

		int32_t faceInd = (int32_t)((GetFaceCount(time) - 1) * (g->GetRand()));
		faceInd = Clamp(faceInd, GetFaceCount(time) - 1, 0);
		Face& face = GetFaces(time)[faceInd];
		Vertex& v0 = GetVertexes(time)[face.Indexes[0]];
		Vertex& v1 = GetVertexes(time)[face.Indexes[1]];
		Vertex& v2 = GetVertexes(time)[face.Indexes[2]];

		float p1 = g->GetRand();
		float p2 = g->GetRand();

		// Fit within plane
		if (p1 + p2 > 1.0f)
		{
			p1 = 1.0f - p1;
			p2 = 1.0f - p2;
		}

		float p0 = 1.0f - p1 - p2;

		Emitter emitter;
		emitter.Position = (v0.Position * p0 + v1.Position * p1 + v2.Position * p2) * magnification;
		emitter.Normal = v0.Normal * p0 + v1.Normal * p1 + v2.Normal * p2;
		emitter.Binormal = v0.Binormal * p0 + v1.Binormal * p1 + v2.Binormal * p2;
		emitter.Tangent = v0.Tangent * p0 + v1.Tangent * p1 + v2.Tangent * p2;

		if (coordinate == CoordinateSystem::LH)
		{
			emitter.Position.Z = -emitter.Position.Z;
			emitter.Normal.Z = -emitter.Normal.Z;
			emitter.Binormal.Z = -emitter.Binormal.Z;
			emitter.Tangent.Z = -emitter.Tangent.Z;
		}

		return emitter;
	}

	Emitter GetEmitterFromVertex(IRandObject* g, int32_t time, CoordinateSystem coordinate, float magnification)
	{
		time = time % GetFrameCount();

		int32_t vertexInd = (int32_t)((GetVertexCount(time) - 1) * (g->GetRand()));
		vertexInd = Clamp(vertexInd, GetVertexCount(time) - 1, 0);
		Vertex& v = GetVertexes(time)[vertexInd];

		Emitter emitter;
		emitter.Position = v.Position * magnification;
		emitter.Normal = v.Normal;
		emitter.Binormal = v.Binormal;
		emitter.Tangent = v.Tangent;

		if (coordinate == CoordinateSystem::LH)
		{
			emitter.Position.Z = -emitter.Position.Z;
			emitter.Normal.Z = -emitter.Normal.Z;
			emitter.Binormal.Z = -emitter.Binormal.Z;
			emitter.Tangent.Z = -emitter.Tangent.Z;
		}

		return emitter;
	}

	Emitter GetEmitterFromVertex(int32_t index, int32_t time, CoordinateSystem coordinate, float magnification)
	{
		time = time % GetFrameCount();

		int32_t vertexInd = index % GetVertexCount(time);
		Vertex& v = GetVertexes(time)[vertexInd];

		Emitter emitter;
		emitter.Position = v.Position * magnification;
		emitter.Normal = v.Normal;
		emitter.Binormal = v.Binormal;
		emitter.Tangent = v.Tangent;

		if (coordinate == CoordinateSystem::LH)
		{
			emitter.Position.Z = -emitter.Position.Z;
			emitter.Normal.Z = -emitter.Normal.Z;
			emitter.Binormal.Z = -emitter.Binormal.Z;
			emitter.Tangent.Z = -emitter.Tangent.Z;
		}

		return emitter;
	}

	Emitter GetEmitterFromFace(IRandObject* g, int32_t time, CoordinateSystem coordinate, float magnification)
	{
		time = time % GetFrameCount();

		int32_t faceInd = (int32_t)((GetFaceCount(time) - 1) * (g->GetRand()));
		faceInd = Clamp(faceInd, GetFaceCount(time) - 1, 0);
		Face& face = GetFaces(time)[faceInd];
		Vertex& v0 = GetVertexes(time)[face.Indexes[0]];
		Vertex& v1 = GetVertexes(time)[face.Indexes[1]];
		Vertex& v2 = GetVertexes(time)[face.Indexes[2]];

		float p0 = 1.0f / 3.0f;
		float p1 = 1.0f / 3.0f;
		float p2 = 1.0f / 3.0f;

		Emitter emitter;
		emitter.Position = (v0.Position * p0 + v1.Position * p1 + v2.Position * p2) * magnification;
		emitter.Normal = v0.Normal * p0 + v1.Normal * p1 + v2.Normal * p2;
		emitter.Binormal = v0.Binormal * p0 + v1.Binormal * p1 + v2.Binormal * p2;
		emitter.Tangent = v0.Tangent * p0 + v1.Tangent * p1 + v2.Tangent * p2;

		if (coordinate == CoordinateSystem::LH)
		{
			emitter.Position.Z = -emitter.Position.Z;
			emitter.Normal.Z = -emitter.Normal.Z;
			emitter.Binormal.Z = -emitter.Binormal.Z;
			emitter.Tangent.Z = -emitter.Tangent.Z;
		}

		return emitter;
	}

	Emitter GetEmitterFromFace(int32_t index, int32_t time, CoordinateSystem coordinate, float magnification)
	{
		time = time % GetFrameCount();

		int32_t faceInd = index % (GetFaceCount(time) - 1);
		Face& face = GetFaces(time)[faceInd];
		Vertex& v0 = GetVertexes(time)[face.Indexes[0]];
		Vertex& v1 = GetVertexes(time)[face.Indexes[1]];
		Vertex& v2 = GetVertexes(time)[face.Indexes[2]];

		float p0 = 1.0f / 3.0f;
		float p1 = 1.0f / 3.0f;
		float p2 = 1.0f / 3.0f;

		Emitter emitter;
		emitter.Position = (v0.Position * p0 + v1.Position * p1 + v2.Position * p2) * magnification;
		emitter.Normal = v0.Normal * p0 + v1.Normal * p1 + v2.Normal * p2;
		emitter.Binormal = v0.Binormal * p0 + v1.Binormal * p1 + v2.Binormal * p2;
		emitter.Tangent = v0.Tangent * p0 + v1.Tangent * p1 + v2.Tangent * p2;

		if (coordinate == CoordinateSystem::LH)
		{
			emitter.Position.Z = -emitter.Position.Z;
			emitter.Normal.Z = -emitter.Normal.Z;
			emitter.Binormal.Z = -emitter.Binormal.Z;
			emitter.Tangent.Z = -emitter.Tangent.Z;
		}

		return emitter;
	}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace Effekseer
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEER_MODEL_H__
