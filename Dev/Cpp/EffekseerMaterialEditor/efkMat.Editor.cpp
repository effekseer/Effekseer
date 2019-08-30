
#include "efkMat.Editor.h"
#include "efkMat.CommandManager.h"
#include "efkMat.Models.h"

#include "ThirdParty/nfd/nfd.h"

#include <fstream>
#include <iostream>
#include <ostream>

#include <Platform/efkMat.GLSL.h>
#include <Platform/efkMat.HLSL.h>

#include <efkMat.StringContainer.h>

#include <filesystem>
namespace fs = std::experimental::filesystem;

namespace EffekseerMaterial
{

NodeUserDataObject::NodeUserDataObject() {}

NodeUserDataObject::~NodeUserDataObject() {}

EditorContent::EditorContent(Editor* editor) : editor_(editor)
{
	material_ = std::make_shared<EffekseerMaterial::Material>();
	material_->Initialize();

	ed::Config config;
	config.SettingsFile = "nodeEditor.json";
	editorContext_ = ed::CreateEditor(&config);
}

EditorContent ::~EditorContent()
{
	ed::DestroyEditor(editorContext_);
	UpdatePath("");
}

bool EditorContent::Save()
{
	if (GetPath() == "")
	{
		nfdchar_t* outPath = NULL;
		nfdresult_t result = NFD_SaveDialog("efkmat", "", &outPath);

		if (result == NFD_OKAY)
		{
			SaveAs(outPath);
			return true;
		}

		return false;
	}
	else
	{
		SaveAs(GetPath().c_str());
		return true;
	}
}

void EditorContent::SaveAs(const char* path)
{
	std::vector<uint8_t> data;
	material_->Save(data, path);

	if (fs::path(path).extension().generic_string() == ".efkmat")
	{
		std::ofstream fout;
		fout.open(std::string(path), std::ios::out | std::ios::binary | std::ios::trunc);

		fout.write((char*)data.data(), data.size());

		fout.close();
	}
	else
	{
		std::ofstream fout;
		fout.open(std::string(path) + ".efkmat", std::ios::out | std::ios::binary | std::ios::trunc);

		fout.write((char*)data.data(), data.size());

		fout.close();
	}

	UpdatePath(path);

	previousChangedID_ = material_->GetCommandManager()->GetHistoryID();
}

bool EditorContent::Load(const char* path, std::shared_ptr<Library> library)
{
	std::ifstream ifs(path, std::ios::in | std::ios::binary);
	if (ifs.fail())
	{
		return false;
	}

	std::vector<uint8_t> data;

	while (!ifs.eof())
	{
		uint8_t d = 0;
		ifs.read((char*)&d, 1);
		data.push_back(d);
	}

	material_->Load(data, library, path);

	UpdatePath(path);

	previousChangedID_ = material_->GetCommandManager()->GetHistoryID();
	return true;
}

void EditorContent::UpdateBinary()
{
	if (hasStorageRef_ && path_ != "" && editor_->keyValueFileStorage_ != nullptr)
	{
		editor_->keyValueFileStorage_->Lock();

		std::vector<uint8_t> data;
		if (material_->Save(data, path_.c_str()))
		{
			editor_->keyValueFileStorage_->UpdateFile(path_.c_str(), data.data(), data.size());
		}

		editor_->keyValueFileStorage_->Unlock();
	}
}

void EditorContent::UpdatePath(const char* path)
{
	if (path_ == path)
		return;

	if (hasStorageRef_ && path_ != "" && editor_->keyValueFileStorage_ != nullptr)
	{
		editor_->keyValueFileStorage_->Lock();
		editor_->keyValueFileStorage_->ReleaseRef(path_.c_str());
		editor_->keyValueFileStorage_->Unlock();
		hasStorageRef_ = false;
	}

	path_ = path;
	material_->SetPath(path_);

	if (path_ != "" && editor_->keyValueFileStorage_ != nullptr)
	{
		editor_->keyValueFileStorage_->Lock();
		hasStorageRef_ = editor_->keyValueFileStorage_->AddRef(path_.c_str());

		std::vector<uint8_t> data;
		if (material_->Save(data, path_.c_str()))
		{
			editor_->keyValueFileStorage_->UpdateFile(path_.c_str(), data.data(), data.size());
		}

		editor_->keyValueFileStorage_->Unlock();
	}
}

std::shared_ptr<Material> EditorContent::GetMaterial() { return material_; }

std::string EditorContent::GetName()
{
	auto isChanged = previousChangedID_ != material_->GetCommandManager()->GetHistoryID();

	if (path_ != "")
	{
		auto path = path_;

		if (isChanged)
		{
			path += " *";
		}
		return path;
	}

	if (isChanged)
	{
		return "New *";
	}
	else
	{
		return "New";
	}
}

std::string EditorContent::GetPath() { return path_; }

bool EditorContent::GetIsChanged() { return previousChangedID_ != material_->GetCommandManager()->GetHistoryID(); }

static const char* label_new_node = "##NEW_NODE";
static const char* label_edit_link = "##EDIT_LINK";
static const char* label_edit_node = "##EDIT_NODE";

bool Editor::InputText(const char* label, std::string& text)
{
	char buf[255];
	memcpy(buf, text.c_str(), text.size());
	buf[text.size()] = 0;

	if (ImGui::InputText(label, buf, 255))
	{
		text = buf;
		return true;
	}

	return false;
}

std::shared_ptr<Material> Editor::GetSelectedMaterial()
{
	if (selectedContentInd_ < 0)
		return nullptr;

	return contents_[selectedContentInd_]->GetMaterial();
}

Editor::Editor(std::shared_ptr<EffekseerMaterial::Graphics> graphics)
{
	// material = std::make_shared<EffekseerMaterial::Material>();
	// material->Initialize();

	library = std::make_shared<Library>();
	graphics_ = graphics;

	preview_ = std::make_shared<EffekseerMaterial::Preview>();
	preview_->Initialize(graphics_);

	startingTime = std::chrono::system_clock::now();

	keyValueFileStorage_ = std::make_shared<IPC::KeyValueFileStorage>();
	keyValueFileStorage_->Start("EffekseerStorage");
}

Editor::~Editor()
{
	contents_.clear();
	keyValueFileStorage_->Stop();
}

void Editor::New()
{
	auto content = std::make_shared<EditorContent>(this);
	contents_.push_back(content);
	selectedContentInd_ = contents_.size() - 1;
}

void Editor::SaveAs(const char* path) { contents_[selectedContentInd_]->SaveAs(path); }

void Editor::SaveAs()
{
	nfdchar_t* outPath = NULL;
	nfdresult_t result = NFD_SaveDialog("efkmat", "", &outPath);

	if (result == NFD_OKAY)
	{
		SaveAs(outPath);
	}
}

bool Editor::Load(const char* path)
{
	auto content = std::make_shared<EditorContent>(this);
	contents_.push_back(content);
	content->Load(path, library);
	selectedContentInd_ = contents_.size() - 1;

	isLoading = true;

	return true;
}

void Editor::Load()
{
	nfdchar_t* outPath = NULL;
	nfdresult_t result = NFD_OpenDialog("efkmat", "", &outPath);

	if (result == NFD_OKAY)
	{
		Load(outPath);
	}
}

bool Editor::LoadOrSelect(const char* path)
{
	for (size_t i = 0; i < contents_.size(); i++)
	{
		if (contents_[i]->GetPath() == path)
		{
			selectedContentInd_ = i;
			return true;
		}
	}

	return Load(path);
}

void Editor::Save()
{
	auto content = contents_[selectedContentInd_];
	if (content->GetPath() == "")
	{
		Save();
	}
	else
	{
		content->SaveAs(content->GetPath().c_str());
	}
}

void Editor::CloseContents()
{
	if (contents_.size() == 0)
		return;

	auto selectedContent = contents_[selectedContentInd_];

	auto removed_it =
		std::remove_if(contents_.begin(), contents_.end(), [](std::shared_ptr<EditorContent> d) -> bool { return d->IsClosing; });
	contents_.erase(removed_it, contents_.end());

	if (selectedContent->IsClosing)
	{
		selectedContentInd_ -= 1;
		selectedContentInd_ = std::max(0, selectedContentInd_);

		if (contents_.size() > 0)
		{
		}
		else
		{
			selectedContentInd_ = -1;
		}
	}
	else
	{
		for (size_t i = 0; i < contents_.size(); i++)
		{
			if (contents_[i] == selectedContent)
			{
				selectedContentInd_ = i;
				break;
			}
		}
	}
}

void Editor::Update()
{
	auto material = GetSelectedMaterial();
	if (material == nullptr)
	{
		return;
	}

	// copy
	if (ImGui::GetIO().KeyCtrl && ImGui::GetIO().KeysDownDuration[ImGui::GetIO().KeyMap[ImGuiKey_C]] == 0)
	{
		ed::NodeId ids[256];
		auto count = ed::GetSelectedNodes(ids, 256);

		std::vector<std::shared_ptr<Node>> nodes;

		for (size_t i = 0; i < count; i++)
		{
			auto id = ids[i];
			auto node = material->FindNode(id.Get());
			if (node != nullptr)
			{
				nodes.push_back(node);
			}
		}

		auto data = material->Copy(nodes, material->GetPath().c_str());
		ImGui::SetClipboardText(data.data());
	}

	// paste
	if (ImGui::GetIO().KeyCtrl && ImGui::GetIO().KeysDownDuration[ImGui::GetIO().KeyMap[ImGuiKey_V]] == 0)
	{
		auto text = ImGui::GetClipboardText();
		if (text != nullptr)
		{
			auto pos = ImGui::GetMousePos();
			material->Paste(text, Vector2DF(pos.x, pos.y), library, material->GetPath().c_str());
		}
	}

	UpdateNodes();

	for (auto link : material->GetLinks())
	{
		UpdateLink(link);
	}

	UpdateCreating();

	UpdateDeleting();

	auto posOnEditor = ImGui::GetMousePos();

	ed::Suspend();

	ed::LinkId linkID = 0;
	ed::NodeId nodeID = 0;
	ed::PinId pinID = 0;

	if (ed::ShowNodeContextMenu(&nodeID))
	{
		ImGui::OpenPopup(label_edit_node);
		popupPosition = posOnEditor;
		currentNodeID = nodeID;
	}
	else if (ed::ShowLinkContextMenu(&linkID))
	{
		ImGui::OpenPopup(label_edit_link);
		popupPosition = posOnEditor;
		currentLinkID = linkID;
	}
	else if (ed::ShowBackgroundContextMenu())
	{
		ImGui::OpenPopup(label_new_node);
		currentPin = nullptr;
		popupPosition = posOnEditor;
	}
	ed::Resume();

	ed::Suspend();

	UpdatePopup();

	ed::Resume();

	isLoading = false;
}

void Editor::UpdateNodes()
{
	auto material = GetSelectedMaterial();
	if (material == nullptr)
	{
		return;
	}

	// Output node
	{
		for (auto node : material->GetNodes())
		{
			if (node->Parameter->Type != NodeType::Output)
				continue;
			if (!node->IsOpened)
				continue;
			if (!node->GetIsDirtied())
				continue;

			EffekseerMaterial::TextExporterGLSL exporter_;
			EffekseerMaterial::TextExporter* exporter = &exporter_;
			auto code = exporter->Export(material, node);

			auto vs = EffekseerMaterial::TextExporterGLSL::GetVertexShaderCode();

			auto textures = code.Textures;
			auto removed_it =
				std::remove_if(textures.begin(),
							   textures.end(),
							   [](const std::shared_ptr<EffekseerMaterial::TextExporterTexture>& v) -> bool { return v->Index < 0; });

			if (removed_it != textures.end())
			{
				textures.erase(removed_it);
			}

			std::sort(textures.begin(),
					  textures.end(),
					  [](const std::shared_ptr<EffekseerMaterial::TextExporterTexture>& a,
						 const std::shared_ptr<EffekseerMaterial::TextExporterTexture>& b) -> bool { return a->Index < b->Index; });

			std::vector<std::shared_ptr<EffekseerMaterial::Texture>> textures_;
			for (auto t : textures)
			{
				auto t_ = EffekseerMaterial::TextureCache::Load(graphics_, t->DefaultPath.c_str());
				textures_.push_back(t_);
			}

			preview_->CompileShader(vs, code.Code, textures_, code.Uniforms);
		}

		for (auto node : material->GetNodes())
		{
			if (node->Parameter->Type != NodeType::Output)
				continue;
			if (!node->GetIsContentDirtied())
				continue;

			EffekseerMaterial::TextExporterGLSL exporter_;
			EffekseerMaterial::TextExporter* exporter = &exporter_;
			auto code = exporter->Export(material, node);

			auto vs = EffekseerMaterial::TextExporterGLSL::GetVertexShaderCode();

			auto textures = code.Textures;
			auto removed_it =
				std::remove_if(textures.begin(),
							   textures.end(),
							   [](const std::shared_ptr<EffekseerMaterial::TextExporterTexture>& v) -> bool { return v->Index < 0; });

			if (removed_it != textures.end())
			{
				textures.erase(removed_it);
			}

			std::sort(textures.begin(),
					  textures.end(),
					  [](const std::shared_ptr<EffekseerMaterial::TextExporterTexture>& a,
						 const std::shared_ptr<EffekseerMaterial::TextExporterTexture>& b) -> bool { return a->Index < b->Index; });

			std::vector<std::shared_ptr<EffekseerMaterial::Texture>> textures_;
			for (auto t : textures)
			{
				auto t_ = EffekseerMaterial::TextureCache::Load(graphics_, t->DefaultPath.c_str());
				textures_.push_back(t_);
			}

			preview_->UpdateUniforms(textures_, code.Uniforms);
			material->ClearContentDirty(node);
		}
	}

	for (auto node : material->GetNodes())
	{
		EffekseerMaterial::Editor::UpdateNode(node);

		if ((node->UserObj == nullptr || node->GetIsDirtied()) && node->IsOpened)
		{
			auto uobj = std::make_shared<EffekseerMaterial::NodeUserDataObject>();
			uobj->GetPreview() = std::make_shared<EffekseerMaterial::Preview>();
			uobj->GetPreview()->Initialize(graphics_);

			EffekseerMaterial::TextExporterGLSL exporter_;
			EffekseerMaterial::TextExporter* exporter = &exporter_;
			auto code = exporter->Export(material, node);

			auto vs = EffekseerMaterial::TextExporterGLSL::GetVertexShaderCode();

			auto textures = code.Textures;
			auto removed_it =
				std::remove_if(textures.begin(),
							   textures.end(),
							   [](const std::shared_ptr<EffekseerMaterial::TextExporterTexture>& v) -> bool { return v->Index < 0; });

			if (removed_it != textures.end())
			{
				textures.erase(removed_it);
			}

			std::sort(textures.begin(),
					  textures.end(),
					  [](const std::shared_ptr<EffekseerMaterial::TextExporterTexture>& a,
						 const std::shared_ptr<EffekseerMaterial::TextExporterTexture>& b) -> bool { return a->Index < b->Index; });

			std::vector<std::shared_ptr<EffekseerMaterial::Texture>> textures_;
			for (auto t : textures)
			{
				auto t_ = EffekseerMaterial::TextureCache::Load(graphics_, t->DefaultPath.c_str());
				textures_.push_back(t_);
			}

			uobj->GetPreview()->CompileShader(vs, code.Code, textures_, code.Uniforms);
			node->UserObj = uobj;
			material->ClearDirty(node);
			material->ClearContentDirty(node);
		}

		if ((node->UserObj != nullptr && node->GetIsContentDirtied()) && node->IsOpened)
		{
			auto uobj = (EffekseerMaterial::NodeUserDataObject*)node->UserObj.get();
			EffekseerMaterial::TextExporterGLSL exporter_;
			EffekseerMaterial::TextExporter* exporter = &exporter_;
			auto code = exporter->Export(material, node);

			auto vs = EffekseerMaterial::TextExporterGLSL::GetVertexShaderCode();

			auto textures = code.Textures;
			auto removed_it =
				std::remove_if(textures.begin(),
							   textures.end(),
							   [](const std::shared_ptr<EffekseerMaterial::TextExporterTexture>& v) -> bool { return v->Index < 0; });

			if (removed_it != textures.end())
			{
				textures.erase(removed_it);
			}

			std::sort(textures.begin(),
					  textures.end(),
					  [](const std::shared_ptr<EffekseerMaterial::TextExporterTexture>& a,
						 const std::shared_ptr<EffekseerMaterial::TextExporterTexture>& b) -> bool { return a->Index < b->Index; });

			std::vector<std::shared_ptr<EffekseerMaterial::Texture>> textures_;
			for (auto t : textures)
			{
				auto t_ = EffekseerMaterial::TextureCache::Load(graphics_, t->DefaultPath.c_str());
				textures_.push_back(t_);
			}

			uobj->GetPreview()->UpdateUniforms(textures_, code.Uniforms);
			material->ClearContentDirty(node);
		}

		auto currentTime =
			std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - startingTime).count() / 1000.0f;

		if (node->UserObj != nullptr)
		{
			auto uobj = (EffekseerMaterial::NodeUserDataObject*)node->UserObj.get();
			uobj->GetPreview()->UpdateTime(currentTime);
		}

		preview_->UpdateTime(currentTime);
	}
}

