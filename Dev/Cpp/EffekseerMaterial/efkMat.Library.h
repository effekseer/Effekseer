
#pragma once

#include "efkMat.Base.h"
#include "efkMat.Models.h"
#include "efkMat.Parameters.h"

namespace EffekseerMaterial
{

class LibraryContentBase
{
public:
	std::string Name;
	std::string Description;
	std::vector<std::string> Group;
	bool IsShown = true;

	virtual std::shared_ptr<NodeParameter> Create() { return nullptr; }
};

template <class NT> class LibraryContent : public LibraryContentBase
{
public:
	LibraryContent()
	{
		auto node = Create();
		Name = node->TypeName;
		Description = node->Description;
		Group = node->Group;

		if (node->Type == NodeType::Output)
		{
			IsShown = false;
		}
	}

	std::shared_ptr<NodeParameter> Create() override { return std::make_shared<NT>(); }
};

class LibraryContentGroup
{
public:
	std::string Name;
	std::vector<std::shared_ptr<LibraryContentBase>> Contents;
	std::map<std::string, std::shared_ptr<LibraryContentGroup>> Groups;
};

class Library
{

public:
	std::vector<std::shared_ptr<LibraryContentBase>> Contents;

	std::shared_ptr<LibraryContentGroup> Root;

	void MakeGroups();

	std::shared_ptr<LibraryContentBase> FindContentWithTypeName(const char* name)
	{
		auto key = std::string(name);

		for (auto content : Contents)
		{
			if (content->Name == key)
				return content;
		}

		return std::shared_ptr<LibraryContentBase>();
	}

	Library();
	virtual ~Library();
};

} // namespace EffekseerMaterial