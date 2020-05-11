#include "ProcedualModelGenerator.h"
#include "../Model/Model.h"
#include "../SIMD/Effekseer.SIMDUtils.h"
#include "ProcedualModelParameter.h"

#define _USE_MATH_DEFINES
#include <math.h>

namespace Effekseer
{

/*
	memo

	// Sugoi tube

	// wire

	// Rectangle

	// Box

	// Others

	// Filter
	// Twist
	// Jitter
	// Require normal
	// Calculate tangent
*/

struct RotatorSphere
{
	float Radius;

	Vec2f GetPosition(float value) const
	{
		value = Clamp(value, 1.0f, 0.0f);
		float axisPos = -Radius + Radius * 2.0f * value;
		return Vec2f(sqrt(Radius * Radius - axisPos * axisPos), axisPos);
	}
};

struct RotatorCone
{
	float Radius;
	float Depth;

	Vec2f GetPosition(float value) const
	{
		value = Clamp(value, 1.0f, 0.0f);
		float axisPos = Depth * value;
		return Vec2f(Radius / Depth * axisPos, axisPos);
	}
};

struct RotatorCylinder
{
	float Radius1;
	float Radius2;
	float Depth;

	Vec2f GetPosition(float value) const
	{
		value = Clamp(value, 1.0f, 0.0f);
		float axisPos = Depth * value;
		return Vec2f((Radius2 - Radius1) / Depth * axisPos + Radius1, axisPos);
	}
};

struct ProcedualMeshVertex
{
	Vec3f Position;
	Vec3f Normal;
	Vec3f Tangent;
	Vec2f UV;
};

struct ProcedualMeshFace
{
	std::array<int32_t, 3> Indexes;
};

struct ProcedualMesh
{
	CustomAlignedVector<ProcedualMeshVertex> Vertexes;
	CustomVector<ProcedualMeshFace> Faces;
};

template <typename T>
struct RotatorMeshGenerator
{
	float DepthMin;
	float DepthMax;
	float AngleMin;
	float AngleMax;
	T Rotator;

	Vec3f GetPosition(float axisValue, float angleValue) const
	{
		auto value = (DepthMax - DepthMin) * axisValue + DepthMin;
		auto angle = (AngleMax - AngleMin) * angleValue + AngleMin;

		Vec2f pos2d = Rotator.GetPosition(value);

		float s;
		float c;
		SinCos(angle, s, c);

		auto rx = pos2d.GetX() * s;
		auto rz = pos2d.GetX() * c;
		auto y = pos2d.GetY();

		return Vec3f(rx, y, rz);
	}

	ProcedualMesh Generate(int32_t axisDivision, int32_t angleDivision) const
	{
		assert(axisDivision > 1);
		assert(angleDivision > 1);

		ProcedualMesh ret;

		ret.Vertexes.resize(axisDivision * angleDivision);
		ret.Faces.resize((axisDivision - 1) * (angleDivision - 1) * 2);

		for (int32_t v = 0; v < axisDivision; v++)
		{
			for (int32_t u = 0; u < angleDivision; u++)
			{
				ret.Vertexes[u + v * angleDivision].Position = GetPosition(u / float(angleDivision - 1), v / float(axisDivision - 1));
				ret.Vertexes[u + v * angleDivision].UV = Vec2f(u / float(angleDivision - 1), v / float(axisDivision - 1));
			}
		}

		for (int32_t v = 0; v < axisDivision - 1; v++)
		{
			for (int32_t u = 0; u < angleDivision - 1; u++)
			{
				ProcedualMeshFace face0;
				ProcedualMeshFace face1;

				int32_t v00 = (u + 0) + (v + 0) * (angleDivision);
				int32_t v10 = (u + 1) + (v + 0) * (angleDivision);
				int32_t v01 = (u + 0) + (v + 1) * (angleDivision);
				int32_t v11 = (u + 1) + (v + 1) * (angleDivision);

				face0.Indexes[0] = v00;
				face0.Indexes[1] = v10;
				face0.Indexes[2] = v11;

				face1.Indexes[0] = v00;
				face1.Indexes[1] = v11;
				face1.Indexes[2] = v01;

				ret.Faces[(u + v * (angleDivision - 1)) * 2 + 0] = face0;
				ret.Faces[(u + v * (angleDivision - 1)) * 2 + 1] = face1;
			}
		}

		return ret;
	}
};

template <typename T>
struct RotatedWireMeshGenerator
{

