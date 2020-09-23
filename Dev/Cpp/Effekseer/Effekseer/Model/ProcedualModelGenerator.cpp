#include "ProcedualModelGenerator.h"
#include "../Model/Model.h"
#include "../SIMD/Effekseer.SIMDUtils.h"
#include "ProcedualModelParameter.h"

#define _USE_MATH_DEFINES
#include <cmath>
#include <iterator>

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

	static ProcedualMesh Combine(ProcedualMesh mesh1, ProcedualMesh mesh2)
	{
		const auto vertexOffset = mesh1.Vertexes.size();
		const auto faceOffset = mesh1.Faces.size();

		std::copy(mesh2.Vertexes.begin(), mesh2.Vertexes.end(), std::back_inserter(mesh1.Vertexes));
		std::copy(mesh2.Faces.begin(), mesh2.Faces.end(), std::back_inserter(mesh1.Faces));

		for (size_t f = faceOffset; f < mesh1.Faces.size(); f++)
		{
			for (auto& ind : mesh1.Faces[f].Indexes)
			{
				ind += static_cast<int32_t>(vertexOffset);
			}
		}

		return std::move(mesh1);
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

	CustomAlignedUnorderedMap<Vec3f, Vec3f> normals;
	CustomAlignedUnorderedMap<Vec3f, Vec3f> tangents;
	CustomAlignedUnorderedMap<Vec3f, int32_t> vertexCounts;

	for (size_t i = 0; i < mesh.Faces.size(); i++)
	{
		for (size_t j = 0; j < 3; j++)
		{
			const auto& key = mesh.Vertexes[mesh.Faces[i].Indexes[j]].Position;

			if (normals.count(key) == 0)
			{
				normals[key] = Vec3f(0.0f, 0.0f, 0.0f);
				tangents[key] = Vec3f(0.0f, 0.0f, 0.0f);
				vertexCounts[key] = 0;
			}

			normals[key] += faceNormals[i];
			tangents[key] += faceTangents[i];
			vertexCounts[key]++;
		}
	}

	for (size_t i = 0; i < mesh.Vertexes.size(); i++)
	{
		const auto& key = mesh.Vertexes[i].Position;
		mesh.Vertexes[i].Normal = normals[key] / static_cast<float>(vertexCounts[key]);
		mesh.Vertexes[i].Tangent = tangents[key] / static_cast<float>(vertexCounts[key]);
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

struct RotatorSphere
{
	float Radius;
	float DepthMin;
	float DepthMax;

	Vec2f GetPosition(float value) const
	{
		const auto depthMax = Clamp(DepthMax, 1.0f, -1.0f);
		const auto depthMin = Clamp(DepthMin, 1.0f, -1.0f);

		float valueMin = depthMin * Radius;
		float valueMax = depthMax * Radius;

		value = Clamp(value, 1.0f, 0.0f);
		float axisPos = (DepthMax - DepthMin) * value + DepthMin;

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

struct RotatorMeshGenerator
{
	float AngleMin;
	float AngleMax;
	bool IsConnected = false;

	std::function<Vec2f(float)> Rotator;

	Vec3f GetPosition(float angleValue, float depthValue) const
	{
		depthValue = Clamp(depthValue, 1.0f, 0.0f);
		auto angle = (AngleMax - AngleMin) * angleValue + AngleMin;

		Vec2f pos2d = Rotator(depthValue);

		float s;
		float c;
		SinCos(angle, s, c);

		auto rx = pos2d.GetX() * s;
		auto rz = pos2d.GetX() * c;
		auto y = pos2d.GetY();

		return Vec3f(rx, y, rz);
	}

	ProcedualMesh Generate(int32_t angleDivision, int32_t depthDivision) const
	{
		assert(depthDivision > 1);
		assert(angleDivision > 1);

		ProcedualMesh ret;

		ret.Vertexes.resize(depthDivision * angleDivision);
		ret.Faces.resize((depthDivision - 1) * (angleDivision - 1) * 2);

		for (int32_t v = 0; v < depthDivision; v++)
		{
			for (int32_t u = 0; u < angleDivision; u++)
			{
				ret.Vertexes[u + v * angleDivision].Position = GetPosition(u / float(angleDivision - 1), v / float(depthDivision - 1));
				ret.Vertexes[u + v * angleDivision].UV = Vec2f(u / float(angleDivision - 1), 1.0f - v / float(depthDivision - 1));
			}
		}

		if (IsConnected)
		{
			for (int32_t v = 0; v < depthDivision; v++)
			{
				ret.Vertexes[(angleDivision - 1) + v * angleDivision].Position = ret.Vertexes[0 + v * angleDivision].Position;
			}
		}

		for (int32_t v = 0; v < depthDivision - 1; v++)
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
				face0.Indexes[1] = v11;
				face0.Indexes[2] = v10;

				face1.Indexes[0] = v00;
				face1.Indexes[1] = v01;
				face1.Indexes[2] = v11;

				ret.Faces[(u + v * (angleDivision - 1)) * 2 + 0] = face0;
				ret.Faces[(u + v * (angleDivision - 1)) * 2 + 1] = face1;
			}
		}

		return ret;
	}
};

struct RotatedWireMeshGenerator
{
	float Rotate;
	int Vertices;
	int Count;

	std::function<Vec2f(float)> Rotator;

	Vec3f GetPosition(float angleValue, float depthValue) const
	{
		auto value = depthValue;
		auto angle = angleValue;

		Vec2f pos2d = Rotator(value);

		float s;
		float c;
		SinCos(angle, s, c);

		auto rx = pos2d.GetX() * s;
		auto rz = pos2d.GetX() * c;
		auto y = pos2d.GetY();

		return Vec3f(rx, y, rz);
	}

	ProcedualMesh Generate() const
	{
		float depthSpeed = 1.0f / static_cast<float>(Vertices);
		float rotateSpeed = Rotate * EFK_PI * 2.0f / static_cast<float>(Vertices);

		ProcedualMesh ret;

		for (int32_t l = 0; l < Count; l++)
		{
			float currentAngle = static_cast<float>(l) / static_cast<float>(Count) * EFK_PI * 2.0f;
			float currentDepth = 0.0f;

			CustomAlignedVector<Vec3f> vs;
			CustomAlignedVector<Vec3f> binormals;
			CustomAlignedVector<Vec3f> normals;

			while (currentDepth < 1.0f)
			{
				auto pos = GetPosition(currentAngle, currentDepth);

				const auto eps = 0.0001f;
				auto pos_diff = GetPosition(currentAngle + eps * rotateSpeed, currentDepth + eps * depthSpeed);
				auto pos_diff_angle = GetPosition(currentAngle + eps, currentDepth);
				auto pos_diff_axis = GetPosition(currentAngle, currentDepth + eps);
				auto normal = Vec3f::Cross(pos_diff_angle - pos, pos_diff_axis - pos);

				vs.emplace_back(pos);
				normals.emplace_back(normal.Normalize());
				binormals.emplace_back((pos_diff - pos).Normalize());
				currentDepth += depthSpeed;
				currentAngle += rotateSpeed;
			}

			ProcedualMesh ribbon;

			ribbon.Vertexes.resize(vs.size() * 2);

			for (int32_t v = 0; v < vs.size(); v++)
			{
				ribbon.Vertexes[v * 2 + 0].Position = vs[v] - Vec3f::Cross(normals[v], binormals[v]).Normalize() * 0.1f;
				ribbon.Vertexes[v * 2 + 1].Position = vs[v] + Vec3f::Cross(normals[v], binormals[v]).Normalize() * 0.1f;
				ribbon.Vertexes[v * 2 + 0].UV = Vec2f(0.0f, v / static_cast<float>(vs.size() - 1));
				ribbon.Vertexes[v * 2 + 1].UV = Vec2f(1.0f, v / static_cast<float>(vs.size() - 1));
			}

			ribbon.Faces.resize((vs.size() - 1) * 2);

			for (int32_t v = 0; v < vs.size() - 1; v++)
			{
				ProcedualMeshFace face0;
				ProcedualMeshFace face1;

				int32_t v00 = (0) + (v + 0) * (2);
				int32_t v10 = (1) + (v + 0) * (2);
				int32_t v01 = (0) + (v + 1) * (2);
				int32_t v11 = (1) + (v + 1) * (2);

				face0.Indexes[0] = v00;
				face0.Indexes[1] = v11;
				face0.Indexes[2] = v10;

				face1.Indexes[0] = v00;
				face1.Indexes[1] = v01;
				face1.Indexes[2] = v11;

				ribbon.Faces[v * 2 + 0] = face0;
				ribbon.Faces[v * 2 + 1] = face1;
			}

			CalculateNormal(ribbon);

			ret = ProcedualMesh::Combine(std::move(ret), std::move(ribbon));
		}

		return std::move(ret);
	}
};

Model* ProcedualModelGenerator::Generate(const ProcedualModelParameter* parameter)
{
	if (parameter == nullptr)
	{
		return nullptr;
	}

	std::function<Vec2f(float)> primitiveGenerator;

	if (parameter->PrimitiveType == ProcedualModelPrimitiveType::Sphere)
	{
		RotatorSphere rotator;
		rotator.DepthMin = parameter->Sphere.DepthMin;
		rotator.DepthMax = parameter->Sphere.DepthMax;
		rotator.Radius = parameter->Sphere.Radius;

		primitiveGenerator = [rotator](float value) -> Vec2f {
			return rotator.GetPosition(value);
		};
	}
	else if (parameter->PrimitiveType == ProcedualModelPrimitiveType::Cone)
	{
		RotatorCone rotator;
		rotator.Radius = parameter->Cone.Radius;
		rotator.Depth = parameter->Cone.Depth;

		primitiveGenerator = [rotator](float value) -> Vec2f {
			return rotator.GetPosition(value);
		};
	}
	else if (parameter->PrimitiveType == ProcedualModelPrimitiveType::Cylinder)
	{
		RotatorCylinder rotator;
		rotator.Radius1 = parameter->Cylinder.Radius1;
		rotator.Radius2 = parameter->Cylinder.Radius2;
		rotator.Depth = parameter->Cone.Depth;

		primitiveGenerator = [rotator](float value) -> Vec2f {
			return rotator.GetPosition(value);
		};
	}
	else
	{
		assert(0);
	}

	if (parameter->Type == ProcedualModelType::Mesh)
	{
		const auto AngleBegin = parameter->Mesh.AngleBegin / 180.0f * EFK_PI;
		const auto AngleEnd = parameter->Mesh.AngleEnd / 180.0f * EFK_PI;
		const auto eps = 0.000001f;
		const auto isConnected = std::fmod(std::abs(parameter->Mesh.AngleBegin - parameter->Mesh.AngleEnd), 360.0f) < eps;

		auto generator = RotatorMeshGenerator();
		generator.Rotator = primitiveGenerator;
		generator.AngleMin = AngleBegin;
		generator.AngleMax = AngleEnd;
		generator.IsConnected = isConnected;
		auto generated = generator.Generate(parameter->Mesh.AngleDivision, parameter->Mesh.DepthDivision);

		CalculateNormal(generated);
		return ConvertMeshToModel(generated);
	}
	else if (parameter->Type == ProcedualModelType::Ribbon)
	{
		auto generator = RotatedWireMeshGenerator();
		generator.Rotator = primitiveGenerator;
		generator.Vertices = parameter->Ribbon.Vertices;
		generator.Rotate = parameter->Ribbon.Rotate;
		generator.Count = parameter->Ribbon.Count;

		auto generated = generator.Generate();

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