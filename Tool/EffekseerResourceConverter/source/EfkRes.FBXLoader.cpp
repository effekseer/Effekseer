#include <string.h>
#include <algorithm>
#include <ufbx.h>
#include "EfkRes.Utils.h"
#include "EfkRes.FBXLoader.h"

namespace efkres
{

namespace
{

Vec2 Convert(ufbx_vec2 v)
{
    return { v.x, v.y };
}

Vec3 Convert(ufbx_vec3 v)
{
    return { v.x, v.y, v.z };
}

Vec4 Convert(ufbx_vec4 v)
{
    return { v.x, v.y, v.z, v.w };
}

Mat43 Convert(ufbx_matrix src)
{
    Mat43 dst;
    memcpy(&dst, &src, sizeof(dst));
    return dst;
}

}

std::optional<Model> FBXLoader::LoadModel(std::string_view filepath)
{
    ufbx_load_opts opts{};
    ufbx_error error{};
    ufbx_scene* scene = ufbx_load_file(ufbx_string_view(filepath.data(), filepath.size()), &opts, &error);
    if (!scene)
    {
        return std::nullopt;
    }

    Model model;
    model.meshes.resize(scene->meshes.count);

    for (size_t meshIndex = 0; meshIndex < scene->meshes.count; meshIndex++)
    {
        Mesh& dstMesh = model.meshes[meshIndex];
        const ufbx_mesh* srcMesh = scene->meshes[meshIndex];
        
        if (srcMesh->vertex_position.exists)
        {
            size_t numVertices = srcMesh->vertex_position.indices.count;
            dstMesh.vertices.resize(numVertices);

            const ufbx_vec3* fbxPositionValues = srcMesh->vertex_position.values.data;
            const uint32_t* fbxPositionIndices = srcMesh->vertex_position.indices.data;
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
            size_t numVertices = srcMesh->vertex_color.indices.count;
            const ufbx_vec4* fbxColorValues = srcMesh->vertex_color.values.data;
            const uint32_t* fbxColorIndices = srcMesh->vertex_color.indices.data;
            for (size_t vertexIndex = 0; vertexIndex < numVertices; vertexIndex++)
            {
                dstMesh.vertices[vertexIndex].color = Convert(fbxColorValues[fbxColorIndices[vertexIndex]]);
            }
        }

        if (srcMesh->vertex_uv.exists)
        {
            size_t numVertices = srcMesh->vertex_uv.indices.count;
            const ufbx_vec2* fbxUV1Values = srcMesh->vertex_uv.values.data;
            const uint32_t* fbxUV1Indices = srcMesh->vertex_uv.indices.data;
            for (size_t vertexIndex = 0; vertexIndex < numVertices; vertexIndex++)
            {
                dstMesh.vertices[vertexIndex].uv1 = Convert(fbxUV1Values[fbxUV1Indices[vertexIndex]]);
            }
        }

        if (srcMesh->uv_sets.count >= 2)
        {
            size_t numVertices = srcMesh->uv_sets[1].vertex_uv.indices.count;
            const ufbx_vec2* fbxUV2Values = srcMesh->uv_sets[1].vertex_uv.values.data;
            const uint32_t* fbxUV2Indices = srcMesh->uv_sets[1].vertex_uv.indices.data;
            for (size_t vertexIndex = 0; vertexIndex < numVertices; vertexIndex++)
            {
                dstMesh.vertices[vertexIndex].uv2 = Convert(fbxUV2Values[fbxUV2Indices[vertexIndex]]);
            }
        }

        if (srcMesh->vertex_normal.exists)
        {
            size_t numVertices = srcMesh->vertex_normal.indices.count;
            const ufbx_vec3* fbxNormalValues = srcMesh->vertex_normal.values.data;
            const uint32_t* fbxNormalIndices = srcMesh->vertex_normal.indices.data;
            for (size_t vertexIndex = 0; vertexIndex < numVertices; vertexIndex++)
            {
                dstMesh.vertices[vertexIndex].normal = Convert(fbxNormalValues[fbxNormalIndices[vertexIndex]]).Normalized();
            }
        }

        if (srcMesh->vertex_tangent.exists)
        {
            size_t numVertices = srcMesh->vertex_tangent.indices.count;
            const ufbx_vec3* fbxTangentValues = srcMesh->vertex_tangent.values.data;
            const uint32_t* fbxTangentIndices = srcMesh->vertex_tangent.indices.data;
            for (size_t vertexIndex = 0; vertexIndex < numVertices; vertexIndex++)
            {
                dstMesh.vertices[vertexIndex].tangent = Convert(fbxTangentValues[fbxTangentIndices[vertexIndex]]).Normalized();
            }
        }

        if (srcMesh->vertex_bitangent.exists)
        {
            size_t numVertices = srcMesh->vertex_bitangent.indices.count;
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

        if (srcMesh->vertex_normal.exists && srcMesh->vertex_tangent.exists && !srcMesh->vertex_bitangent.exists)
        {
            for (auto& vertex : dstMesh.vertices)
            {
                vertex.binormal = CrossProduct(vertex.normal, vertex.tangent).Normalized();
            }
        }
        else if (srcMesh->vertex_normal.exists && !srcMesh->vertex_tangent.exists && !srcMesh->vertex_bitangent.exists)
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
            auto meshIndex = std::distance(scene->meshes.begin(), it);
            dstNode.mesh = &model.meshes[meshIndex];
        }
    }

    ufbx_free_scene(scene);
    return std::move(model);
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

}
