
#include "Utils.h"

namespace fbxToEfkMdl
{

void CalculateAllGlobalMateixes(std::vector<NodeState>& nodes)
{
	for (auto& node : nodes)
	{
		for (auto& node2 : nodes)
		{
			if (node2.TargetNode == node.ParentNode)
			{
				node.MatGlobal = node2.MatGlobal * node.MatLocal;
			}
		}
	}
}

void AssignAllDefaultGlobalMateixes(std::vector<NodeState>& nodes)
{
	for (auto& node : nodes)
	{
		node.AssignDefaultValues();
	}

	for (auto& node : nodes)
	{
		node.CalculateLocalMatrix();
		node.MatGlobal.SetIdentity();
	}

	CalculateAllGlobalMateixes(nodes);

	for (auto& node : nodes)
	{
		node.MatGlobalDefault = node.MatGlobal;
	}
}

std::vector<MeshState> GetAllMeshes(std::shared_ptr<Node> node)
{
	std::vector<MeshState> ret;

	if (node->MeshData != nullptr)
	{
		MeshState m;
		m.Target = node->MeshData;
		m.MeshNode = node;

		for (auto b : m.Target->BoneConnectors)
		{
			BoneConnectorState bs;
			bs.NodeIndex = -1;
			m.Connectors.push_back(bs);
		}

		ret.push_back(m);
	}

	for (auto c : node->Children)
	{
		auto ms = GetAllMeshes(c);

		for (auto m : ms)
		{
			ret.push_back(m);
		}
	}

	return ret;
}

std::vector<NodeState> GetAllNodes(std::shared_ptr<Node> node, std::shared_ptr<Node> parentNode)
{
	std::vector<NodeState> ret;
	ret.push_back(NodeState());
	(ret.end() - 1)->ParentNode = parentNode;
	(ret.end() - 1)->TargetNode = node;

	for (auto c : node->Children)
	{
		auto ms = GetAllNodes(c, node);

		for (auto m : ms)
		{
			ret.push_back(m);
		}
	}

	return ret;
}

} // namespace fbxToEfkMdl