void Editor::UpdatePopup()
{
	auto material = GetSelectedMaterial();
	if (material == nullptr)
	{
		return;
	}

	// Edit node
	if (ImGui::BeginPopup(label_edit_node))
	{
		auto node = material->FindNode(currentNodeID.Get());

		if (node != nullptr && node->Parameter->Type != NodeType::Output)
		{
			if (ImGui::MenuItem("Delete"))
			{
				material->RemoveNode(node);
			}
		}

		// Special node
		for (auto func : node->Parameter->Funcs)
		{
			if (ImGui::MenuItem(func->Name.c_str()))
			{
				func->Func(material, node);
			}
		}

		ImGui::EndPopup();
	}

	// Edit link
	if (ImGui::BeginPopup(label_edit_link))
	{
		if (ImGui::MenuItem("Delete"))
		{
			auto link = material->FindLink(currentLinkID.Get());
			material->BreakPin(link);
		}

		ImGui::EndPopup();
	}

	// New node
	if (ImGui::BeginPopup(label_new_node))
	{
		auto create_node = [&, this](std::shared_ptr<LibraryContentBase> content) -> void {
			auto nodeParam = content->Create();
			auto node = material->CreateNode(nodeParam);
			ed::SetNodePosition(node->GUID, popupPosition);
			node->Pos.X = popupPosition.x;
			node->Pos.Y = popupPosition.y;

			// link automatically
			if (currentPin != nullptr)
			{
				if (currentPin->PinDirection == PinDirectionType::Input)
				{
					if (node->OutputPins.size() > 0 && material->CanConnectPin(node->OutputPins[0], currentPin) == ConnectResultType::OK)
					{
						material->ConnectPin(node->OutputPins[0], currentPin);
					}
				}

				if (currentPin->PinDirection == PinDirectionType::Output)
				{
					if (node->InputPins.size() > 0 && material->CanConnectPin(node->InputPins[0], currentPin) == ConnectResultType::OK)
					{
						material->ConnectPin(node->InputPins[0], currentPin);
					}
				}
			}
		};

		// TODO recursive
		for (auto content : library->Root->Contents)
		{
			if (!content->IsShown)
			{
				continue;
			}

			auto& nodeTypeName = StringContainer::GetValue((content->Name + "_Name").c_str(), content->Name.c_str());

			if (ImGui::MenuItem(nodeTypeName.c_str()))
			{
				create_node(content);
				break;
			}

			// Show a description
			if (ImGui::IsItemHovered() && content->Description != "")
			{
				ImGui::SetTooltip(content->Description.c_str());
			}
		}

		for (auto group : library->Root->Groups)
		{
			auto& groupName = StringContainer::GetValue((group.first + "_Name").c_str(), group.first.c_str());

			if (ImGui::BeginMenu(groupName.c_str()))
			{
				for (auto content : group.second->Contents)
				{
					auto& nodeTypeName = StringContainer::GetValue((content->Name + "_Name").c_str(), content->Name.c_str());

					if (ImGui::MenuItem(nodeTypeName.c_str()))
					{
						create_node(content);

						break;
					}

					// Show a description
					if (ImGui::IsItemHovered() && content->Description != "")
					{
						ImGui::SetTooltip(content->Description.c_str());
					}
				}

				ImGui::EndMenu();
			}
		}

		ImGui::EndPopup();
	}
}

