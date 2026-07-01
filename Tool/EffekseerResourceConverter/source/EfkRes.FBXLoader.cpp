#include "EfkRes.FBXLoader.h"
#include "EfkRes.Utils.h"
#include <algorithm>
#include <cmath>
#include <string.h>
#include <ufbx.h>

namespace efkres
{

namespace
{

Vec2 Convert(ufbx_vec2 v) { return {v.x, v.y}; }

Vec2 ConvertUV(ufbx_vec2 v) { return {v.x, 1.0 - v.y}; }

Vec2 GenerateUV(Vec3 position) { return {position.x + position.z, position.y}; }

Vec3 Convert(ufbx_vec3 v) { return {v.x, v.y, v.z}; }

Vec4 Convert(ufbx_vec4 v) { return {v.x, v.y, v.z, v.w}; }

Mat43 Convert(ufbx_matrix src)
{
	Mat43 dst;
	memcpy(&dst, &src, sizeof(dst));
	return dst;
}

std::optional<Model> LoadModelFromScene(const ufbx_scene* scene)
{
	Model model;
	model.meshes.resize(scene->meshes.count);

	for (size_t meshIndex = 0; meshIndex < scene->meshes.count; meshIndex++)
	{
		Mesh& dstMesh = model.meshes[meshIndex];
		const ufbx_mesh* srcMesh = scene->meshes[meshIndex];
		const ufbx_vertex_vec3& positionAttr = srcMesh->skinned_position.exists ? srcMesh->skinned_position : srcMesh->vertex_position;
		dstMesh.positionsAreLocal = !(srcMesh->skinned_position.exists && !srcMesh->skinned_is_local);

		if (positionAttr.exists)
		{
			size_t numVertices = positionAttr.indices.count;
			dstMesh.vertices.resize(numVertices);

			const ufbx_vec3* fbxPositionValues = positionAttr.values.data;
			const uint32_t* fbxPositionIndices = positionAttr.indices.data;
			for (size_t vertexIndex = 0; vertexIndex < numVertices; vertexIndex++)
			{
				dstMesh.vertices[vertexIndex].position = Convert(fbxPositionValues[fbxPositionIndices[vertexIndex]]);
			}
		}
		else
		{
			continue;
		}

		if (srcMesh->vertex_color.exists)
		{
			size_t numVertices = std::min(srcMesh->vertex_color.indices.count, dstMesh.vertices.size());
			const ufbx_vec4* fbxColorValues = srcMesh->vertex_color.values.data;
			const uint32_t* fbxColorIndices = srcMesh->vertex_color.indices.data;
			for (size_t vertexIndex = 0; vertexIndex < numVertices; vertexIndex++)
			{
				dstMesh.vertices[vertexIndex].color = Convert(fbxColorValues[fbxColorIndices[vertexIndex]]);
			}
		}

		if (srcMesh->vertex_uv.exists)
		{
			size_t numVertices = std::min(srcMesh->vertex_uv.indices.count, dstMesh.vertices.size());
			const ufbx_vec2* fbxUV1Values = srcMesh->vertex_uv.values.data;
			const uint32_t* fbxUV1Indices = srcMesh->vertex_uv.indices.data;
			for (size_t vertexIndex = 0; vertexIndex < numVertices; vertexIndex++)
			{
				dstMesh.vertices[vertexIndex].uv1 = ConvertUV(fbxUV1Values[fbxUV1Indices[vertexIndex]]);
			}
		}
		else
		{
			for (auto& vertex : dstMesh.vertices)
			{
				vertex.uv1 = GenerateUV(vertex.position);
			}
		}

		if (srcMesh->uv_sets.count >= 2 && srcMesh->uv_sets[1].vertex_uv.exists)
		{
			size_t numVertices = std::min(srcMesh->uv_sets[1].vertex_uv.indices.count, dstMesh.vertices.size());
			const ufbx_vec2* fbxUV2Values = srcMesh->uv_sets[1].vertex_uv.values.data;
			const uint32_t* fbxUV2Indices = srcMesh->uv_sets[1].vertex_uv.indices.data;
			for (size_t vertexIndex = 0; vertexIndex < numVertices; vertexIndex++)
			{
				dstMesh.vertices[vertexIndex].uv2 = ConvertUV(fbxUV2Values[fbxUV2Indices[vertexIndex]]);
			}
		}
		else
		{
			for (auto& vertex : dstMesh.vertices)
			{
				vertex.uv2 = vertex.uv1;
			}
		}

		const ufbx_vertex_vec3& normalAttr = srcMesh->skinned_normal.exists ? srcMesh->skinned_normal : srcMesh->vertex_normal;
		const bool hasNormal = normalAttr.exists;
		const bool hasTangent = srcMesh->vertex_tangent.exists && dstMesh.positionsAreLocal;
		const bool hasBitangent = srcMesh->vertex_bitangent.exists && dstMesh.positionsAreLocal;

		if (hasNormal)
		{
			size_t numVertices = std::min(normalAttr.indices.count, dstMesh.vertices.size());
			const ufbx_vec3* fbxNormalValues = normalAttr.values.data;
			const uint32_t* fbxNormalIndices = normalAttr.indices.data;
			for (size_t vertexIndex = 0; vertexIndex < numVertices; vertexIndex++)
			{
				dstMesh.vertices[vertexIndex].normal = Convert(fbxNormalValues[fbxNormalIndices[vertexIndex]]).Normalized();
			}
		}

		if (hasTangent)
		{
			size_t numVertices = std::min(srcMesh->vertex_tangent.indices.count, dstMesh.vertices.size());
			const ufbx_vec3* fbxTangentValues = srcMesh->vertex_tangent.values.data;
			const uint32_t* fbxTangentIndices = srcMesh->vertex_tangent.indices.data;
			for (size_t vertexIndex = 0; vertexIndex < numVertices; vertexIndex++)
			{
				dstMesh.vertices[vertexIndex].tangent = Convert(fbxTangentValues[fbxTangentIndices[vertexIndex]]).Normalized();
			}
		}

		if (hasBitangent)
		{
			size_t numVertices = std::min(srcMesh->vertex_bitangent.indices.count, dstMesh.vertices.size());
			const ufbx_vec3* fbxBitangentValues = srcMesh->vertex_bitangent.values.data;
			const uint32_t* fbxBitangentIndices = srcMesh->vertex_bitangent.indices.data;
			for (size_t vertexIndex = 0; vertexIndex < numVertices; vertexIndex++)
			{
				dstMesh.vertices[vertexIndex].binormal = Convert(fbxBitangentValues[fbxBitangentIndices[vertexIndex]]).Normalized();
			}
		}

		std::vector<uint32_t> triangleIndices;
		triangleIndices.resize(srcMesh->max_face_triangles * 3);

		for (size_t faceIndex = 0; faceIndex < srcMesh->num_faces; faceIndex++)
		{
			const ufbx_face& fbxFace = srcMesh->faces[faceIndex];

			uint32_t numTriangles = ufbx_triangulate_face(triangleIndices.data(), triangleIndices.size(), srcMesh, fbxFace);
			for (size_t triangleIndex = 0; triangleIndex < numTriangles; triangleIndex++)
			{
				MeshFace dstFace{};
				dstFace.indices[0] = triangleIndices[triangleIndex * 3 + 2];
				dstFace.indices[1] = triangleIndices[triangleIndex * 3 + 1];
				dstFace.indices[2] = triangleIndices[triangleIndex * 3 + 0];
				dstMesh.faces.emplace_back(dstFace);
			}
		}

		if (hasNormal && hasTangent && !hasBitangent)
		{
			for (auto& vertex : dstMesh.vertices)
			{
				vertex.binormal = CrossProduct(vertex.normal, vertex.tangent).Normalized();
			}
		}
		else if (hasNormal && (!hasTangent || !hasBitangent))
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

	model.nodes.resize(scene->nodes.count);
	for (size_t nodeIndex = 0; nodeIndex < scene->nodes.count; nodeIndex++)
	{
		Node& dstNode = model.nodes[nodeIndex];
		const ufbx_node* srcNode = scene->nodes[nodeIndex];

		dstNode.transform = Convert(srcNode->geometry_to_world);

		if (srcNode->mesh)
		{
			auto it = std::find(scene->meshes.begin(), scene->meshes.end(), srcNode->mesh);
			if (it == scene->meshes.end())
			{
				continue;
			}

			auto meshIndex = std::distance(scene->meshes.begin(), it);
			dstNode.mesh = &model.meshes[meshIndex];
		}
	}

	return std::move(model);
}

} // namespace

std::optional<Model> FBXLoader::LoadModel(std::string_view filepath)
{
	auto sequence = LoadModelSequence(filepath);
	if (!sequence.has_value() || sequence->empty())
	{
		return std::nullopt;
	}

	return std::move(sequence.value()[0]);
}

std::optional<std::vector<Model>> FBXLoader::LoadModelSequence(std::string_view filepath)
{
	ufbx_load_opts opts{};
	opts.evaluate_skinning = true;
	opts.evaluate_caches = true;
	opts.generate_missing_normals = true;
	opts.normalize_normals = true;
	opts.normalize_tangents = true;
	opts.target_axes = ufbx_axes_right_handed_y_up;

	ufbx_error error{};
	ufbx_scene* scene = ufbx_load_file(ufbx_string_view(filepath.data(), filepath.size()), &opts, &error);
	if (!scene)
	{
		return std::nullopt;
	}

	std::vector<Model> models;
	const ufbx_anim_stack* animStack = scene->anim_stacks.count > 0 ? scene->anim_stacks[0] : nullptr;
	const ufbx_anim* anim = animStack != nullptr ? animStack->anim : nullptr;
	const double timeBegin = animStack != nullptr ? animStack->time_begin : 0.0;
	const double timeEnd = animStack != nullptr ? animStack->time_end : 0.0;
	const int32_t frameCount = static_cast<int32_t>(std::round(std::max(0.0, timeEnd - timeBegin) * 60.0));

	if (anim != nullptr && frameCount > 0)
	{
		models.reserve(frameCount);
		ufbx_evaluate_opts evaluateOpts{};
		evaluateOpts.evaluate_skinning = true;
		evaluateOpts.evaluate_caches = true;

		for (int32_t frame = 0; frame < frameCount; frame++)
		{
			double time = timeBegin + static_cast<double>(frame) / 60.0;
			ufbx_scene* evaluatedScene = ufbx_evaluate_scene(scene, anim, time, &evaluateOpts, &error);
			if (evaluatedScene == nullptr)
			{
				ufbx_free_scene(scene);
				return std::nullopt;
			}

			auto model = LoadModelFromScene(evaluatedScene);
			ufbx_free_scene(evaluatedScene);
			if (!model.has_value())
			{
				ufbx_free_scene(scene);
				return std::nullopt;
			}

			models.emplace_back(std::move(model.value()));
		}
	}
	else
	{
		auto model = LoadModelFromScene(scene);
		if (model.has_value())
		{
			models.emplace_back(std::move(model.value()));
		}
	}

	ufbx_free_scene(scene);
	return std::move(models);
}

std::optional<Curve> FBXLoader::LoadCurve(std::string_view filepath)
{
	ufbx_load_opts opts{};
	ufbx_error error{};
	ufbx_scene* scene = ufbx_load_file(ufbx_string_view(filepath.data(), filepath.size()), &opts, &error);
	if (!scene)
	{
		return std::nullopt;
	}

	if (scene->nurbs_curves.count == 0)
	{
		return std::nullopt;
	}

	Curve curve;

	auto srcCurve = scene->nurbs_curves[0];
	for (auto point : srcCurve->control_points)
	{
		curve.controllPoints.push_back(Convert(point));
	}
	for (auto knot : srcCurve->basis.knot_vector)
	{
		curve.knotValues.push_back(knot);
	}

	curve.order = srcCurve->basis.order;
	curve.dimension = srcCurve->basis.is_2d ? 2 : 3;
	curve.step = 16;
	curve.type = 0;

	return curve;
}

} // namespace efkres
