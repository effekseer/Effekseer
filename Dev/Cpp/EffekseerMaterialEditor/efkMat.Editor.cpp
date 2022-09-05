#define NOMINMAX
#include "efkMat.Editor.h"
#include "efkMat.CommandManager.h"
#include "efkMat.Models.h"

#include <imgui_node_editor.h>

#include <imgui_node_editor_internal.h>

#include <imgui_internal.h>
#include <nfd.h>

#include <GUI/MainWindow.h>
#include <fstream>
#include <iostream>
#include <ostream>

#include <efkMat.StringContainer.h>
#include <efkMat.TextExporter.h>

#include <ImGradientHDR.h>

#include "../Effekseer/Effekseer/Material/Effekseer.MaterialFile.h"
#include "../EffekseerMaterialCompiler/OpenGL/EffekseerMaterialCompilerGL.h"
#include "../EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.MaterialLoader.h"

#include <boxer.h>

namespace EffekseerMaterial
{
static Vector2DF GetNodeGroupSize(uint64_t guid)
{
	auto editor = reinterpret_cast<ax::NodeEditor::Detail::EditorContext*>(ed::GetCurrentEditor());
	if (editor == nullptr)
	{
		return Vector2DF{0.0f, 0.0f};
	}
	auto nodeContext = editor->GetNode(guid);

	if (nodeContext->m_Type == ax::NodeEditor::Detail::NodeType::Node)
	{
		return Vector2DF{0.0f, 0.0f};
	}

	ImVec2 size{0, 0};

	if (nodeContext != nullptr)
	{
		size = nodeContext->m_GroupBounds.GetSize();
	}

	return {size.x, size.y};
}

static std::tuple<Vector2DF, Vector2DF> GetSelectedNodeBounds(float margin)
{
	std::array<ed::NodeId, 256> selectedNodes;
	auto selectedCount = ed::GetSelectedNodes(selectedNodes.data(), selectedNodes.size());

	ImVec2 areaMin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	ImVec2 areaMax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());

	for (size_t i = 0; i < selectedCount; i++)
	{
		const auto pos = ed::GetNodePosition(selectedNodes[i]);
		const auto size = ed::GetNodeSize(selectedNodes[i]);
		areaMin.x = std::min(areaMin.x, pos.x);
		areaMin.y = std::min(areaMin.y, pos.y);
		areaMax.x = std::max(areaMax.x, (pos + size).x);
		areaMax.y = std::max(areaMax.y, (pos + size).y);
	}

	if (selectedCount > 0)
	{
		areaMin.x -= margin;
		areaMin.y -= margin;
		areaMax.x += margin;
		areaMax.y += margin;
		return std::tuple<Vector2DF, Vector2DF>{Vector2DF{areaMin.x, areaMin.y}, Vector2DF{areaMax.x, areaMax.y}};
	}

	return std::tuple<Vector2DF, Vector2DF>{};
}

void Compile(std::shared_ptr<Graphics> graphics,
			 std::shared_ptr<Material> material,
			 std::shared_ptr<Node> node,
			 std::vector<std::shared_ptr<TextureWithSampler>>& outputTextures,
			 std::vector<std::shared_ptr<TextExporterUniform>>& outputUniforms,
			 std::vector<std::shared_ptr<TextExporterGradient>>& gradients,
			 std::vector<std::shared_ptr<TextExporterGradient>>& fixedGradients,
			 std::string& vs,
			 std::string& ps)
{
	EffekseerMaterial::TextExporter exporter;
	auto result = (&exporter)->Export(material, node);

	auto efkMaterial = Effekseer::MaterialFile();
	efkMaterial.SetGenericCode(result.Code.c_str());
	efkMaterial.SetIsSimpleVertex(false);
	efkMaterial.SetHasRefraction(result.HasRefraction);
	efkMaterial.SetShadingModel(static_cast<Effekseer::ShadingModelType>(result.ShadingModel));

	// HACK (adhoc support in the editor)
	// efkMaterial.SetCustomData1Count(result.CustomData1);
	// efkMaterial.SetCustomData2Count(result.CustomData2);

	efkMaterial.SetTextureCount(result.Textures.size());
	efkMaterial.SetUniformCount(result.Uniforms.size());

	for (const auto& type : result.RequiredPredefinedMethodTypes)
	{
		efkMaterial.RequiredMethods.emplace_back(static_cast<Effekseer::MaterialFile::RequiredPredefinedMethodType>(type));
	}

	for (size_t i = 0; i < result.Textures.size(); i++)
	{
		efkMaterial.SetTextureIndex(i, i);
		efkMaterial.SetTextureName(i, result.Textures[i]->UniformName.c_str());
	}

	for (size_t i = 0; i < result.Uniforms.size(); i++)
	{
		efkMaterial.SetUniformIndex(i, (int)result.Uniforms[i]->Type);
		efkMaterial.SetUniformName(i, result.Uniforms[i]->UniformName.c_str());
	}

	const auto copyGradient = [&](std::vector<Effekseer::MaterialFile::GradientParameter>& dst, const std::vector<std::shared_ptr<TextExporterGradient>>& src)
	{
		dst.resize(src.size());

		for (size_t i = 0; i < dst.size(); i++)
		{
			dst[i].Name = src[i]->UniformName;
			dst[i].Data.ColorCount = src[i]->Defaults.ColorCount;
			for (size_t j = 0; j < dst[i].Data.Colors.size(); j++)
			{
				dst[i].Data.Colors[j].Color = src[i]->Defaults.Colors[j].Color;
				dst[i].Data.Colors[j].Intensity = src[i]->Defaults.Colors[j].Intensity;
				dst[i].Data.Colors[j].Position = src[i]->Defaults.Colors[j].Position;
			}

			dst[i].Data.AlphaCount = src[i]->Defaults.AlphaCount;
			for (size_t j = 0; j < dst[i].Data.Alphas.size(); j++)
			{
				dst[i].Data.Alphas[j].Alpha = src[i]->Defaults.Alphas[j].Alpha;
				dst[i].Data.Alphas[j].Position = src[i]->Defaults.Alphas[j].Position;
			}
		}
	};

	copyGradient(efkMaterial.Gradients, result.Gradients);

	copyGradient(efkMaterial.FixedGradients, result.FixedGradients);

	auto compiler = ::Effekseer::CreateUniqueReference(new Effekseer::MaterialCompilerGL());
	auto binary = ::Effekseer::CreateUniqueReference(compiler->Compile(&efkMaterial, 1024, 1024));

	vs = reinterpret_cast<const char*>(binary->GetVertexShaderData(Effekseer::MaterialShaderType::Standard));
	ps = reinterpret_cast<const char*>(binary->GetPixelShaderData(Effekseer::MaterialShaderType::Standard));

	auto textures = result.Textures;
	auto removed_it = std::remove_if(textures.begin(),
									 textures.end(),
									 [](const std::shared_ptr<EffekseerMaterial::TextExporterTexture>& v) -> bool
									 { return v->Index < 0; });

	if (removed_it != textures.end())
	{
		textures.erase(removed_it);
	}

	std::sort(textures.begin(),
			  textures.end(),
			  [](const std::shared_ptr<EffekseerMaterial::TextExporterTexture>& a,
				 const std::shared_ptr<EffekseerMaterial::TextExporterTexture>& b) -> bool
			  { return a->Index < b->Index; });

	for (auto t : textures)
	{
		auto t_ = EffekseerMaterial::TextureCache::Load(graphics, t->DefaultPath.c_str());
		auto ts = std::make_shared<TextureWithSampler>();
		ts->Name = t->UniformName;
		ts->TexturePtr = t_;
		ts->SamplerType = t->Sampler;
		outputTextures.push_back(ts);
	}

	outputUniforms = result.Uniforms;

	fixedGradients = result.FixedGradients;

	gradients = result.Gradients;
}

