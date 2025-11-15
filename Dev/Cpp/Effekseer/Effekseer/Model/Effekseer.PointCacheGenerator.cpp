#include "Effekseer.PointCacheGenerator.h"
#include "../SIMD/Bridge.h"
#include "../SIMD/Int4.h"
#include "../SIMD/Utils.h"
#include "../SIMD/Vec2f.h"
#include "../SIMD/Vec3f.h"
#include "../SIMD/Vec4f.h"
#include "Effekseer.Model.h"
#include <random>

namespace Effekseer
{

namespace
{
struct Random
{
	std::mt19937 engine;
	std::uniform_real_distribution<float> dist;

	Random(uint32_t seed)
		: engine(seed)
		, dist(0.0f, 1.0f)
	{
	}

	float operator()()
	{
		return dist(engine);
	}
};

template <class T>
T RandomTriangle(Random& random, T a, T b, T c)
{
	float u = random(), v = random();
	float t = std::min(u, v), s = std::max(u, v);
	float ma = t, mb = 1.0f - s, mc = s - t;
	return a * ma + b * mb + c * mc;
}

inline uint32_t PackNormal(SIMD::Vec3f v)
{
	v = v.GetNormal();
	v = (v + SIMD::Vec3f(1.0f)) * 0.5f * 1023.0f;
	SIMD::Int4 s = v.s.Convert4i();
	s = SIMD::Int4::Min(SIMD::Int4::Max(s, SIMD::Int4(0)), SIMD::Int4(1023));
	uint32_t xyz[3];
	SIMD::Int4::Store3(xyz, s);
	return xyz[0] | (xyz[1] << 10) | (xyz[2] << 20);
}

inline uint32_t PackUV(SIMD::Vec2f v)
{
	v *= 65535.0f;
	SIMD::Int4 s = v.s.Convert4i();
	s = SIMD::Int4::Min(SIMD::Int4::Max(s, SIMD::Int4(0)), SIMD::Int4(65535));
	uint32_t xy[2];
	SIMD::Int4::Store2(xy, s);
	return xy[0] | (xy[1] << 16);
}

inline uint32_t PackColor(SIMD::Vec4f v)
{
	v *= 255.0f;
	SIMD::Int4 s = v.s.Convert4i();
	s = SIMD::Int4::Min(SIMD::Int4::Max(s, SIMD::Int4(0)), SIMD::Int4(255));
	uint32_t xyzw[4];
	SIMD::Int4::Store4(xyzw, s);
	return xyzw[0] | (xyzw[1] << 8) | (xyzw[2] << 16) | (xyzw[3] << 24);
}

} // namespace

// (fp32-x3) Point
void PointCacheGenerator::SetPointBuffer(void* buffer, size_t stride)
{
	pointBuffer_ = reinterpret_cast<uintptr_t>(buffer);
	pointStride_ = stride;
}

// (Packed32-x4) Normal, Tangent, UV, Color
void PointCacheGenerator::SetAttributeBuffer(void* buffer, size_t stride)
{
	attribBuffer_ = reinterpret_cast<uintptr_t>(buffer);
	attribStride_ = stride;
}

void PointCacheGenerator::SetSourceModel(ModelRef model)
{
	model_ = model;

	int32_t frameCount = model_->GetFrameCount();
	modelFaceAreas_.resize((size_t)frameCount);

	// Calculate the area of all faces
	for (int32_t frameIndex = 0; frameIndex < frameCount; frameIndex++)
	{
		auto vertexes = model_->GetVertexes(frameIndex);
		int32_t vertexCount = model_->GetVertexCount(frameIndex);
		auto faces = model_->GetFaces(frameIndex);
		int32_t faceCount = model_->GetFaceCount(frameIndex);

		auto& faceAreas = modelFaceAreas_[frameIndex];
		faceAreas.resize((size_t)faceCount);

		for (int32_t faceIndex = 0; faceIndex < faceCount; faceIndex++)
		{
			auto& v0 = vertexes[faces[faceIndex].Indexes[0]];
			auto& v1 = vertexes[faces[faceIndex].Indexes[1]];
			auto& v2 = vertexes[faces[faceIndex].Indexes[2]];
			float r0 = Effekseer::Vector3D::Length(v0.Position - v1.Position);
			float r1 = Effekseer::Vector3D::Length(v1.Position - v2.Position);
			float r2 = Effekseer::Vector3D::Length(v2.Position - v0.Position);
			float s = (r0 + r1 + r2) / 2.0f;
			float area = sqrt(s * (s - r0) * (s - r1) * (s - r2));
			totalArea_ += area;
			faceAreas[faceIndex] = area;
		}
	}
}

void PointCacheGenerator::Generate(uint32_t pointCount, uint32_t seed)
{
	Random random(seed);

	int32_t frameCount = static_cast<int32_t>(modelFaceAreas_.size());

	uint32_t pointIndex = 0;
	float summedArea = 0.0f;
	for (int32_t frameIndex = 0; frameIndex < frameCount; frameIndex++)
	{
		auto vertexes = model_->GetVertexes(frameIndex);
		int32_t vertexCount = model_->GetVertexCount(frameIndex);
		auto faces = model_->GetFaces(frameIndex);
		int32_t faceCount = model_->GetFaceCount(frameIndex);

		auto& faceAreas = modelFaceAreas_[frameIndex];

		for (int32_t faceIndex = 0; faceIndex < faceCount; faceIndex++)
		{
			auto& v0 = vertexes[faces[faceIndex].Indexes[0]];
			auto& v1 = vertexes[faces[faceIndex].Indexes[1]];
			auto& v2 = vertexes[faces[faceIndex].Indexes[2]];

			summedArea += faceAreas[faceIndex];

			uint32_t genCount = (uint32_t)(summedArea / totalArea_ * pointCount) - pointIndex;
			for (uint32_t i = 0; i < genCount; i++)
			{
				Point* point = reinterpret_cast<Point*>(pointBuffer_ + pointIndex * pointStride_);
				Attribute* attrib = reinterpret_cast<Attribute*>(attribBuffer_ + pointIndex * attribStride_);
				point->Position = SIMD::ToStruct(RandomTriangle<SIMD::Vec3f>(random, v0.Position, v1.Position, v2.Position));
				attrib->PackedNormal = PackNormal(RandomTriangle<SIMD::Vec3f>(random, v0.Normal, v1.Normal, v2.Normal));
				attrib->PackedTangent = PackNormal(RandomTriangle<SIMD::Vec3f>(random, v0.Tangent, v1.Tangent, v2.Tangent));
				attrib->PackedUV = PackUV(RandomTriangle<SIMD::Vec2f>(random, v0.UV1, v1.UV1, v2.UV1));
				attrib->PackedColor = PackColor(RandomTriangle<SIMD::Vec4f>(random, v0.VColor, v1.VColor, v2.VColor));
				pointIndex += 1;
			}
		}
	}
}

} // namespace Effekseer