void Editor::UpdateCreating()
{
	auto material = GetSelectedMaterial();
	if (material == nullptr)
	{
		return;
	}

	std::shared_ptr<EffekseerMaterial::Pin> newLinkPin;

	if (ed::BeginCreate(ImColor(255, 255, 255), 2.0f))
	{

		auto showLabel = [](const char* label, ImColor color) {
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetTextLineHeight());
			auto size = ImGui::CalcTextSize(label);

			auto padding = ImGui::GetStyle().FramePadding;
			auto spacing = ImGui::GetStyle().ItemSpacing;

			auto cursorPos = ImGui::GetCursorPos();
			cursorPos.x += spacing.x;
			cursorPos.y -= spacing.y;

			ImGui::SetCursorPos(cursorPos);

			auto cursorScreenPos = ImGui::GetCursorScreenPos();
			auto rectMin = cursorScreenPos;
			auto rectMax = cursorScreenPos;

			rectMin.x -= padding.x;
			rectMin.y -= padding.y;

			rectMax.x += padding.x;
			rectMax.y += padding.y;
			rectMax.x += size.x;
			rectMax.y += size.y;

			auto drawList = ImGui::GetWindowDrawList();
			drawList->AddRectFilled(rectMin, rectMax, color, size.y * 0.15f);
			ImGui::TextUnformatted(label);
		};

		ed::PinId startPinId = 0, endPinId = 0;
		if (ed::QueryNewLink(&startPinId, &endPinId))
		{
			auto startPin = material->FindPin(startPinId.Get());
			auto endPin = material->FindPin(endPinId.Get());

			newLinkPin = startPin ? startPin : endPin;

			if (startPin->PinDirection == EffekseerMaterial::PinDirectionType::Input)
			{
				std::swap(startPin, endPin);
				std::swap(startPinId, endPinId);
			}

			if (startPin && endPin)
			{
				auto result = material->CanConnectPin(startPin, endPin);

				if (result == EffekseerMaterial::ConnectResultType::SameNode)
				{
					showLabel(StringContainer::GetValue("Error_SameNode").c_str(), ImColor(45, 32, 32, 180));
					ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
				}
				else if (result == EffekseerMaterial::ConnectResultType::SamePin)
				{
					showLabel(StringContainer::GetValue("Error_SamePin").c_str(), ImColor(45, 32, 32, 180));
					ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
				}
				else if (result == EffekseerMaterial::ConnectResultType::SameDirection)
				{
					showLabel(StringContainer::GetValue("Error_SameDirection").c_str(), ImColor(45, 32, 32, 180));
					ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
				}
				else if (result == EffekseerMaterial::ConnectResultType::Loop)
				{
					showLabel(StringContainer::GetValue("Error_Loop").c_str(), ImColor(45, 32, 32, 180));
					ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
				}
				else if (result == EffekseerMaterial::ConnectResultType::Type)
				{
					showLabel(StringContainer::GetValue("Error_Type").c_str(), ImColor(45, 32, 32, 180));
					ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
				}
				else if (result == EffekseerMaterial::ConnectResultType::OK)
				{
					showLabel(StringContainer::GetValue("Create_Link").c_str(), ImColor(32, 45, 32, 180));
					if (ed::AcceptNewItem(ImColor(128, 255, 128), 4.0f))
					{
						material->ConnectPin(startPin, endPin);
					}
				}
			}
		}

		ed::PinId pinId = 0;
		if (ed::QueryNewNode(&pinId))
		{
			newLinkPin = material->FindPin(pinId.Get());
			if (newLinkPin)
				showLabel(StringContainer::GetValue("Create_Node").c_str(), ImColor(32, 45, 32, 180));

			if (ed::AcceptNewItem())
			{
				auto posOnEditor = ImGui::GetMousePos();
				currentPin = newLinkPin;

				ed::Suspend();
				popupPosition = posOnEditor;
				ImGui::OpenPopup(label_new_node);
				ed::Resume();
			}
		}
	}
	else
	{
		newLinkPin = nullptr;
	}

	ed::EndCreate();
}