void ExtractUniforms(std::shared_ptr<Graphics> graphics,
					 std::shared_ptr<Material> material,
					 std::shared_ptr<Node> node,
					 std::vector<std::shared_ptr<TextureWithSampler>>& outputTextures,
					 std::vector<std::shared_ptr<TextExporterUniform>>& outputUniforms,
					 std::vector<std::shared_ptr<TextExporterGradient>>& outputGradients)
{
	outputTextures.clear();
	outputUniforms.clear();

	EffekseerMaterial::TextExporter exporter;
	auto result = (&exporter)->Export(material, node);

	// auto vs = EffekseerMaterial::TextExporterGLSL::GetVertexShaderCode();

	auto textures = result.Textures;
	auto removed_it = std::remove_if(textures.begin(),
									 textures.end(),
									 [](const std::shared_ptr<EffekseerMaterial::TextExporterTexture>& v) -> bool
									 { return v->Index < 0; });

	if (removed_it != textures.end())
	{
		textures.erase(removed_it);
	}

	std::sort(textures.begin(),
			  textures.end(),
			  [](const std::shared_ptr<EffekseerMaterial::TextExporterTexture>& a,
				 const std::shared_ptr<EffekseerMaterial::TextExporterTexture>& b) -> bool
			  { return a->Index < b->Index; });

	for (auto t : textures)
	{
		auto t_ = EffekseerMaterial::TextureCache::Load(graphics, t->DefaultPath.c_str());
		auto ts = std::make_shared<TextureWithSampler>();
		ts->Name = t->UniformName;
		ts->TexturePtr = t_;
		ts->SamplerType = t->Sampler;
		outputTextures.push_back(ts);
	}

	outputUniforms = result.Uniforms;

	outputGradients = result.Gradients;
}

NodeUserDataObject::NodeUserDataObject()
{
}

NodeUserDataObject::~NodeUserDataObject()
{
}

EditorContent::EditorContent(Editor* editor)
	: editor_(editor)
{
	material_ = std::make_shared<EffekseerMaterial::Material>();
	material_->Initialize();

	ed::Config config;
	config.SettingsFile = "config.EffekseerMaterial.Node.json";
	editorContext_ = ed::CreateEditor(&config);

	ClearIsChanged();
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
		nfdchar_t* outPath = nullptr;
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

	char16_t path16[260];
	Effekseer::ConvertUtf8ToUtf16(path16, 260, path);

	std::string pathstr = path;
	int ext_i = pathstr.find_last_of(".");

	if (ext_i >= 0 && pathstr.substr(ext_i, pathstr.size() - ext_i) == ".efkmat")
	{
		FILE* fp = nullptr;
#ifdef _WIN32
		_wfopen_s(&fp, (const wchar_t*)path16, L"wb");
#else
		fp = fopen(path, "wb");
#endif
		if (fp == nullptr)
		{
			return;
		}

		fwrite(data.data(), 1, data.size(), fp);
		fclose(fp);
	}
	else
	{
		char16_t path16[260];
		Effekseer::ConvertUtf8ToUtf16(path16, 260, (std::string(path) + ".efkmat").c_str());

		FILE* fp = nullptr;
#ifdef _WIN32
		_wfopen_s(&fp, (const wchar_t*)path16, L"wb");
#else
		fp = fopen(path, "wb");
#endif
		if (fp == nullptr)
		{
			return;
		}

		fwrite(data.data(), 1, data.size(), fp);
		fclose(fp);
	}

	UpdatePath(path);

	previousChangedID_ = material_->GetCommandManager()->GetHistoryID();
}

ErrorCode EditorContent::Load(const char* path, std::shared_ptr<Library> library)
{
	FILE* fp = nullptr;
#ifdef _WIN32
	char16_t path16[260];
	Effekseer::ConvertUtf8ToUtf16(path16, 260, path);
	_wfopen_s(&fp, (const wchar_t*)path16, L"rb");
#else
	fp = fopen(path, "rb");
#endif
	if (fp == nullptr)
	{
		return ErrorCode::NotFound;
	}

	std::vector<uint8_t> data;

	fseek(fp, 0, SEEK_END);
	auto datasize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	data.resize(datasize);

	fread(data.data(), 1, data.size(), fp);
	fclose(fp);

	auto err = material_->Load(data, library, path);
	if (err != ErrorCode::OK)
	{
		if (err == ErrorCode::NewVersion)
		{
			boxer::show(
				StringContainer::GetValue("Error_NewVersion").c_str(),
				"Error",
				(boxer::Style::Error),
				(boxer::Buttons::OK));
		}
		return err;
	}

	UpdatePath(path);

	ClearIsChanged();
	return ErrorCode::OK;
}

void EditorContent::UpdateBinary()
{
	if (hasStorageRef_ && path_ != "" && editor_->keyValueFileStorage_ != nullptr)
	{
		editor_->keyValueFileStorage_->Lock();

		std::vector<uint8_t> data;
		if (material_->Save(data, path_.c_str()))
		{
#ifdef _DEBUG
			std::cout << "UpdateFile : " << path_ << std::endl;
#endif
			editor_->keyValueFileStorage_->UpdateFile(path_.c_str(), data.data(), data.size());
		}

		editor_->keyValueFileStorage_->Unlock();
	}
}

