
#pragma once

#include "imgui_node_editor.h"
namespace ed = ax::NodeEditor;

#include "Graphics/efkMat.Graphics.h"

#include <chrono>
#include <efkMat.Base.h>
#include <efkMat.Library.h>
#include <efkMat.Models.h>
#include <efkMat.Parameters.h>

namespace EffekseerMaterial
{
class NodeUserDataObject : public EffekseerMaterial::UserObject
{
private:
	std::shared_ptr<Preview> preview;

public:
	NodeUserDataObject();
	virtual ~NodeUserDataObject();

	bool IsDirtied = true;

	std::shared_ptr<Preview>& GetPreview() { return preview; }
};

class Editor
{
private:
	std::shared_ptr<EffekseerMaterial::Graphics> graphics_;
	std::chrono::system_clock::time_point startingTime;
	bool isLoading = false;

public: // TODO temp
	std::shared_ptr<Material> material;
	std::shared_ptr<Library> library;
	std::shared_ptr<Preview> preview_;

	ImVec2 popupPosition;
	ed::NodeId currentNodeID = 0;
	ed::LinkId currentLinkID = 0;
	std::shared_ptr<Pin> currentPin;

	static bool InputText(const char* label, std::string& text);

public:
	Editor(std::shared_ptr<EffekseerMaterial::Graphics> graphics);
	virtual ~Editor();

	void Save();

	void Load();

	void Update();

	void UpdateNodes();

	void UpdatePopup();

	void UpdateCreating();

	void UpdateDeleting();

	void UpdateParameterEditor(std::shared_ptr<Node> node);

	void UpdatePreview();

	void UpdateNode(std::shared_ptr<Node> node);

	void UpdateLink(std::shared_ptr<Link> link);
};
} // namespace EffekseerMaterial
