#pragma once

#include "EfkRes.Math.h"
#include <array>
#include <stdint.h>
#include <vector>

namespace efkres
{

struct MeshVertex
{
	Vec3 position{0.0, 0.0, 0.0};
	Vec3 normal{0.0, 0.0, 0.0};
	Vec3 tangent{0.0, 0.0, 0.0};
	Vec3 binormal{0.0, 0.0, 0.0};
	Vec4 color{1.0, 1.0, 1.0, 1.0};
	Vec2 uv1{0.0, 0.0};
	Vec2 uv2{0.0, 0.0};
};

struct MeshFace
{
	std::array<int32_t, 3> indices;
};

struct Mesh
{
	std::vector<MeshVertex> vertices;
	std::vector<MeshFace> faces;
};

struct Node
{
	Mesh* mesh = nullptr;
	Mat43 transform;
};

struct Model
{
	std::vector<Mesh> meshes;
	std::vector<Node> nodes;
};

} // namespace efkres
