#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "EfkRes.GLTFLoader.h"
#include "EfkRes.Utils.h"
#include <tiny_gltf.h>

namespace efkres
{

namespace
{

struct Attribute
{
	const uint8_t* data = nullptr;
	size_t count = 0;
	size_t byteStride = 0;
	int dataType = 0;
	int componentType = 0;
	operator bool() const { return data != nullptr; }
};

double ReadElement(float value) { return value; }
double ReadElement(uint8_t value) { return std::max(-1.0, static_cast<double>(value) / std::numeric_limits<uint8_t>::max()); }
double ReadElement(uint16_t value) { return std::max(-1.0, static_cast<double>(value) / std::numeric_limits<uint16_t>::max()); }

template <class T> void ReadVertexData(std::vector<MeshVertex>& vertices, size_t offset, const Attribute& attribute)
{
	uint8_t* vertexBuffer = reinterpret_cast<uint8_t*>(vertices.data()) + offset;
	size_t byteStride = (attribute.byteStride > 0) ? attribute.byteStride : (sizeof(T) * (size_t)attribute.dataType);
	for (size_t vertexOffset = 0; vertexOffset < attribute.count; vertexOffset++)
	{
		double* dstBuffer = reinterpret_cast<double*>(vertexBuffer + sizeof(MeshVertex) * vertexOffset);
		const T* srcBuffer = reinterpret_cast<const T*>(attribute.data + byteStride * vertexOffset);

		for (size_t elementIndex = 0; elementIndex < (size_t)attribute.dataType; elementIndex++)
		{
			dstBuffer[elementIndex] = ReadElement(srcBuffer[elementIndex]);
		}
	}
}

void ReadVertexData(std::vector<MeshVertex>& vertices, size_t offset, const Attribute& attribute)
{
	switch (attribute.componentType)
	{
	case TINYGLTF_COMPONENT_TYPE_FLOAT:
		ReadVertexData<float>(vertices, offset, attribute);
		break;
	case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
		ReadVertexData<uint8_t>(vertices, offset, attribute);
		break;
	case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
		ReadVertexData<uint16_t>(vertices, offset, attribute);
		break;
	}
}

template <class T> void ReadIndexData(std::vector<MeshFace>& faces, const uint8_t* indexData, size_t indexCount)
{
	const T* indices = reinterpret_cast<const T*>(indexData);
	for (size_t indexOffset = 0; indexOffset < indexCount; indexOffset += 3)
	{
		MeshFace& face = faces[indexOffset / 3];
		face.indices[0] = static_cast<int32_t>(indices[indexOffset + 2]);
		face.indices[1] = static_cast<int32_t>(indices[indexOffset + 1]);
		face.indices[2] = static_cast<int32_t>(indices[indexOffset + 0]);
	}
}

void ReadIndexData(std::vector<MeshFace>& faces, const uint8_t* indexData, size_t indexCount, int componentType)
{
	switch (componentType)
	{
	case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
		ReadIndexData<uint8_t>(faces, indexData, indexCount);
		break;
	case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
		ReadIndexData<uint16_t>(faces, indexData, indexCount);
		break;
	case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
		ReadIndexData<uint32_t>(faces, indexData, indexCount);
		break;
	}
}

Mat43 ConvertMatrix(const std::vector<double>& src)
{
	if (src.size() == 0)
	{
		return Mat43::Identity();
	}
	else
	{
		assert(src.size() >= 12);
		Mat43 dst;
		memcpy(&dst.v, src.data(), sizeof(dst.v));
		return dst;
	}
}

} // namespace

std::optional<Model> GLTFLoader::LoadModel(std::string_view filepath)
{
	std::string_view fileExt = filepath.substr(filepath.find_last_of('.'));

	tinygltf::TinyGLTF gltfLoader;
	tinygltf::Model gltfModel;
	std::string gltfError, gltfWarn;
	bool loadResult = false;

	if (fileExt == ".gltf")
	{
		loadResult = gltfLoader.LoadASCIIFromFile(&gltfModel, &gltfError, &gltfWarn, std::string(filepath));
	}
	else if (fileExt == ".glb")
	{
		loadResult = gltfLoader.LoadBinaryFromFile(&gltfModel, &gltfError, &gltfWarn, std::string(filepath));
	}
	if (!loadResult)
	{
		return std::nullopt;
	}

	Model model;
	model.meshes.resize(gltfModel.meshes.size());

	for (size_t meshIndex = 0; meshIndex < gltfModel.meshes.size(); meshIndex++)
	{
		auto& dstMesh = model.meshes[meshIndex];
		auto& srcMesh = gltfModel.meshes[meshIndex];

		for (auto& srcPrims : srcMesh.primitives)
		{
			{
				auto& indexAccessor = gltfModel.accessors[srcPrims.indices];
				auto& indexView = gltfModel.bufferViews[indexAccessor.bufferView];
				auto& indexBuffer = gltfModel.buffers[indexView.buffer];
				const uint8_t* indexData = &indexBuffer.data[indexView.byteOffset + indexAccessor.byteOffset];

				dstMesh.faces.resize(indexAccessor.count / 3);
				ReadIndexData(dstMesh.faces, indexData, indexAccessor.count, indexAccessor.componentType);
			}

			auto findAttribute = [&](const char* attribName) -> Attribute
			{
				if (auto it = srcPrims.attributes.find(attribName); it != srcPrims.attributes.end())
				{
					auto& attribAccessor = gltfModel.accessors[it->second];
					auto& attribView = gltfModel.bufferViews[attribAccessor.bufferView];
					auto& attribBuffer = gltfModel.buffers[attribView.buffer];
					const uint8_t* attribData = &attribBuffer.data[attribView.byteOffset + attribAccessor.byteOffset];
					return {attribData, attribAccessor.count, attribView.byteStride, attribAccessor.type, attribAccessor.componentType};
				}
				return {};
			};

			if (auto attribPosition = findAttribute("POSITION"); attribPosition.count > 0)
			{
				dstMesh.vertices.resize(attribPosition.count);
				ReadVertexData(dstMesh.vertices, offsetof(MeshVertex, position), attribPosition);
			}
			else
			{
				continue;
			}

			if (auto attribColor = findAttribute("COLOR_0"))
			{
				ReadVertexData(dstMesh.vertices, offsetof(MeshVertex, color), attribColor);
			}

			if (auto attribUV1 = findAttribute("TEXCOORD_0"))
			{
				ReadVertexData(dstMesh.vertices, offsetof(MeshVertex, uv1), attribUV1);
			}

			if (auto attribUV2 = findAttribute("TEXCOORD_1"))
			{
				ReadVertexData(dstMesh.vertices, offsetof(MeshVertex, uv2), attribUV2);
			}

			if (auto attribNormal = findAttribute("NORMAL"))
			{
				ReadVertexData(dstMesh.vertices, offsetof(MeshVertex, normal), attribNormal);
			}

			if (auto attribTangent = findAttribute("TANGENT"))
			{
				ReadVertexData(dstMesh.vertices, offsetof(MeshVertex, tangent), attribTangent);

				for (auto& vertex : dstMesh.vertices)
				{
					double sign = vertex.binormal.x; // Overrun value by tangent
					vertex.binormal = CrossProduct(vertex.normal, vertex.tangent).Normalized() * sign;
				}
			}
			else
			{
				for (auto& face : dstMesh.faces)
				{
					MeshVertex& v1 = dstMesh.vertices[face.indices[0]];
					MeshVertex& v2 = dstMesh.vertices[face.indices[1]];
					MeshVertex& v3 = dstMesh.vertices[face.indices[2]];
					CalcTangentSpace(v1, v2, v3);
				}
			}
		}
	}

	model.nodes.resize(gltfModel.nodes.size());
	for (size_t nodeIndex = 0; nodeIndex < gltfModel.nodes.size(); nodeIndex++)
	{
		auto& dstNode = model.nodes[nodeIndex];
		auto& srcNode = gltfModel.nodes[nodeIndex];

		if (srcNode.mesh >= 0)
		{
			dstNode.mesh = &model.meshes[srcNode.mesh];
			dstNode.transform = ConvertMatrix(srcNode.matrix);
		}
	}

	return std::move(model);
}

} // namespace efkres
