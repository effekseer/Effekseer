#include "EfkRes.EfkModelSaver.h"
#include "EfkRes.Utils.h"
#include <algorithm>

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

std::array<float, 2> QuantizeFloat32(Vec2 v) { return {static_cast<float>(v.x), static_cast<float>(v.y)}; }

std::array<float, 3> QuantizeFloat32(Vec3 v) { return {static_cast<float>(v.x), static_cast<float>(v.y), static_cast<float>(v.z)}; }

template <class T> T QuantizeIntValue(double value)
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

std::array<uint16_t, 2> QuantizeUnorm16(Vec2 v)
{
	return {
		QuantizeIntValue<uint16_t>(v.x),
		QuantizeIntValue<uint16_t>(v.y),
	};
}

void SaveFrame(BinaryWriter& writer, const Model& model, int32_t version, float modelScale)
{
	int32_t vertexCount = 0;
	int32_t faceCount = 0;

	for (auto& node : model.nodes)
	{
		if (node.mesh == nullptr)
		{
			continue;
		}

		auto& mesh = *node.mesh;
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
		Mat43 transform = mesh.positionsAreLocal ? node.transform : Mat43::Identity();

		if (version <= 5)
		{
			for (auto& vertex : mesh.vertices)
			{
				auto position = QuantizeFloat32(transform.TransformPosition(vertex.position * modelScale));
				auto normal = QuantizeFloat32(transform.TransformDirection(vertex.normal));
				auto binormal = QuantizeFloat32(transform.TransformDirection(vertex.binormal));
				auto tangent = QuantizeFloat32(transform.TransformDirection(vertex.tangent));
				auto uv = QuantizeFloat32(vertex.uv1);
				auto color = QuantizeUnorm8(vertex.color);

				writer.Write(position);
				writer.Write(normal);
				writer.Write(binormal);
				writer.Write(tangent);
				writer.Write(uv);
				writer.Write(color);
			}
		}
		else if (version >= 6)
		{
			for (auto& vertex : mesh.vertices)
			{
				auto position = QuantizeFloat32(transform.TransformPosition(vertex.position * modelScale));
				auto normal = QuantizeFloat32(transform.TransformDirection(vertex.normal));
				auto binormal = QuantizeFloat32(transform.TransformDirection(vertex.binormal));
				auto tangent = QuantizeFloat32(transform.TransformDirection(vertex.tangent));
				auto uv1 = QuantizeFloat32(vertex.uv1);
				auto uv2 = QuantizeFloat32(vertex.uv2);
				auto color = QuantizeUnorm8(vertex.color);

				writer.Write(position);
				writer.Write(normal);
				writer.Write(binormal);
				writer.Write(tangent);
				writer.Write(uv1);
				writer.Write(uv2);
				writer.Write(color);
			}
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
}

bool SaveModels(std::string_view filepath, const std::vector<const Model*>& frames, double scale)
{
	if (frames.empty())
	{
		return false;
	}

	const int32_t version = 6;
	float modelScale = static_cast<float>(scale);
	int32_t modelCount = 1;
	int32_t frameCount = static_cast<int32_t>(frames.size());

	BinaryWriter writer;
	if (!writer.Open(std::string(filepath).c_str()))
	{
		return false;
	}

	writer.Write(version);
	writer.Write(modelScale);
	writer.Write(modelCount);
	writer.Write(frameCount);

	for (auto frame : frames)
	{
		SaveFrame(writer, *frame, version, modelScale);
	}

	return true;
}

} // namespace

bool EfkModelSaver::Save(std::string_view filepath, const Model& model)
{
	return SaveModels(filepath, std::vector<const Model*>{&model}, m_scale);
}

bool EfkModelSaver::Save(std::string_view filepath, const std::vector<Model>& frames)
{
	std::vector<const Model*> framePtrs;
	framePtrs.reserve(frames.size());
	for (auto& frame : frames)
	{
		framePtrs.push_back(&frame);
	}
	return SaveModels(filepath, framePtrs, m_scale);
}

} // namespace efkres