void EditorContent::UpdatePath(const char* path)
{
	auto p = ResolvePath(path);
	if (path_ == p)
		return;

	if (hasStorageRef_ && path_ != "" && editor_->keyValueFileStorage_ != nullptr)
	{
		editor_->keyValueFileStorage_->Lock();
		editor_->keyValueFileStorage_->ReleaseRef(path_.c_str());
		editor_->keyValueFileStorage_->Unlock();
		hasStorageRef_ = false;
	}

	path_ = p;
	material_->SetPath(path_);

	// get name
	{
		auto it = path_.find_last_of('/');
		name_ = (it != path_.npos) ? path_.substr(it + 1) : path_;
	}

	if (path_ != "" && editor_->keyValueFileStorage_ != nullptr)
	{
		editor_->keyValueFileStorage_->Lock();
		hasStorageRef_ = editor_->keyValueFileStorage_->AddRef(path_.c_str());

		std::vector<uint8_t> data;
		if (material_->Save(data, path_.c_str()))
		{
#ifdef _DEBUG
			std::cout << "UpdateFile : " << path_ << std::endl;
#endif

			editor_->keyValueFileStorage_->UpdateFile(path_.c_str(), data.data(), data.size());
		}

		editor_->keyValueFileStorage_->Unlock();
	}
}

std::shared_ptr<Material> EditorContent::GetMaterial()
{
	return material_;
}

