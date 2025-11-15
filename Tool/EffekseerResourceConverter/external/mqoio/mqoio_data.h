#pragma once

#include <vector>
#include <map>
#include <string>
#include <variant>

namespace mqoio
{

struct Vector2D
{
	double X, Y;
};

struct Vector3D
{
	double X, Y, Z;
};

struct Vector4D
{
	double X, Y, Z, W;
};

struct Color
{
	double R, G, B, A;
};

using Parameter = std::variant<std::string, double, Vector2D, Vector3D, Vector4D>;

struct Material
{
	std::string Name;
	std::map<std::string, Parameter> Paramaters;
};

struct Face
{
	std::vector<int> Indexes;
	std::vector<Vector2D> UV;
	std::vector<Color> Colors;
	int MaterialIndex = -1;
};

struct Object
{
	std::string Name;
	bool Visible = true;
	double Facet = 0.0;
	int Mirror = 0;
	int MirrorAxis = 0;
	std::vector<Vector3D> Vertexes;
	std::vector<Face> Faces;
};

struct Scene
{
	Vector3D Position;
	Vector3D LookAt;
	double Head;
	double Pich;
	int Ortho;
	double Zoom2;
	Vector3D Amb;
};

struct Data
{
	Scene SceneInfo;
	std::vector<Material> Materials;
	std::vector<Object> Objects;
};

}
