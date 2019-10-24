#include "efkMat.Library.h"

namespace EffekseerMaterial
{

Library::Library()
{
	
	
	Contents.push_back(std::make_shared<LibraryContent<NodeOutput>>());

	// Math
	Contents.push_back(std::make_shared<LibraryContent<NodeAdd>>());
	Contents.push_back(std::make_shared<LibraryContent<NodeSubtract>>());
	Contents.push_back(std::make_shared<LibraryContent<NodeMultiply>>());
	Contents.push_back(std::make_shared<LibraryContent<NodeDivide>>());
	Contents.push_back(std::make_shared<LibraryContent<NodeFmod>>());
	Contents.push_back(std::make_shared<LibraryContent<NodeCeil>>());
	Contents.push_back(std::make_shared<LibraryContent<NodeFloor>>());
	Contents.push_back(std::make_shared<LibraryContent<NodeFrac>>());
	Contents.push_back(std::make_shared<LibraryContent<NodeAbs>>());
	Contents.push_back(std::make_shared<LibraryContent<NodeComponentMask>>());
	Contents.push_back(std::make_shared<LibraryContent<NodeAppendVector>>());
	Contents.push_back(std::make_shared<LibraryContent<NodeOneMinus>>());
	Contents.push_back(std::make_shared<LibraryContent<NodeSine>>());

	Contents.push_back(std::make_shared<LibraryContent<NodeTextureCoordinate>>());
	Contents.push_back(std::make_shared<LibraryContent<NodePanner>>());

	Contents.push_back(std::make_shared<LibraryContent<NodeSampleTexture>>());
	Contents.push_back(std::make_shared<LibraryContent<NodeTextureObjectParameter>>());
	Contents.push_back(std::make_shared<LibraryContent<NodeTextureObject>>());

	Contents.push_back(std::make_shared<LibraryContent<NodeVertexNormalWS>>());
	Contents.push_back(std::make_shared<LibraryContent<NodePixelNormalWS>>());

#ifdef _DEBUG
	Contents.push_back(std::make_shared<LibraryContent<NodeVertexTangentWS>>());
#endif
	Contents.push_back(std::make_shared<LibraryContent<NodeCustomData1>>());
	Contents.push_back(std::make_shared<LibraryContent<NodeCustomData2>>());

	// Constant
	Contents.push_back(std::make_shared<LibraryContent<NodeTime>>());
	Contents.push_back(std::make_shared<LibraryContent<NodeConstant1>>());
	Contents.push_back(std::make_shared<LibraryContent<NodeConstant2>>());
	Contents.push_back(std::make_shared<LibraryContent<NodeConstant3>>());
	Contents.push_back(std::make_shared<LibraryContent<NodeConstant4>>());

	// Parameter
	Contents.push_back(std::make_shared<LibraryContent<NodeParameter1>>());
	Contents.push_back(std::make_shared<LibraryContent<NodeParameter4>>());

	// Comment
	Contents.push_back(std::make_shared<LibraryContent<NodeComment>>());

	MakeGroups();
}

Library::~Library() {}

void Library::MakeGroups()
{
	// TODO recurcive
	Root = std::make_shared<LibraryContentGroup>();

	for (auto content : Contents)
	{
		if (content->Group.size() == 0)
		{
			Root->Contents.push_back(content);
			continue;
		}

		if (Root->Groups.count(content->Group[0]) == 0)
		{
			Root->Groups[content->Group[0]] = std::make_shared<LibraryContentGroup>();
			Root->Groups[content->Group[0]]->Name = content->Group[0];
		}

		Root->Groups[content->Group[0]]->Contents.push_back(content);
	}
}

} // namespace EffekseerMaterial