std::string EditorContent::GetName()
{
	auto isChanged = previousChangedID_ != material_->GetCommandManager()->GetHistoryID();

	if (path_ != "")
	{
		std::string name = name_;
		if (isChanged)
		{
			name += " *";
		}
		return name;
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

std::string EditorContent::GetPath()
{
	return path_;
}

bool EditorContent::GetIsChanged()
{
	return previousChangedID_ != material_->GetCommandManager()->GetHistoryID();
}

void EditorContent::ClearIsChanged()
{
	previousChangedID_ = material_->GetCommandManager()->GetHistoryID();
}

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
	preview_->ModelType = PreviewModelType::Sphere;

	startingTime = std::chrono::system_clock::now();

	keyValueFileStorage_ = std::make_shared<IPC::KeyValueFileStorage>();
	keyValueFileStorage_->Start("EfkStorage");

	previewTypeButtons_.emplace_back(TextureCache::Load(graphics, "resources/icons/Material_Icon_Squre.png"));
	previewTypeButtons_.emplace_back(TextureCache::Load(graphics, "resources/icons/Material_Icon_Sphere.png"));
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
	isSelectedDirty_ = true;
}

void Editor::SaveAs(const char* path)
{
	if (selectedContentInd_ < 0 || selectedContentInd_ >= contents_.size())
		return;
	contents_[selectedContentInd_]->SaveAs(path);
}

void Editor::SaveAs()
{
	if (selectedContentInd_ < 0 || selectedContentInd_ >= contents_.size())
		return;

	nfdchar_t* outPath = nullptr;
	nfdresult_t result = NFD_SaveDialog("efkmat", "", &outPath);

	if (result == NFD_OKAY)
	{
		SaveAs(outPath);
	}
}

ErrorCode Editor::Load(const char* path)
{
	auto content = std::make_shared<EditorContent>(this);
	auto err = content->Load(path, library);
	if (err != ErrorCode::OK)
	{
		return err;
	}

	contents_.push_back(content);

	selectedContentInd_ = contents_.size() - 1;

	isSelectedDirty_ = true;
	content->IsLoading = true;

	return ErrorCode::OK;
}

bool Editor::Load()
{
	nfdchar_t* outPath = nullptr;
	nfdresult_t result = NFD_OpenDialog("efkmat", "", &outPath);

	if (result == NFD_OKAY)
	{
		LoadOrSelect(outPath);
		return true;
	}
	return false;
}

ErrorCode Editor::LoadOrSelect(const char* path)
{
	auto p = ResolvePath(path);

	for (size_t i = 0; i < contents_.size(); i++)
	{
		if (contents_[i]->GetPath() == p)
		{
			selectedContentInd_ = i;
			return ErrorCode::OK;
		}
	}

	return Load(p.c_str());
}

void Editor::Save()
{
	if (selectedContentInd_ < 0 || selectedContentInd_ >= contents_.size())
		return;

	auto content = contents_[selectedContentInd_];
	if (content->GetPath() == "")
	{
		SaveAs();
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
		std::remove_if(contents_.begin(), contents_.end(), [](std::shared_ptr<EditorContent> d) -> bool
					   { return d->IsClosing; });
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

	if (!ImGui::IsAnyItemActive())
	{
		bool isCtrlPressed = false;
		// TODO refactoring
#ifdef __APPLE__
		isCtrlPressed = ImGui::GetIO().KeySuper;
#else
		isCtrlPressed = ImGui::GetIO().KeyCtrl;
#endif

		// copy
		if (isCtrlPressed && ImGui::IsKeyPressed(ImGuiKey_C))
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
		if (isCtrlPressed && ImGui::IsKeyPressed(ImGuiKey_V))
		{
			auto text = ImGui::GetClipboardText();
			if (text != nullptr)
			{
				auto pos = ImGui::GetMousePos();
				material->Paste(text, Vector2DF(pos.x, pos.y), library, material->GetPath().c_str());
			}
		}

		// save
		if (isCtrlPressed && ImGui::IsKeyPressed(ImGuiKey_S))
		{
			Save();
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
		searchingKeywords.fill(0);
		searchingKeywordsActual.fill(0);
		isJustNewNodePanelOpened_ = true;
		currentPin = nullptr;
		popupPosition = posOnEditor;
	}
	ed::Resume();

	ed::Suspend();

	UpdatePopup();

	ed::Resume();

	contents_[GetSelectedContentIndex()]->IsLoading = false;
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
			if (!node->GetIsDirtied() && !isSelectedDirty_)
				continue;

			std::vector<std::shared_ptr<TextExporterUniform>> uniforms;
			std::vector<std::shared_ptr<TextureWithSampler>> textures;
			std::vector<std::shared_ptr<TextExporterGradient>> gradients;
			std::vector<std::shared_ptr<TextExporterGradient>> fixedGradients;

			std::string vs;
			std::string ps;
			Compile(graphics_, material, node, textures, uniforms, gradients, fixedGradients, vs, ps);

			// update pin state
			for (auto behavior : node->Parameter->BehaviorComponents)
			{
				if (!behavior->IsGetIsInputPinEnabledInherited)
					continue;

				for (auto pin : node->InputPins)
				{
					auto enabled = behavior->GetIsInputPinEnabled(material, node->Parameter, node, pin);
					pin->IsEnabled = enabled;
				}
			}

			preview_->CompileShader(vs, ps, textures, uniforms, gradients, fixedGradients);
			previewTextureCount_ = textures.size();
			previewUniformCount_ = uniforms.size();
			previewGradientCount_ = gradients.size();
		}

		for (auto node : material->GetNodes())
		{
			if (node->Parameter->Type != NodeType::Output)
				continue;
			if (!node->GetIsContentDirtied())
				continue;

			std::vector<std::shared_ptr<TextExporterUniform>> uniforms;
			std::vector<std::shared_ptr<TextureWithSampler>> textures;
			std::vector<std::shared_ptr<TextExporterGradient>> gradients;

			ExtractUniforms(graphics_, material, node, textures, uniforms, gradients);

			preview_->UpdateUniforms(textures, uniforms, gradients);
			previewTextureCount_ = textures.size();
			previewUniformCount_ = uniforms.size();
			previewGradientCount_ = gradients.size();
		}
	}

	auto currentTime =
		std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - startingTime).count() / 1000.0f;

	UpdateToRecordMovingCommand();

	for (auto node : material->GetNodes())
	{
		EffekseerMaterial::Editor::UpdateNode(node);

		if ((node->UserObj == nullptr || node->GetIsDirtied()) && (node->IsPreviewOpened || node->Parameter->Type == NodeType::Output))
		{
			bool isGenerated = false;

			if (node->UserObj == nullptr)
			{
				isGenerated = true;
				node->UserObj = std::make_shared<EffekseerMaterial::NodeUserDataObject>();
			}

			auto uobj = (EffekseerMaterial::NodeUserDataObject*)node->UserObj.get();

			if (uobj->GetPreview() == nullptr)
			{
				uobj->GetPreview() = std::make_shared<EffekseerMaterial::Preview>();
				uobj->GetPreview()->Initialize(graphics_);
			}

			auto preview = uobj->GetPreview();

			if (isGenerated && node->Parameter->Type == NodeType::Output)
			{
				preview->ModelType = PreviewModelType::Sphere;
			}

			std::vector<std::shared_ptr<TextExporterUniform>> uniforms;
			std::vector<std::shared_ptr<TextureWithSampler>> textures;
			std::vector<std::shared_ptr<TextExporterGradient>> gradients;
			std::vector<std::shared_ptr<TextExporterGradient>> fixedGradients;

			std::string vs;
			std::string ps;
			Compile(graphics_, material, node, textures, uniforms, gradients, fixedGradients, vs, ps);

			// update pin state
			for (auto behavior : node->Parameter->BehaviorComponents)
			{
				if (!behavior->IsGetIsInputPinEnabledInherited)
					continue;

				for (auto pin : node->InputPins)
				{
					auto enabled = behavior->GetIsInputPinEnabled(material, node->Parameter, node, pin);
					pin->IsEnabled = enabled;
				}
			}

			preview->CompileShader(vs, ps, textures, uniforms, gradients, fixedGradients);

			material->ClearDirty(node);
			material->ClearContentDirty(node);
		}

		if ((node->UserObj != nullptr && node->GetIsContentDirtied()) &&
			(node->IsPreviewOpened || node->Parameter->Type == NodeType::Output))
		{
			auto uobj = (EffekseerMaterial::NodeUserDataObject*)node->UserObj.get();

			auto preview = uobj->GetPreview();

			std::vector<std::shared_ptr<TextExporterUniform>> uniforms;
			std::vector<std::shared_ptr<TextureWithSampler>> textures;
			std::vector<std::shared_ptr<TextExporterGradient>> gradients;

			ExtractUniforms(graphics_, material, node, textures, uniforms, gradients);

			preview->UpdateUniforms(textures, uniforms, gradients);
			material->ClearContentDirty(node);
		}

		if (node->UserObj != nullptr)
		{
			auto uobj = (EffekseerMaterial::NodeUserDataObject*)node->UserObj.get();
			uobj->GetPreview()->UpdateConstantValues(currentTime, material->CustomData[0].Values, material->CustomData[1].Values);
		}
	}

	preview_->UpdateConstantValues(currentTime, material->CustomData[0].Values, material->CustomData[1].Values);
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

		// Special node
		for (auto func : node->Parameter->Funcs)
		{
			if (ImGui::MenuItem(StringContainer::GetValue((func->Name + "_Name").c_str(), func->Name.c_str()).c_str()))
			{
				func->Func(material, node);
			}
		}

		if (node != nullptr && node->Parameter->Type != NodeType::Output)
		{
			if (ImGui::MenuItem(StringContainer::GetValue("Delete_Name", "Delete").c_str()))
			{
				material->RemoveNode(node);
			}
		}

		ImGui::EndPopup();
	}

	// Edit link
	if (ImGui::BeginPopup(label_edit_link))
	{
		if (ImGui::MenuItem(StringContainer::GetValue("Delete_Name", "Delete").c_str()))
		{
			auto link = material->FindLink(currentLinkID.Get());
			material->BreakPin(link);
		}

		ImGui::EndPopup();
	}

	// New node
	if (ImGui::BeginPopup(label_new_node))
	{
		auto create_node = [&, this](std::shared_ptr<LibraryContentBase> content) -> void
		{
			auto nodeParam = content->Create();
			auto node = material->CreateNode(nodeParam, false);
			ImVec2 nodePos{floorf(popupPosition.x), floorf(popupPosition.y)};
			ed::SetNodePosition(node->GUID, nodePos);
			node->Pos.X = nodePos.x;
			node->Pos.Y = nodePos.y;

			if (node->Parameter->Type == NodeType::Comment)
			{
				// A comment node includes all selected nodes
				const auto [areaMin, areaMax] = GetSelectedNodeBounds(30.0f);
				if (areaMax.X - areaMin.X > 0.0f && areaMax.Y - areaMin.Y > 0.0f)
				{
					node->Pos = areaMin;
					node->CommentSize.X = areaMax.X - areaMin.X;
					node->CommentSize.Y = areaMax.Y - areaMin.Y;
				}
			}

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

		auto showContent = [&create_node](std::shared_ptr<LibraryContentBase> c) -> void
		{
			auto& nodeTypeName = StringContainer::GetValue((c->Name + "_Node_Name").c_str(), c->Name.c_str());

			if (ImGui::MenuItem(nodeTypeName.c_str()))
			{
				create_node(c);
				return;
			}

			// Show a description
			if (ImGui::IsItemHovered() && ImGui::GetCurrentContext()->HoveredIdTimer > 0.25)
			{
				auto desc = std::string("Key : ") + c->KeywordsShown + "\n";
				desc += StringContainer::GetValue((c->Name + "_Node_Desc").c_str(), "");

				ImGui::SetTooltip(desc.c_str());
			}
		};

		auto isShown = [this](std::shared_ptr<LibraryContentBase> c) -> bool
		{
			if (!c->IsShown)
			{
				return false;
			}

			if (searchingKeywordsActual[0] == 0)
			{
				return true;
			}

			auto name = c->Name;

			if (name.find(searchingKeywordsActual.data()) != std::string::npos)
				return true;

			for (auto keyword : c->Keywords)
			{
				if (keyword.find(searchingKeywordsActual.data()) != std::string::npos)
					return true;
			}

			return false;
		};

		// keyword box

		// focus into a searching textbox
		if (isJustNewNodePanelOpened_)
		{
			ImGui::SetKeyboardFocusHere(0);
		}

		ImGui::InputText(StringContainer::GetValue("Search").c_str(), searchingKeywords.data(), searchingKeywords.size());

		for (size_t c = 0; c < searchingKeywords.size(); c++)
		{
			searchingKeywordsActual[c] = tolower(searchingKeywords[c]);
			if (searchingKeywordsActual[c] == 0)
				break;
		}

		if (searchingKeywordsActual[0] == 0)
		{
			for (auto group : library->Root->Groups)
			{
				auto& groupName = StringContainer::GetValue((group->Name + "_Name").c_str(), group->Name.c_str());

				if (ImGui::BeginMenu(groupName.c_str()))
				{
					for (auto content : group->Contents)
					{
						if (!isShown(content))
						{
							continue;
						}

						showContent(content);
					}

					ImGui::EndMenu();
				}
			}

			// TODO recursive
			for (auto content : library->Root->Contents)
			{
				if (!isShown(content))
				{
					continue;
				}

				showContent(content);
			}
		}
		else
		{
			for (auto group : library->Root->Groups)
			{
				for (auto content : group->Contents)
				{
					if (!isShown(content))
					{
						continue;
					}

					showContent(content);
				}
			}

			// TODO recursive
			for (auto content : library->Root->Contents)
			{
				if (!isShown(content))
				{
					continue;
				}

				showContent(content);
			}
		}

		isJustNewNodePanelOpened_ = false;

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

	// call GetMousePos to get to a relative position because of imgui specification on this
	auto posOnEditor = ImGui::GetMousePos();
	std::shared_ptr<EffekseerMaterial::Pin> newLinkPin;

	if (ed::BeginCreate(ImColor(255, 255, 255), 2.0f))
	{

		auto showLabel = [](const char* label, ImColor color)
		{
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

				currentPin = newLinkPin;

				ed::Suspend();
				popupPosition = posOnEditor;
				ImGui::OpenPopup(label_new_node);
				searchingKeywords.fill(0);
				searchingKeywordsActual.fill(0);
				isJustNewNodePanelOpened_ = true;
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
		bool foundDelete = false;

		auto startCollection = [&foundDelete, &material]() -> void
		{
			if (!foundDelete)
			{
				material->GetCommandManager()->StartCollection();
				foundDelete = true;
			}
		};

		ed::LinkId linkId = 0;
		while (ed::QueryDeletedLink(&linkId))
		{
			if (ed::AcceptDeletedItem())
			{
				startCollection();

				auto link = material->FindLink(linkId.Get());
				material->BreakPin(link);
			}
		}

		ed::NodeId nodeId = 0;
		while (ed::QueryDeletedNode(&nodeId))
		{
			if (ed::AcceptDeletedItem())
			{
				startCollection();

				auto node = material->FindNode(nodeId.Get());
				material->RemoveNode(node);
			}
		}

		if (foundDelete)
		{
			material->GetCommandManager()->EndCollection();
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
	auto updateProp = [&, node](ValueType type, std::string name, std::shared_ptr<EffekseerMaterial::NodeProperty> p) -> void
	{
		auto floatValues = p->Floats;

		auto nameStr = StringContainer::GetValue((name + "_Name").c_str(), name.c_str());

		if (type == ValueType::Int)
		{
			int32_t value = static_cast<int32_t>(floatValues[0]);
			if (ImGui::DragInt(nameStr.c_str(), &value, 1, 1, 100))
			{
				floatValues[0] = value;
				material->ChangeValue(p, floatValues);
				material->MakeDirty(node);
			}
		}
		else if (type == ValueType::Float1)
		{
			if (ImGui::DragFloat(nameStr.c_str(), floatValues.data(), 0.01f))
			{
				material->ChangeValue(p, floatValues);
				material->MakeDirty(node);
			}
		}
		else if (type == ValueType::Float2)
		{
			if (ImGui::DragFloat2(nameStr.c_str(), floatValues.data(), 0.01f))
			{
				material->ChangeValue(p, floatValues);
				material->MakeDirty(node);
			}
		}
		else if (type == ValueType::Float3)
		{
			if (ImGui::DragFloat3(nameStr.c_str(), floatValues.data(), 0.01f))
			{
				material->ChangeValue(p, floatValues);
				material->MakeDirty(node);
			}
		}
		else if (type == ValueType::Float4)
		{
			if (ImGui::DragFloat4(nameStr.c_str(), floatValues.data(), 0.01f))
			{
				material->ChangeValue(p, floatValues);
				material->MakeDirty(node);
			}
		}
		else if (type == ValueType::Bool)
		{
			bool b = floatValues[0] > 0.0f;
			if (ImGui::Checkbox(nameStr.c_str(), &b))
			{
				floatValues[0] = b ? 1.0f : 0.0f;
				material->ChangeValue(p, floatValues);
				material->MakeDirty(node);
			}
		}
		else if (type == ValueType::String)
		{
			// is memory safe?

			auto str = p->Str;
			str.resize(str.size() + 16, 0);

			// Shader result doesn't change
			if (InputText(nameStr.c_str(), str))
			{
				material->ChangeValue(p, str);
			}
		}
		else if (type == ValueType::Texture)
		{
			auto showPath = [&p]() -> void
			{
				if (ImGui::IsItemHovered() && !ImGui::IsItemActive())
				{
					ImGui::SetTooltip(p->Str.c_str());
				}
			};

			if (ImGui::Button(nameStr.c_str()))
			{
				nfdchar_t* outPath = nullptr;
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

			showPath();

			if (p->Str != "")
			{
				auto texture = EffekseerMaterial::TextureCache::Load(graphics_, p->Str.c_str());
				ImVec2 size;
				size.x = Preview::TextureSize;
				size.y = Preview::TextureSize;

				if (texture != nullptr && texture->GetTexture() != nullptr)
				{
					ImGui::Image((void*)texture->GetTexture()->GetInternalObjects()[0], size, ImVec2(0.0, 1.0), ImVec2(1.0, 0.0));

					showPath();

					// adhoc
					glBindTexture(GL_TEXTURE_2D, (GLuint)texture->GetTexture()->GetInternalObjects()[0]);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				}
			}

			if (p->Str != "")
			{
				auto texture = material->FindTexture(p->Str.c_str());

				const char* items[] = {"Color", "Value"};

				if (ImGui::BeginCombo("ColorType", items[static_cast<int>(texture->Type)]))
				{
					for (size_t i = 0; i < 2; i++)
					{
						auto isSelected = static_cast<int>(texture->Type) == i;
						if (ImGui::Selectable(items[i], isSelected))
						{
							material->ChangeValueTextureType(texture, static_cast<TextureValueType>(i));
							material->MakeContentDirty(node);
						}
						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
			}
		}
		else if (type == ValueType::Enum)
		{
			if (name == std::string("Index") || name == std::string("UVIndex"))
			{
				const char* items[] = {"1", "2"};

				if (ImGui::BeginCombo(nameStr.c_str(), items[static_cast<int>(floatValues[0])]))
				{
					for (size_t i = 0; i < 2; i++)
					{
						auto isSelected = static_cast<int>(floatValues[0]) == i;
						if (ImGui::Selectable(items[i], isSelected))
						{
							floatValues[0] = i;
							material->ChangeValue(p, floatValues);
							material->MakeDirty(node);
						}
						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
			}
			else if (name == std::string("Sampler"))
			{
				const char* items[] = {"Repeat", "Clamp"};

				if (ImGui::BeginCombo(nameStr.c_str(), items[static_cast<int>(floatValues[0])]))
				{
					for (size_t i = 0; i < 2; i++)
					{
						auto isSelected = static_cast<int>(floatValues[0]) == i;
						if (ImGui::Selectable(items[i], isSelected))
						{
							floatValues[0] = i;
							material->ChangeValue(p, floatValues);
							material->MakeDirty(node);
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

				if (ImGui::BeginCombo(nameStr.c_str(), items[static_cast<int>(floatValues[0])]))
				{
					for (size_t i = 0; i < 2; i++)
					{
						auto isSelected = static_cast<int>(floatValues[0]) == i;
						if (ImGui::Selectable(items[i], isSelected))
						{
							floatValues[0] = i;
							material->ChangeValue(p, floatValues);
							material->MakeDirty(node);
						}
						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
			}
		}
		else if (type == ValueType::Gradient)
		{
			assert(p->GradientData != nullptr);

			ImGradientHDRState state;

			state.ColorCount = p->GradientData->ColorCount;

			for (int i = 0; i < state.ColorCount; i++)
			{
				state.Colors[i].Color = p->GradientData->Colors[i].Color;
				state.Colors[i].Intensity = p->GradientData->Colors[i].Intensity;
				state.Colors[i].Position = p->GradientData->Colors[i].Position;
			}

			state.AlphaCount = p->GradientData->AlphaCount;

			for (int i = 0; i < state.AlphaCount; i++)
			{
				state.Alphas[i].Alpha = p->GradientData->Alphas[i].Alpha;
				state.Alphas[i].Position = p->GradientData->Alphas[i].Position;
			}

			ImGradientHDRTemporaryState tempState;

			ImGui::PushID(node->GUID);

			const int idSelectedMarkerType = 100;
			const int idSelectedIndex = 101;
			const int idDraggingMarkerType = 102;
			const int idDraggingIndex = 103;

			tempState.selectedMarkerType = static_cast<ImGradientHDRMarkerType>(ImGui::GetStateStorage()->GetInt(idSelectedMarkerType, static_cast<int>(ImGradientHDRMarkerType::Unknown)));
			tempState.selectedIndex = ImGui::GetStateStorage()->GetInt(idSelectedIndex, -1);
			tempState.draggingMarkerType = static_cast<ImGradientHDRMarkerType>(ImGui::GetStateStorage()->GetInt(idDraggingMarkerType, static_cast<int>(ImGradientHDRMarkerType::Unknown)));
			tempState.draggingIndex = ImGui::GetStateStorage()->GetInt(idDraggingIndex, -1);

			if (ImGradientHDR(node->GUID, state, tempState))
			{
				material->MakeDirty(node);
			}

			if (tempState.selectedMarkerType == ImGradientHDRMarkerType::Color)
			{
				auto selectedColorMarker = state.GetColorMarker(tempState.selectedIndex);
				if (selectedColorMarker != nullptr)
				{
					if (ImGui::ColorEdit3("Color", selectedColorMarker->Color.data(), ImGuiColorEditFlags_Float))
					{
						material->MakeDirty(node);
					}

					if (ImGui::DragFloat("Intensity", &selectedColorMarker->Intensity, 0.1f, 0.0f, 100.0f, "%f", 1.0f))
					{
						material->MakeDirty(node);
					}
				}
			}

			if (tempState.selectedMarkerType == ImGradientHDRMarkerType::Alpha)
			{
				auto selectedAlphaMarker = state.GetAlphaMarker(tempState.selectedIndex);
				if (selectedAlphaMarker != nullptr)
				{
					if (ImGui::DragFloat("Alpha", &selectedAlphaMarker->Alpha, 0.1f, 0.0f, 1.0f, "%f", 1.0f))
					{
						material->MakeDirty(node);
					}
				}
			}

			if (tempState.selectedMarkerType != ImGradientHDRMarkerType::Unknown)
			{
				if (ImGui::Button("Delete"))
				{
					material->MakeDirty(node);

					if (tempState.selectedMarkerType == ImGradientHDRMarkerType::Color)
					{
						state.RemoveColorMarker(tempState.selectedIndex);
						tempState = ImGradientHDRTemporaryState{};
					}
					else if (tempState.selectedMarkerType == ImGradientHDRMarkerType::Alpha)
					{
						state.RemoveAlphaMarker(tempState.selectedIndex);
						tempState = ImGradientHDRTemporaryState{};
					}
				}
			}

			{
				p->GradientData->ColorCount = state.ColorCount;

				for (int i = 0; i < state.ColorCount; i++)
				{
					p->GradientData->Colors[i].Color = state.Colors[i].Color;
					p->GradientData->Colors[i].Intensity = state.Colors[i].Intensity;
					p->GradientData->Colors[i].Position = state.Colors[i].Position;
				}

				p->GradientData->AlphaCount = state.AlphaCount;

				for (int i = 0; i < state.AlphaCount; i++)
				{
					p->GradientData->Alphas[i].Alpha = state.Alphas[i].Alpha;
					p->GradientData->Alphas[i].Position = state.Alphas[i].Position;
				}
			}

			ImGui::GetStateStorage()->SetInt(idSelectedMarkerType, static_cast<int>(tempState.selectedMarkerType));
			ImGui::GetStateStorage()->SetInt(idSelectedIndex, static_cast<int>(tempState.selectedIndex));
			ImGui::GetStateStorage()->SetInt(idDraggingMarkerType, static_cast<int>(tempState.draggingMarkerType));
			ImGui::GetStateStorage()->SetInt(idDraggingIndex, static_cast<int>(tempState.draggingIndex));

			ImGui::PopID();
		}
		else
		{
			assert(0);
		}

		auto descStr = StringContainer::GetValue((name + "_Desc").c_str(), "");

		if (ImGui::IsItemHovered() && !ImGui::IsItemActive() && descStr != "")
		{
			ImGui::SetTooltip(descStr.c_str());
		}
	};

	for (size_t i = 0; i < node->Properties.size(); i++)
	{
		auto p = node->Properties[i];
		auto pp = node->Parameter->Properties[i];

		updateProp(pp->Type, pp->Name, p);
	}

	// cutomdata
	if (node->Parameter->Type == NodeType::CustomData1)
	{
		if (ImGui::DragFloat4(StringContainer::GetValue("Value_Name").c_str(), material->CustomData[0].Values.data(), 0.01f))
		{
		}
	}
	if (node->Parameter->Type == NodeType::CustomData2)
	{
		if (ImGui::DragFloat4(StringContainer::GetValue("Value_Name").c_str(), material->CustomData[1].Values.data(), 0.01f))
		{
		}
	}

	// name and description for other editor

	if (node->Parameter->HasDescription)
	{
		ImGui::Separator();
		ImGui::Text(StringContainer::GetValue("Description_Name").c_str());

		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip(StringContainer::GetValue("Description_Desc").c_str());
		}

		const char* languages[] = {"Ja", "En"};

		if (ImGui::BeginCombo(StringContainer::GetValue("Language_Name").c_str(),
							  (StringContainer::GetValue(languages[static_cast<int>(material->Language)]) + "###" +
							   languages[static_cast<int>(material->Language)])
								  .c_str()))
		{
			for (size_t i = 0; i < 2; i++)
			{
				auto isSelected = static_cast<int>(material->Language) == i;
				if (ImGui::Selectable((StringContainer::GetValue(languages[i]) + "###" + languages[i]).c_str(), isSelected))
				{
					material->Language = static_cast<LanguageType>(i);
				}
				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		// is memory safe?
		auto name = node->Descriptions[static_cast<int>(material->Language)]->Summary;
		name.resize(name.size() + 256, 0);

		auto desc = node->Descriptions[static_cast<int>(material->Language)]->Detail;
		desc.resize(desc.size() + 256, 0);

		if (ImGui::InputText(StringContainer::GetValue("Summary_Name").c_str(), const_cast<char*>(name.data()), name.size()))
		{
			node->Descriptions[static_cast<int>(material->Language)]->Summary = name.c_str();
		}

		if (ImGui::InputTextMultiline(StringContainer::GetValue("Detail_Name").c_str(), const_cast<char*>(desc.data()), desc.size()))
		{
			node->Descriptions[static_cast<int>(material->Language)]->Detail = desc.c_str();
		}
	}
}

void Editor::UpdatePreview()
{
	ImVec2 size;
	size.x = Preview::TextureSize;
	size.y = Preview::TextureSize;
	ImGui::Image((void*)preview_->GetInternal(), size, ImVec2(0.0, 1.0), ImVec2(1.0, 0.0));

	if (ImGui::ImageButton((ImTextureID)previewTypeButtons_[0]->GetInternal(), ImVec2(20.0, 20.0), ImVec2(0.0, 1.0), ImVec2(1.0, 0.0)))
	{
		preview_->ModelType = PreviewModelType::Screen;
	}

	ImGui::SameLine();

	if (ImGui::ImageButton((ImTextureID)previewTypeButtons_[1]->GetInternal(), ImVec2(20.0, 20.0), ImVec2(0.0, 1.0), ImVec2(1.0, 0.0)))
	{
		preview_->ModelType = PreviewModelType::Sphere;
	}

	{
		auto textureNumHeader = StringContainer::GetValue("TextureCount", "Texture");

		if (previewTextureCount_ > Effekseer::UserTextureSlotMax)
		{
			ImGui::TextColored(
				ImColor(255, 0, 0, 255), (textureNumHeader + " %d / %d").c_str(), previewTextureCount_, Effekseer::UserTextureSlotMax);
		}
		else
		{
			ImGui::Text((textureNumHeader + " %d / %d").c_str(), previewTextureCount_, Effekseer::UserTextureSlotMax);
		}
	}

	// TODO : refactor
	{
		auto uniformNumHeader = StringContainer::GetValue("UniformCount", "Uniform");
		const int uniformMax = 16;

		if (previewUniformCount_ > uniformMax)
		{
			ImGui::TextColored(
				ImColor(255, 0, 0, 255), (uniformNumHeader + " %d / %d").c_str(), previewUniformCount_, uniformMax);
		}
		else
		{
			ImGui::Text((uniformNumHeader + " %d / %d").c_str(), previewUniformCount_, uniformMax);
		}
	}

	{
		const auto uniformNumHeader = StringContainer::GetValue("GradientCount", "Gradient");
		const int uniformMax = 2;

		if (previewGradientCount_ > uniformMax)
		{
			ImGui::TextColored(
				ImColor(255, 0, 0, 255), (uniformNumHeader + " %d / %d").c_str(), previewGradientCount_, uniformMax);
		}
		else
		{
			ImGui::Text((uniformNumHeader + " %d / %d").c_str(), previewGradientCount_, uniformMax);
		}
	}
}

void Editor::UpdateToRecordMovingCommand()
{
	auto material = GetSelectedMaterial();
	if (material == nullptr)
	{
		return;
	}

	std::vector<std::shared_ptr<Node>> nodes;
	std::vector<Vector2DF> poses;
	std::vector<Vector2DF> sizes;

	for (auto node : material->GetNodes())
	{
		if (contents_[GetSelectedContentIndex()]->IsLoading || node->GetIsPosDirtied())
			continue;

		auto nodePos = ed::GetNodePosition(node->GUID);
		auto nodeSize = GetNodeGroupSize(node->GUID);
		if (nodePos.x != node->Pos.X || nodePos.y != node->Pos.Y || node->CommentSize.X != nodeSize.X || node->CommentSize.Y != nodeSize.Y)
		{
			nodes.push_back(node);
			poses.push_back(Vector2DF(nodePos.x, nodePos.y));
			sizes.push_back(nodeSize);
		}
	}

	if (nodes.size() == 1)
	{
		nodes[0]->UpdateRegion(Vector2DF(poses[0].X, poses[0].Y), sizes[0]);
	}
	else if (nodes.size() > 0)
	{
		material->ApplyMoveNodesMultiply(nodes, poses);
	}
}

void Editor::UpdateNode(std::shared_ptr<Node> node)
{
	auto applyPosition = [&]() -> void
	{
		if (contents_[GetSelectedContentIndex()]->IsLoading || node->GetIsPosDirtied())
		{
			ed::SetNodePosition(node->GUID, ImVec2(node->Pos.X, node->Pos.Y));
		}
	};

	if (node->Parameter->Type == NodeType::Comment)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);

		ed::BeginNode(node->GUID);

		applyPosition();

		ImGui::BeginVertical("content");
		ImGui::BeginHorizontal("horizontal");
		ImGui::Text(node->Properties[0]->Str.c_str());
		ImGui::EndHorizontal();

		auto editor = reinterpret_cast<ax::NodeEditor::Detail::EditorContext*>(ed::GetCurrentEditor());
		auto nodeContext = editor->GetNode(node->GUID);

		ImVec2 size{0, 0};
		if (nodeContext != nullptr)
		{
			size = nodeContext->m_GroupBounds.GetSize();
		}
		size.x = size.x < 64 ? 64 : size.x;
		size.y = size.y < 64 ? 64 : size.y;

		// auto size = ed::GetNodeSize(node->GUID);

		// initialize
		if (contents_[GetSelectedContentIndex()]->IsLoading || node->GetIsPosDirtied())
		{
			size.x = node->CommentSize.X;
			size.y = node->CommentSize.Y;
			nodeContext->m_GroupBounds.Max = nodeContext->m_GroupBounds.Min + size;
		}

		ed::Group(size);
		ImGui::EndVertical();
		ed::EndNode();

		ImGui::PopStyleVar();

		if (ed::BeginGroupHint(node->GUID))
		{
			ed::EndGroupHint();
		}

		node->ClearPosDirtied();

		return;
	}

	// except a comment node

	ed::BeginNode(node->GUID);

	applyPosition();

	ImGui::PushID(node->GUID);

	ImGui::BeginVertical("node");

	// Header
	ImGui::BeginHorizontal("header");

	std::string headerName;
	bool found = false;
	for (auto& behavior : node->Parameter->BehaviorComponents)
	{
		if (behavior->IsGetHeaderInherited)
		{
			headerName = behavior->GetHeader(GetSelectedMaterial(), node->Parameter, node);
			found = true;
		}
	}

	if (!found)
	{
		headerName = node->Parameter->GetHeader(GetSelectedMaterial(), node);
	}

	ImGui::Text(headerName.c_str());

	// show preview button
	ImGui::Spring(1, 0);

	if (node->IsPreviewOpened)
	{
		if (ImGui::SmallButton("+"))
		{
			node->IsPreviewOpened = !node->IsPreviewOpened;
		}
	}
	else
	{
		if (ImGui::SmallButton("-"))
		{
			node->IsPreviewOpened = !node->IsPreviewOpened;
		}
	}

	ImGui::Spring(0, 0);

	ImGui::EndHorizontal();

	// Input and output
	ImGui::BeginHorizontal("inout_");

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

			auto text = typeShape + std::string(" ") +
						StringContainer::GetValue((pin->Parameter->Name + "_Name").c_str(), pin->Parameter->Name.c_str());

			if (pin->IsEnabled)
			{
				ImGui::Text(text.c_str());
			}
			else
			{
				ImGui::TextColored(ImColor(100, 100, 100), text.c_str());
			}

			ImGui::EndHorizontal();

			ed::EndPin();
		}

		// To save space, put under inputs
		if (node->IsPreviewOpened)
		{
			auto preview = (NodeUserDataObject*)node->UserObj.get();
			if (preview != nullptr)
			{
				ImVec2 size;
				size.x = Preview::TextureSize;
				size.y = Preview::TextureSize;
				ImGui::Image((void*)preview->GetPreview()->GetInternal(), size, ImVec2(0.0, 1.0), ImVec2(1.0, 0.0));
			}
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

			ImGui::Text((StringContainer::GetValue((pin->Parameter->Name + "_Name").c_str(), pin->Parameter->Name.c_str()) +
						 std::string(" ") + typeShape)
							.c_str());

			ImGui::EndHorizontal();

			ed::EndPin();
		}

		ed::PopStyleVar(2);
		ImGui::Spring(1, 0);
		ImGui::EndVertical();
	}

	ImGui::EndHorizontal();

	// show a preview
	if (node->IsPreviewOpened)
	{
		auto preview = (NodeUserDataObject*)node->UserObj.get();
		if (preview != nullptr)
		{
			if (ImGui::ImageButton(
					(ImTextureID)previewTypeButtons_[0]->GetInternal(), ImVec2(20.0, 20.0), ImVec2(0.0, 1.0), ImVec2(1.0, 0.0)))
			{
				preview->GetPreview()->ModelType = PreviewModelType::Screen;
			}

			ImGui::SameLine();

			if (ImGui::ImageButton(
					(ImTextureID)previewTypeButtons_[1]->GetInternal(), ImVec2(20.0, 20.0), ImVec2(0.0, 1.0), ImVec2(1.0, 0.0)))
			{
				preview->GetPreview()->ModelType = PreviewModelType::Sphere;
			}
		}
	}

	// show an warning
	if (node->CurrentWarning != WarningType::None)
	{
		if (node->CurrentWarning == WarningType::DifferentSampler)
		{
			ImGui::TextColored(ImColor(1.0f, 0.0f, 0.0f, 1.0f), StringContainer::GetValue("Warning_DifferentSampler").c_str());
		}
		else if (node->CurrentWarning == WarningType::WrongInputType)
		{
			ImGui::TextColored(ImColor(1.0f, 0.0f, 0.0f, 1.0f), StringContainer::GetValue("Warning_WrongInputType").c_str());
		}
		else if (node->CurrentWarning == WarningType::WrongProperty)
		{
			ImGui::TextColored(ImColor(1.0f, 0.0f, 0.0f, 1.0f), StringContainer::GetValue("Warning_WrongProperty").c_str());
		}
		else if (node->CurrentWarning == WarningType::SameName)
		{
			ImGui::TextColored(ImColor(1.0f, 0.0f, 0.0f, 1.0f), StringContainer::GetValue("Warning_SameName").c_str());
		}
		else if (node->CurrentWarning == WarningType::InvalidName)
		{
			ImGui::TextColored(ImColor(1.0f, 0.0f, 0.0f, 1.0f), StringContainer::GetValue("Warning_InvalidName").c_str());
		}
		else if (node->CurrentWarning == WarningType::PixelNodeAndNormal)
		{
			ImGui::TextColored(ImColor(1.0f, 0.0f, 0.0f, 1.0f), StringContainer::GetValue("Warning_PixelNodeAndNormal").c_str());
		}
		else
		{
			assert(0);
		}
	}

	ImGui::EndVertical();

	ImGui::PopID();
	ed::EndNode();

	auto itemRectMin = ImGui::GetItemRectMin();
	auto itemRectMax = ImGui::GetItemRectMax();

	auto bg_drawList = ed::GetNodeBackgroundDrawList(node->GUID);

	// The background of header
	auto mainWindow = Effekseer::MainWindow::GetInstance();
	bg_drawList->AddRectFilled(ImVec2(itemRectMin.x, itemRectMin.y),
							   ImVec2(itemRectMax.x, itemRectMin.y + 30 * mainWindow->GetDPIScale()),
							   IM_COL32(110, 110, 110, 255),
							   ed::GetStyle().NodeRounding,
							   ImDrawCornerFlags_Top);

	node->ClearPosDirtied();
}

void Editor::UpdateLink(std::shared_ptr<Link> link)
{
	ed::Link(link->GUID, link->InputPin->GUID, link->OutputPin->GUID);
}

bool Editor::GetIsSelectedDirty()
{
	auto ret = isSelectedDirty_;
	isSelectedDirty_ = false;
	return ret;
}

void Editor::ClearDirtiedSelectedFlags()
{
	isSelectedDirty_ = false;
}

} // namespace EffekseerMaterial
