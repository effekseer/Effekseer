
#pragma once

#include "imgui_node_editor.h"
namespace ed = ax::NodeEditor;

#include "Graphics/efkMat.Graphics.h"

#include <chrono>
#include <efkMat.Base.h>
#include <efkMat.Library.h>
#include <efkMat.Models.h>
#include <efkMat.Parameters.h>

#include "../IPC/IPC.h"

namespace EffekseerMaterial
{

class Editor;

class NodeUserDataObject : public EffekseerMaterial::UserObject
{
private:
	std::shared_ptr<Preview> preview;

public:
	NodeUserDataObject();
	virtual ~NodeUserDataObject();

	bool IsDirtied = true;

	std::shared_ptr<Preview>& GetPreview()
	{
		return preview;
	}
};

class EditorContent
{
private:
	Editor* editor_ = nullptr;
	std::string path_;
	std::string name_;

	std::shared_ptr<Material> material_;
	ed::EditorContext* editorContext_;
	bool hasStorageRef_ = false;
	uint64_t previousChangedID_ = 0;

public:
	//! immediately after loaded
	bool IsLoading = true;

	EditorContent(Editor* editor);

	virtual ~EditorContent();

	bool Save();

	void SaveAs(const char* path);

	ErrorCode Load(const char* path, std::shared_ptr<Library> library);

	void UpdateBinary();

	void UpdatePath(const char* path);

	std::shared_ptr<Material> GetMaterial();

	std::string GetName();

	std::string GetPath();

	bool GetIsChanged();

	void ClearIsChanged();

	ed::EditorContext* GetEditorContext()
	{
		return editorContext_;
	}

	//! this content will be closed and will show a dialog. This flag is true, dialog is shown and make the flag false
	bool WillShowClosingDialog = false;
	bool IsClosing = false;
};

class Editor
{
private:
	std::shared_ptr<EffekseerMaterial::Graphics> graphics_;
	std::chrono::system_clock::time_point startingTime;

	int32_t selectedContentInd_ = -1;
	std::vector<std::shared_ptr<EditorContent>> contents_;

	//! is selected changed and not applied to gui yet
	bool isSelectedDirty_ = false;

	//! popupsed keywords
	bool isJustNewNodePanelOpened_ = false;
	std::array<char, 256> searchingKeywords;
	std::array<char, 256> searchingKeywordsActual;

	std::vector<std::shared_ptr<Texture>> previewTypeButtons_;

public: // TODO temp
	std::shared_ptr<Library> library;
	std::shared_ptr<Preview> preview_;
	int32_t previewTextureCount_ = 0;
	int32_t previewUniformCount_ = 0;
	int32_t previewGradientCount_ = 0;

	std::shared_ptr<IPC::KeyValueFileStorage> keyValueFileStorage_;

	ImVec2 popupPosition;
	ed::NodeId currentNodeID = 0;
	ed::LinkId currentLinkID = 0;
	std::shared_ptr<Pin> currentPin;

	static bool InputText(const char* label, std::string& text);

	std::shared_ptr<Material> GetSelectedMaterial();

public:
	Editor(std::shared_ptr<EffekseerMaterial::Graphics> graphics);
	virtual ~Editor();

	void New();

	void SaveAs(const char* path);

	void SaveAs();

	ErrorCode Load(const char* path);

	bool Load();

	/**
		@brief	if a file is not loaded, open the file, otherwise select the file
	*/
	ErrorCode LoadOrSelect(const char* path);

	void Save();

	void CloseContents();

	void Update();

	void UpdateNodes();

	void UpdatePopup();

	void UpdateCreating();

	void UpdateDeleting();

	void UpdateParameterEditor(std::shared_ptr<Node> node);

	void UpdatePreview();

	/**
		@brief	to record positions of nodes to realize undo. Moving positions are caused on View and multiple nodes are moved simultaniously.
	*/
	void UpdateToRecordMovingCommand();

	void UpdateNode(std::shared_ptr<Node> node);

	void UpdateLink(std::shared_ptr<Link> link);

	std::vector<std::shared_ptr<EditorContent>> GetContents()
	{
		return contents_;
	}

	int32_t GetSelectedContentIndex() const
	{
		return selectedContentInd_;
	}
	void SelectContent(int32_t index)
	{
		if (selectedContentInd_ == index)
			return;

		selectedContentInd_ = index;
		isSelectedDirty_ = true;
	}

	bool GetIsSelectedDirty();

	void ClearDirtiedSelectedFlags();
};
} // namespace EffekseerMaterial
