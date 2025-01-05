#include <algorithm>
#include "EfkRes.Utils.h"
#include "EfkRes.EfkModelSaver.h"

namespace efkres
{

struct PackedMeshVertex1
{
	std::array<float, 3> position;
	std::array<float, 3> normal;
	std::array<float, 3> binormal;
	std::array<float, 3> tangent;
	std::array<float, 2> uv;
	std::array<uint8_t, 4> color;
};

struct PackedMeshVertex2
{
	std::array<float, 3> position;
	std::array<int16_t, 4> qtangent;
	std::array<uint8_t, 4> color;
	std::array<int16_t, 2> uv1;
	std::array<int16_t, 2> uv2;
};

namespace
{

std::array<float, 2> QuantizeFloat32(Vec2 v)
{
	return { static_cast<float>(v.x), static_cast<float>(v.y) };
}

std::array<float, 3> QuantizeFloat32(Vec3 v)
{
	return { static_cast<float>(v.x), static_cast<float>(v.y), static_cast<float>(v.z) };
}

template <class T>
T QuantizeIntValue(double value)
{
	using limit = std::numeric_limits<T>;
	double qValue = value * limit::max() + copysign(0.5, value);
	double minValue = static_cast<double>(limit::min());
	double maxValue = static_cast<double>(limit::max());
	return static_cast<T>(std::clamp(qValue, minValue, maxValue));
}

std::array<uint8_t, 4> QuantizeUnorm8(Vec4 v)
{
	return {
		QuantizeIntValue<uint8_t>(v.x),
		QuantizeIntValue<uint8_t>(v.y),
		QuantizeIntValue<uint8_t>(v.z),
		QuantizeIntValue<uint8_t>(v.w),
	};
}

}

bool EfkModelSaver::Save(std::string_view filepath, const Model& model)
{
	return SaveAsVersion5(std::string(filepath).c_str(), model);
}

bool EfkModelSaver::SaveAsVersion5(const char* filepath, const Model& model)
{
	const int32_t version = 5;
	float modelScale = m_scale;
	int32_t modelCount = 1;
	int32_t frameCount = 1;

	BinaryWriter writer;
	if (!writer.Open(filepath))
	{
		return false;
	}

	writer.Write(version);
	writer.Write(modelScale);
	writer.Write(modelCount);
	writer.Write(frameCount);

	int32_t vertexCount = 0;
	int32_t faceCount = 0;

	for (auto& mesh : model.meshes)
	{
		vertexCount += static_cast<int32_t>(mesh.vertices.size());
		faceCount += static_cast<int32_t>(mesh.faces.size());
	}

	writer.Write(vertexCount);

	for (auto& node : model.nodes)
	{
		if (node.mesh == nullptr)
		{
			continue;
		}

		auto& mesh = *node.mesh;

		for (auto& vertex : mesh.vertices)
		{
			auto position = QuantizeFloat32(node.transform.TransformPosition(vertex.position * modelScale));
			auto normal = QuantizeFloat32(node.transform.TransformDirection(vertex.normal));
			auto binormal = QuantizeFloat32(node.transform.TransformDirection(vertex.binormal));
			auto tangent = QuantizeFloat32(node.transform.TransformDirection(vertex.tangent));
			auto uv = QuantizeFloat32(vertex.uv1);
			auto color = QuantizeUnorm8(vertex.color);

			writer.Write(position.data(), sizeof(float) * 3);
			writer.Write(normal.data(), sizeof(float) * 3);
			writer.Write(binormal.data(), sizeof(float) * 3);
			writer.Write(tangent.data(), sizeof(float) * 3);
			writer.Write(uv.data(), sizeof(float) * 2);
			writer.Write(color.data(), sizeof(uint8_t) * 4);
		}
	}

	writer.Write(faceCount);

	int32_t faceOffset = 0;
	for (auto& node : model.nodes)
	{
		if (node.mesh == nullptr)
		{
			continue;
		}

		auto& mesh = *node.mesh;

		for (auto& face : mesh.faces)
		{
			writer.Write(faceOffset + face.indices[0]);
			writer.Write(faceOffset + face.indices[1]);
			writer.Write(faceOffset + face.indices[2]);
		}

		faceOffset += static_cast<int32_t>(mesh.vertices.size());
	}

	return true;
}

}
