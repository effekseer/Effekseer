
#include "fbxToEfkMdl.Base.h"

namespace fbxToEfkMdl
{
	class FBXConverter
	{
		struct FbxFace
		{
			std::vector<Vertex> Vertecies;
		};

		struct VertexNormals
		{
			int32_t Count = 0;
			FbxVector4 Binormal;
			FbxVector4 Tangent;
		};

	private:
		void CalcTangentSpace(const Vertex& v1, const Vertex& v2, const Vertex& v3, FbxVector4& binormal, FbxVector4& tangent);

		FbxVector4 LoadPosition(FbxMesh* fbxMesh, int32_t ctrlPointIndex);
		FbxVector4 LoadNormal(FbxLayerElementNormal* normals, int32_t vertexID, int32_t ctrlPointIndex);
		FbxVector2 LoadUV(FbxMesh* fbxMesh, FbxLayerElementUV* uvs, int32_t vertexID, int32_t ctrlPointIndex, int32_t polygonIndex, int32_t polygonPointIndex);
		FbxColor LoadVertexColor(FbxMesh* fbxMesh, FbxLayerElementVertexColor* colors, int32_t vertexID, int32_t ctrlPointIndex, int32_t polygonIndex, int32_t polygonPointIndex);

		std::shared_ptr<Mesh> LoadMesh(FbxMesh* fbxMesh);

		std::shared_ptr<Node> LoadHierarchy(std::shared_ptr<Node> parent, FbxNode* fbxNode, FbxManager* fbxManager);

	public:

		std::shared_ptr<Scene> LoadScene(FbxScene* fbxScene, FbxManager* fbxManager);
	};
}