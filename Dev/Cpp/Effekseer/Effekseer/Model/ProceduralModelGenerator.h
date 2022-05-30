#ifndef __EFFEKSEER_PROCEDURAL_MODEL_GENERATOR_H__
#define __EFFEKSEER_PROCEDURAL_MODEL_GENERATOR_H__

#include "../Effekseer.Base.h"
#include "../Effekseer.Color.h"
#include "../SIMD/Vec2f.h"
#include "../SIMD/Vec3f.h"
#include "../Utils/Effekseer.CustomAllocator.h"
#include "Model.h"

namespace Effekseer
{

struct ProceduralMeshVertex
{
	SIMD::Vec3f Position;
	SIMD::Vec3f Normal;
	SIMD::Vec3f Tangent;
	SIMD::Vec2f UV;
	Color VColor;
};

struct ProceduralMeshFace
{
	std::array<int32_t, 3> Indexes;
};

struct ProceduralMesh
{
	CustomAlignedVector<ProceduralMeshVertex> Vertexes;
	CustomVector<ProceduralMeshFace> Faces;

	static ProceduralMesh Combine(ProceduralMesh mesh1, ProceduralMesh mesh2);
};

class ProceduralModelGenerator : public ReferenceObject
{
public:
	ProceduralModelGenerator() = default;
	virtual ~ProceduralModelGenerator() = default;

	virtual ModelRef Generate(const ProceduralModelParameter& parameter);

	virtual void Ungenerate(ModelRef model);

protected:
	virtual ModelRef ConvertMeshToModel(const ProceduralMesh& mesh);

	virtual ModelRef CreateModel(const CustomVector<Model::Vertex>& vs, const CustomVector<Model::Face>& faces);
};

} // namespace Effekseer

#endif