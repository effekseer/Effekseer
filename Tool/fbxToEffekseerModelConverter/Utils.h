
#pragma once

#include "fbxToEfkMdl.Base.h"

namespace fbxToEfkMdl
{

void CalculateAllGlobalMateixes(std::vector<NodeState>& nodes);

void AssignAllDefaultGlobalMateixes(std::vector<NodeState>& nodes);

std::vector<MeshState> GetAllMeshes(std::shared_ptr<Node> node);

std::vector<NodeState> GetAllNodes(std::shared_ptr<Node> node, std::shared_ptr<Node> parentNode);

} // namespace fbxToEfkMdl