	ProcedualMesh Generate(float angleBegin, float angleEnd, float axisBegin, float axisEnd, int32_t axisDivision, int32_t angleDivision)
	{
	}
};

static void CalcTangentSpace(const ProcedualMeshVertex& v1, const ProcedualMeshVertex& v2, const ProcedualMeshVertex& v3, Vec3f& binormal, Vec3f& tangent)
{
	binormal = Vec3f();
	tangent = Vec3f();

	Vec3f cp0[3];
	cp0[0] = Vec3f(v1.Position.GetX(), v1.UV.GetX(), v1.UV.GetY());
	cp0[1] = Vec3f(v1.Position.GetY(), v1.UV.GetX(), v1.UV.GetY());
	cp0[2] = Vec3f(v1.Position.GetZ(), v1.UV.GetX(), v1.UV.GetY());

	Vec3f cp1[3];
	cp1[0] = Vec3f(v2.Position.GetX(), v2.UV.GetX(), v2.UV.GetY());
	cp1[1] = Vec3f(v2.Position.GetY(), v2.UV.GetX(), v2.UV.GetY());
	cp1[2] = Vec3f(v2.Position.GetZ(), v2.UV.GetX(), v2.UV.GetY());

	Vec3f cp2[3];
	cp2[0] = Vec3f(v3.Position.GetX(), v3.UV.GetX(), v3.UV.GetY());
	cp2[1] = Vec3f(v3.Position.GetY(), v3.UV.GetX(), v3.UV.GetY());
	cp2[2] = Vec3f(v3.Position.GetZ(), v3.UV.GetX(), v3.UV.GetY());

	float u[3];
	float v[3];

	for (int32_t i = 0; i < 3; i++)
	{
		auto v1 = cp1[i] - cp0[i];
		auto v2 = cp2[i] - cp1[i];
		auto abc = Vec3f::Cross(v1, v2);

		if (abc.GetX() == 0.0f)
		{
			return;
		}
		else
		{
			u[i] = -abc.GetY() / abc.GetX();
			v[i] = -abc.GetZ() / abc.GetX();
		}
	}

	tangent = Vec3f(u[0], u[1], u[2]);
	tangent.Normalize();

	binormal = Vec3f(v[0], v[1], v[2]);
	binormal.Normalize();
}

static void CalculateNormal(ProcedualMesh& mesh)
{
	CustomAlignedVector<Vec3f> faceNormals;
	CustomAlignedVector<Vec3f> faceTangents;

	faceNormals.resize(mesh.Faces.size());
	faceTangents.resize(mesh.Faces.size());

	for (size_t i = 0; i < faceNormals.size(); i++)
	{
		faceNormals[i] = Vec3f(0.0f, 0.0f, 0.0f);
		faceTangents[i] = Vec3f(0.0f, 0.0f, 0.0f);
	}

	for (size_t i = 0; i < mesh.Faces.size(); i++)
	{
		const auto& v1 = mesh.Vertexes[mesh.Faces[i].Indexes[0]];
		const auto& v2 = mesh.Vertexes[mesh.Faces[i].Indexes[1]];
		const auto& v3 = mesh.Vertexes[mesh.Faces[i].Indexes[2]];
		const auto normal = Vec3f::Cross(v3.Position - v1.Position, v2.Position - v1.Position).Normalize();

		faceNormals[i] = normal;
		Vec3f binotmal;
		Vec3f tangent;

		CalcTangentSpace(v1, v2, v3, binotmal, tangent);

		faceTangents[i] = tangent;
	}

	CustomAlignedVector<Vec3f> normals;
	CustomAlignedVector<Vec3f> tangents;
	CustomVector<int32_t> vertexCounts;

	normals.resize(mesh.Vertexes.size());
	tangents.resize(mesh.Vertexes.size());
	vertexCounts.resize(mesh.Vertexes.size());

	for (size_t i = 0; i < normals.size(); i++)
	{
		normals[i] = Vec3f(0.0f, 0.0f, 0.0f);
		tangents[i] = Vec3f(0.0f, 0.0f, 0.0f);
	}

	for (size_t i = 0; i < mesh.Faces.size(); i++)
	{
		for (size_t j = 0; j < 3; j++)
		{
			normals[mesh.Faces[i].Indexes[j]] = faceNormals[i];
			tangents[mesh.Faces[i].Indexes[j]] = faceTangents[i];
			vertexCounts[mesh.Faces[i].Indexes[j]]++;
		}
	}

	for (size_t i = 0; i < normals.size(); i++)
	{
		mesh.Vertexes[i].Normal = normals[i] / static_cast<float>(vertexCounts[i]);
		mesh.Vertexes[i].Tangent = tangents[i] / static_cast<float>(vertexCounts[i]);
	}
}

static Model* ConvertMeshToModel(const ProcedualMesh& mesh)
{
	CustomVector<Model::Vertex> vs;
	CustomVector<Model::Face> faces;

	vs.resize(mesh.Vertexes.size());
	faces.resize(mesh.Faces.size());

	for (size_t i = 0; i < vs.size(); i++)
	{
		vs[i].Position = ToStruct(mesh.Vertexes[i].Position);
		vs[i].Normal = ToStruct(mesh.Vertexes[i].Normal);
		vs[i].Tangent = ToStruct(mesh.Vertexes[i].Tangent);
		vs[i].UV = ToStruct(mesh.Vertexes[i].UV);
		Vector3D::Cross(vs[i].Binormal, vs[i].Normal, vs[i].Tangent);
		Vector3D::Normal(vs[i].Binormal, vs[i].Binormal);
		vs[i].VColor = Color(255, 255, 255, 255);
	}

	for (size_t i = 0; i < faces.size(); i++)
	{
		faces[i].Indexes[0] = mesh.Faces[i].Indexes[0];
		faces[i].Indexes[1] = mesh.Faces[i].Indexes[1];
		faces[i].Indexes[2] = mesh.Faces[i].Indexes[2];
	}

	return new Model(vs, faces);
}

Model* ProcedualModelGenerator::Generate(const ProcedualModelParameter* parameter)
{
	if (parameter == nullptr)
	{
		return nullptr;
	}

	if (parameter->Type == ProcedualModelType::Sphere)
	{
		auto generator = RotatorMeshGenerator<RotatorSphere>();
		generator.AngleMin = parameter->AngleBegin;
		generator.AngleMax = parameter->AngleEnd;
		generator.DepthMin = parameter->DepthMin;
		generator.DepthMax = parameter->DepthMax;
		generator.Rotator.Radius = parameter->Sphere.Radius;
		auto generated = generator.Generate(parameter->AxisDivision, parameter->AngleDivision);

		CalculateNormal(generated);

		return ConvertMeshToModel(generated);
	}
	else if (parameter->Type == ProcedualModelType::Cone)
	{
		auto generator = RotatorMeshGenerator<RotatorCone>();
		generator.AngleMin = parameter->AngleBegin;
		generator.AngleMax = parameter->AngleEnd;
		generator.DepthMin = parameter->DepthMin;
		generator.DepthMax = parameter->DepthMax;
		generator.Rotator.Radius = parameter->Cone.Radius;
		generator.Rotator.Depth = parameter->Cone.Depth;
		auto generated = generator.Generate(parameter->AxisDivision, parameter->AngleDivision);

		CalculateNormal(generated);

		return ConvertMeshToModel(generated);
	}
	else if (parameter->Type == ProcedualModelType::Cylinder)
	{
		auto generator = RotatorMeshGenerator<RotatorCylinder>();
		generator.AngleMin = parameter->AngleBegin;
		generator.AngleMax = parameter->AngleEnd;
		generator.DepthMin = parameter->DepthMin;
		generator.DepthMax = parameter->DepthMax;
		generator.Rotator.Radius1 = parameter->Cylinder.Radius1;
		generator.Rotator.Radius2 = parameter->Cylinder.Radius2;
		generator.Rotator.Depth = parameter->Cylinder.Depth;
		auto generated = generator.Generate(parameter->AxisDivision, parameter->AngleDivision);

		CalculateNormal(generated);

		return ConvertMeshToModel(generated);
	}

	return nullptr;
}

void ProcedualModelGenerator::Ungenerate(Model* model)
{
	if (model != nullptr)
	{
		delete model;
	}
}

} // namespace Effekseer