void Editor::UpdateDeleting()
{
	auto material = GetSelectedMaterial();
	if (material == nullptr)
	{
		return;
	}

	if (ed::BeginDelete())
	{
		ed::LinkId linkId = 0;
		while (ed::QueryDeletedLink(&linkId))
		{
			if (ed::AcceptDeletedItem())
			{
				auto link = material->FindLink(linkId.Get());
				material->BreakPin(link);
			}
		}

		ed::NodeId nodeId = 0;
		while (ed::QueryDeletedNode(&nodeId))
		{
			if (ed::AcceptDeletedItem())
			{
				auto node = material->FindNode(nodeId.Get());
				material->RemoveNode(node);
			}
		}

		ed::EndDelete();
	}
}

void Editor::UpdateParameterEditor(std::shared_ptr<Node> node)
{
	auto material = GetSelectedMaterial();
	if (material == nullptr)
	{
		return;
	}

	// Property
	auto updateProp = [&, node](ValueType type, std::string name, std::shared_ptr<EffekseerMaterial::NodeProperty> p) -> void {
		auto floatValues = p->Floats;

		if (type == ValueType::Float1)
		{
			if (ImGui::DragFloat(name.c_str(), floatValues.data(), 0.01f))
			{
				material->ChangeValue(p, floatValues);
				material->MakeDirty(node);
			}
		}

		if (type == ValueType::Float2)
		{
			if (ImGui::DragFloat2(name.c_str(), floatValues.data(), 0.01f))
			{
				material->ChangeValue(p, floatValues);
				material->MakeDirty(node);
			}
		}

		if (type == ValueType::Float3)
		{
			if (ImGui::DragFloat3(name.c_str(), floatValues.data(), 0.01f))
			{
				material->ChangeValue(p, floatValues);
				material->MakeDirty(node);
			}
		}

		if (type == ValueType::Float4)
		{
			if (ImGui::DragFloat4(name.c_str(), floatValues.data(), 0.01f))
			{
				material->ChangeValue(p, floatValues);
				material->MakeDirty(node);
			}
		}

		if (type == ValueType::Bool)
		{
			bool b = floatValues[0] > 0.0f;
			if (ImGui::Checkbox(name.c_str(), &b))
			{
				floatValues[0] = b ? 1.0f : 0.0f;
				material->ChangeValue(p, floatValues);
				material->MakeDirty(node);
			}
		}

		if (type == ValueType::String)
		{
			auto str = p->Str;

			// Shader result doesn't change
			if (InputText(name.c_str(), str))
			{
				material->ChangeValue(p, str);
			}
		}

		if (type == ValueType::Texture)
		{
			if (ImGui::Button("Open"))
			{
				nfdchar_t* outPath = NULL;
				nfdresult_t result = NFD_OpenDialog("png", "", &outPath);

				if (result == NFD_OKAY)
				{
					material->ChangeValue(p, outPath);
					material->FindTexture(p->Str.c_str());
				}
				else
				{
				}

				material->MakeContentDirty(node);
			}

			if (p->Str != "")
			{
				auto texture = material->FindTexture(p->Str.c_str());

				const char* items[] = {"Color", "Value"};

				if (ImGui::BeginCombo("Type", items[static_cast<int>(texture->Type)]))
				{
					for (size_t i = 0; i < 2; i++)
					{
						auto isSelected = static_cast<int>(texture->Type) == i;
						if (ImGui::Selectable(items[i], isSelected))
						{
							material->ChangeValueTextureType(texture, static_cast<TextureType>(i));
							material->MakeContentDirty(node);
						}
						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
			}
		}

		if (type == ValueType::Enum)
		{
			if (name == std::string("Index"))
			{
				const char* items[] = {"1", "2"};

				if (ImGui::BeginCombo(name.c_str(), items[static_cast<int>(floatValues[0])]))
				{
					for (size_t i = 0; i < 2; i++)
					{
						auto isSelected = static_cast<int>(floatValues[0]) == i;
						if (ImGui::Selectable(items[i], isSelected))
						{
							floatValues[0] = i;
							material->ChangeValue(p, floatValues);
							material->MakeContentDirty(node);
						}
						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
			}
			else
			{
				const char* items[] = {"Lit", "Unlit"};

				if (ImGui::BeginCombo(name.c_str(), items[static_cast<int>(floatValues[0])]))
				{
					for (size_t i = 0; i < 2; i++)
					{
						auto isSelected = static_cast<int>(floatValues[0]) == i;
						if (ImGui::Selectable(items[i], isSelected))
						{
							floatValues[0] = i;
							material->ChangeValue(p, floatValues);
							material->MakeContentDirty(node);
						}
						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
			}
		}
	};

	for (size_t i = 0; i < node->Properties.size(); i++)
	{
		auto p = node->Properties[i];
		auto pp = node->Parameter->Properties[i];

		updateProp(pp->Type, pp->Name, p);
	}
}

void Editor::UpdatePreview()
{
	ImVec2 size;
	size.x = Preview::TextureSize;
	size.y = Preview::TextureSize;
	ImGui::Image((void*)preview_->GetInternal(), size);
}

void Editor::UpdateNode(std::shared_ptr<Node> node)
{
	if (node->Parameter->Type == NodeType::Comment)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);

		ed::BeginNode(node->GUID);
		ImGui::BeginVertical("content");
		ImGui::BeginHorizontal("horizontal");
		ImGui::Text(node->Properties[0]->Str.c_str());
		ImGui::EndHorizontal();
		auto size = ed::GetNodeSize(node->GUID);
		size.x = size.x < 100 ? 100 : size.x;
		size.y = size.y < 100 ? 100 : size.y;
		ed::Group(size);
		ImGui::EndVertical();
		ed::EndNode();

		ImGui::PopStyleVar();

		if (ed::BeginGroupHint(node->GUID))
		{
			ed::EndGroupHint();
		}
		return;
	}

	ed::BeginNode(node->GUID);

	if (isLoading || node->isPosDirty)
	{
		ed::SetNodePosition(node->GUID, ImVec2(node->Pos.X, node->Pos.Y));
	}
	else
	{
		auto nodePos = ed::GetNodePosition(node->GUID);
		node->UpdatePos(Vector2DF(nodePos.x, nodePos.y));
	}

	ImGui::PushID(node->GUID);

	ImGui::BeginVertical("node");

	// Header
	ImGui::BeginHorizontal("header");
	auto& nodeTypeName = StringContainer::GetValue((node->Parameter->TypeName + "_Name").c_str(), node->Parameter->TypeName.c_str());
	ImGui::Text(nodeTypeName.c_str());
	ImGui::EndHorizontal();

	// Input and output
	ImGui::BeginHorizontal("inout_");

	if (0 < node->InputPins.size())
	{
		ImGui::BeginVertical("inputs", ImVec2(0, 0), 0.0f);
		ed::PushStyleVar(ed::StyleVar_PivotAlignment, ImVec2(0, 0.5f));
		ed::PushStyleVar(ed::StyleVar_PivotSize, ImVec2(0, 0));

		for (auto pin : node->InputPins)
		{
			ed::BeginPin(pin->GUID, ed::PinKind::Input);

			ImGui::BeginHorizontal(pin->GUID);

			std::string typeShape = "";
			if (pin->Parameter->Type == ValueType::Texture)
			{
				typeShape = "=";
			}
			else
			{
				typeShape = "-";
			}

			ImGui::Text((typeShape + std::string(" ") + pin->Parameter->Name).c_str());

			ImGui::EndHorizontal();

			ed::EndPin();
		}

		ed::PopStyleVar(2);
		ImGui::Spring(1, 0);
		ImGui::EndVertical();
	}

	if (0 < node->OutputPins.size())
	{
		ImGui::Spring(1);
		ImGui::BeginVertical("outputs", ImVec2(0, 0), 1.0f);

		ed::PushStyleVar(ed::StyleVar_PivotAlignment, ImVec2(1.0f, 0.5f));
		ed::PushStyleVar(ed::StyleVar_PivotSize, ImVec2(0, 0));

		for (auto pin : node->OutputPins)
		{
			ed::BeginPin(pin->GUID, ed::PinKind::Output);

			ImGui::BeginHorizontal(pin->GUID);

			std::string typeShape = "";
			if (pin->Parameter->Type == ValueType::Texture)
			{
				typeShape = "=";
			}
			else
			{
				typeShape = "-";
			}

			ImGui::Text((pin->Parameter->Name + std::string(" ") + typeShape).c_str());

			ImGui::EndHorizontal();

			ed::EndPin();
		}

		ed::PopStyleVar(2);
		ImGui::Spring(1, 0);
		ImGui::EndVertical();
	}

	ImGui::EndHorizontal();

	// show a preview
	ImGui::SetNextTreeNodeOpen(node->IsOpened, ImGuiCond_Once);
	if (ImGui::TreeNode("Preview"))
	{
		auto preview = (NodeUserDataObject*)node->UserObj.get();
		if (preview != nullptr)
		{
			ImVec2 size;
			size.x = Preview::TextureSize;
			size.y = Preview::TextureSize;
			ImGui::Image((void*)preview->GetPreview()->GetInternal(), size);
		}
		ImGui::TreePop();

		node->IsOpened = true;
	}
	else
	{
		node->IsOpened = false;
	}

	ImGui::EndVertical();

	ImGui::PopID();
	ed::EndNode();

	auto itemRectMin = ImGui::GetItemRectMin();
	auto itemRectMax = ImGui::GetItemRectMax();

	auto bg_drawList = ed::GetNodeBackgroundDrawList(node->GUID);

	// Header backgroud
	bg_drawList->AddRectFilled(ImVec2(itemRectMin.x, itemRectMin.y),
							   ImVec2(itemRectMax.x, itemRectMin.y + 30),
							   IM_COL32(110, 110, 110, 255),
							   ed::GetStyle().NodeRounding,
							   ImDrawCornerFlags_Top);

	node->isPosDirty = false;
}

void Editor::UpdateLink(std::shared_ptr<Link> link) { ed::Link(link->GUID, link->InputPin->GUID, link->OutputPin->GUID); }

} // namespace EffekseerMaterial
